#ifndef _EMBEDDED_DATABASE_COUNTER_H_
#define _EMBEDDED_DATABASE_COUNTER_H_

#include "CommonLibrary/FixedMemoryStream.h"

namespace embDB
{

	template<class TValue>
	class TCounter
	{
		public:
			TCounter(short nObjectPage, short nSubObjectPage, uint32 nPageSize, TValue val = 1, int64 nPage = -1) : m_nVal(val), m_nPage(-1),
					m_nPageSize(nPageSize), m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage)
			{

			}
			~TCounter(){}

			TValue GetNext() {return m_nVal++;}

			void SetPage(int64 nPage)
			{
				m_nPage = nPage;
			}
			int64 GetPage() const{ return m_nPage;}
			void SetValue(TValue val)
			{
				m_nVal = val;
			}
			template<class TStorage>
			bool load(TStorage *pStorage)
			{
				FilePagePtr pPage = pStorage->getFilePage(m_nPage, m_nPageSize);
				if(!pPage.get())
					return false; //TO DO Log;

				CommonLib::FxMemoryReadStream stream;
				stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, pPage->getPageSize());
				if(!header.isValid())
				{
					//TO DO Log
					return false;
				}
				if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				{
					//TO DO Log
					return false;
				}
				stream.read(m_nVal);

				return true;

			}
			template<class TStorage>
			bool save(TStorage *pStorage)
			{
				FilePagePtr pPage = pStorage->getFilePage(m_nPage, m_nPageSize);
				if(!pPage.get())
					return false; //TO DO Log;
				CommonLib::FxMemoryWriteStream stream;
				stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, m_nObjectPage, m_nSubObjectPage, pPage->getPageSize());
				stream.write(m_nVal);
				header.writeCRC32(stream);
				pPage->setFlag(eFP_CHANGE, true);
				pStorage->saveFilePage(pPage);
				return true;
			}
		private:
			TValue m_nVal;
			int64 m_nPage;
			uint32 m_nPageSize;
			short m_nObjectPage;
			short m_nSubObjectPage;
	};

}

#endif