#ifndef _EMBEDDED_DATABASE_PAGE_LIST_
#define _EMBEDDED_DATABASE_PAGE_LIST_
#include "PageVector.h"

namespace embDB
{

	template<typename _TValue, typename _TReaderWriter = TSimpleReaderWriter<_TValue> >
	class TPageList
	{

		typedef _TValue   TValue;
		typedef _TReaderWriter TReaderWriter;
		typedef std::vector<TValue> TVecValues;
	public:

		TPageList(int64 nPage, uint32 nPageSize, short nObjectPage, short nSubObjectPage) :
		  m_nRootPage(nPage), m_nPageSize(nPageSize), m_nObjectPage(nObjectPage), 
			  m_nSubObjectPage(nSubObjectPage), m_nPageAddr(nPage)
		{}
		~TPageList(){}

		void setRoot(int64 nPage)
		{
			m_nRootPage = nPage;
			m_nPageAddr = nPage;
		}

		template <typename _TStorage>
		bool push( const TValue& val, _TStorage *pStorage)
		{
			if(!checkSize())
			{
				if(!save(pStorage))
					return false;
			}
			m_values.push_back(val);
			return true;
		}

		bool checkSize()
		{
			size_t nMemSize = (m_values.size() * (m_rw.rowSize() + 1)) + sizeof(int64) + sizeof(int32) + sFilePageHeader::size();
			return nMemSize < m_nPageSize; 
		}
		template <typename _TStorage>
		bool save(_TStorage *pStorage)
		{
			CFilePage *pPage = pStorage->getFilePage(m_nPageAddr);
			if(!pPage)
			{
				//TO DO Logs
				return false;
			}
		 
			CFilePage* pNextPage = NULL;
			if(!checkSize())
			{
				pNextPage = pStorage->getNewPage();
				if(!pNextPage)
				{
					//TO DO Logs
					return false;
				}
			}
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, m_nObjectPage, m_nSubObjectPage);

			stream.write(pNextPage ? pNextPage->getAddr() : -1);
			stream.write((int32)m_values.size());

			for (size_t i = 0, sz = m_values.size(); i < sz; ++i)
			{
				m_rw.write(m_values[i], stream);
			}
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pStorage->saveFilePage(pPage);

			m_values.clear();
			if(pNextPage)
				m_nPageAddr = pNextPage->getAddr();
			return true;
		}

		template <typename _TStorage>
		struct iterator
		{
		public:
			iterator(int64 nPage, _TStorage *pStorage, TReaderWriter *pRW, short nObjectPage, short nSubObjectPage)
				: m_nPage(nPage), pStorage(pStorage), pRW(pRW),  m_nSubObjectPage(nSubObjectPage),  m_nNextPage(-1)
			{

			}
			bool isEnd()
			{
				return m_nPage == -1;
			}
			bool nextPage()
			{
				m_nPage = m_nNextPage;
				return m_nPage != -1;
			}
			bool getValues(TVecValues &vecValues)
			{
				if(isEnd())
					return false;
				CFilePage *pPage = pStorage->getFilePage(m_nPage);
				if(!pPage)
				{
					//TO DO Logs
					return false;
				}

				CommonLib::FxMemoryReadStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream);
				if(!header.isValid())
				{
					//TO DO Logs
					return false;
				}
				if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				{
						//TO DO Logs
					return false;
				}

				m_nNextPage = stream.readInt64();
				int32 nSize = stream.readInt32();
				for(int32 i = 0; i < nSize; ++i)
				{
					TValue val;
					pRW->read(val, stream);
					vecValues.push_back(val);
				}
				return true;
			}

		
		private:

			int64 m_nPage;
			int64 m_nNextPage;
			_TStorage *pStorage;
			TReaderWriter *pRW;
			short m_nObjectPage; 
			short m_nSubObjectPage;

		};
		template <typename _TStorage>
		iterator<_TStorage> begin(_TStorage *pStorage)
		{
			return iterator<_TStorage> (m_nRootPage, pStorage, &m_rw, m_nObjectPage, m_nSubObjectPage);
		}
	private:
		int64 m_nRootPage;
		uint32 m_nPageSize;
		short m_nObjectPage; 
		short m_nSubObjectPage;

		TVecValues m_values;
		int64 m_nPageAddr;

		TReaderWriter m_rw;

	};
}
#endif