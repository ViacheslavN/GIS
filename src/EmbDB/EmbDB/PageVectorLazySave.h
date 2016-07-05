#ifndef _EMBEDDED_DATABASE_PAGE_LIST_
#define _EMBEDDED_DATABASE_PAGE_LIST_
#include "PageVector.h"
#include "FilePage.h"

namespace embDB
{

	template<typename _TValue, typename _TReaderWriter = TSimpleReaderWriter<_TValue> >
	class TPageVectorLazySave
	{

		typedef _TValue   TValue;
		typedef _TReaderWriter TReaderWriter;
		typedef std::vector<TValue> TVecValues;
	public:

		TPageVectorLazySave(int64 nPage, uint32 nPageSize, uint16 nObjectPage, uint16 nSubObjectPage) :
		  m_nRootPage(nPage), m_nPageSize(nPageSize), m_nObjectPage(nObjectPage), 
			  m_nSubObjectPage(nSubObjectPage), m_nPageAddr(nPage)
		{}
		~TPageVectorLazySave(){}

		void setRoot(int64 nPage)
		{
			m_nRootPage = nPage;
			m_nPageAddr = nPage;
		}

		template <typename _TStorage, typename _TFilePage>
		bool push( const TValue& val, _TStorage *pStorage)
		{
			if(!checkSize())
			{
				if(!save<_TStorage, _TFilePage>(pStorage))
					return false;
			}
			m_values.push_back(val);
			return true;
		}

		bool checkSize()
		{
			uint32 nMemSize = (((uint32)m_values.size() + 1) * m_rw.rowSize()) + sizeof(int64) + sizeof(int32) + sFilePageHeader::size();
			return nMemSize < m_nPageSize; 
		}
		template <typename _TStorage, typename _FilePage>
		bool save(_TStorage *pStorage)
		{
			_FilePage pPage = pStorage->getFilePage(m_nPageAddr, m_nPageSize, false);
			if(pPage == NULL)
			{
				//TO DO Logs
				return false;
			}
		 
			_FilePage pNextPage(NULL);
			if(!checkSize())
			{
				pNextPage = pStorage->getNewPage(m_nPageSize);
				if(pNextPage == NULL)
				{
					//TO DO Logs
					return false;
				}
			}
			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, m_nObjectPage, m_nSubObjectPage, pPage->getPageSize());
			stream.write(pNextPage != NULL? pNextPage->getAddr() : (int64)-1);
			stream.write((int32)m_values.size());

			for (uint32 i = 0, sz = (uint32)m_values.size(); i < sz; ++i)
			{
				m_rw.write(m_values[i], stream);
			}
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pStorage->saveFilePage(pPage);

			m_values.clear();
			if(pNextPage != NULL)
				m_nPageAddr = pNextPage->getAddr();
			return true;
		}

		template <typename _TStorage>
		struct iterator
		{
		public:
			iterator(int64 nPage, _TStorage *pStorage, TReaderWriter *pRW, uint32 nPageSize, short nObjectPage, short nSubObjectPage)
				: m_nPage(nPage), pStorage(pStorage), pRW(pRW), m_nPageSize(nPageSize), m_nObjectPage(nObjectPage),  m_nSubObjectPage(nSubObjectPage),  m_nNextPage(-1), m_nPageIDx(0)
			{

			}
			bool isNull()
			{
				if(m_nPage != -1 && m_nPageIDx <  (int32)m_vecCurValues.size() && !m_vecCurValues.empty())
					return false;
				return true;
			}
			bool LoadNext()
			{
				m_vecCurValues.clear();
				m_nPageIDx = 0;
				m_nPage = m_nNextPage;
				if(m_nPage == -1)
				{
					return false;
				}
				return load();

			}

			TValue& value()
			{
				return m_vecCurValues[m_nPageIDx];
			}
			bool next()
			{
				m_nPageIDx++;
				if(m_nPageIDx < (int32)m_vecCurValues.size() )
					return false;
				return LoadNext();
			}
			bool load()
			{
				if(m_nPage == -1 )
					return false;
				CFilePage *pPage = pStorage->getFilePage(m_nPage, m_nPageSize);
				if(!pPage)
				{
					//TO DO Logs
					return false;
				}

				CommonLib::FxMemoryReadStream stream;
				stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, pPage->getPageSize());
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
					m_vecCurValues.push_back(val);
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
			int32 m_nPageIDx;
			TVecValues m_vecCurValues;
			uint32 m_nPageSize;
		};
		template <typename _TStorage>
		iterator<_TStorage> begin(_TStorage *pStorage)
		{
			return iterator<_TStorage> (m_nRootPage, pStorage, &m_rw, m_nPageSize, m_nObjectPage, m_nSubObjectPage);
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