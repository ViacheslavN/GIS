#ifndef _EMBEDDED_DATABASE_PAGE_VECTOR_
#define _EMBEDDED_DATABASE_PAGE_VECTOR_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include <vector>
#include <set>
#include <algorithm>
#include "FilePage.h"
#include "RBMap.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{


template<typename _TValue>
class TSimpleReaderWriter
{
public:
	typedef _TValue   TValue;
	void write(const TValue& value, CommonLib::FxMemoryWriteStream& stream)
	{
		stream.write(value);
	}
	void read(TValue& value, CommonLib::FxMemoryReadStream& stream)
	{
		stream.read(value);
	}

	size_t rowSize()
	{
		return sizeof(TValue);
	}
};

template<typename _TValue, typename _TReaderWriter = TSimpleReaderWriter<_TValue> >
class TPageVector
{
public:
	typedef _TValue   TValue;
	typedef _TReaderWriter TReaderWriter;

	typedef std::vector<TValue> TvecValues;
	struct SPageValues
	{
		SPageValues(int64 nPageAddr) : 
			m_nPageAddr(nPageAddr), m_pNext(NULL), m_pPrev(NULL)
		{}
		int64 m_nPageAddr;
		SPageValues* m_pNext;
		SPageValues* m_pPrev;
		TvecValues m_vec;
	
	
		bool isFree(uint32 nPageSize)
		{
			return (sFilePageHeader::size() + sizeof(int64) + sizeof(int32) + (sizeof(TValue) * (m_vec.size() + 1))) < nPageSize;
		}
		void insert(const TValue& val)
		{
			m_vec.push_back(val);
		}
		size_t size() {return m_vec.size();}
		void save(TReaderWriter& rw, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = m_vec.size();
			stream.write(nSize);
		
			for (size_t idx = 0, sz = m_vec.size(); idx < m_vec.size(); ++idx)
			{
				//stream.write(m_vec[idx]);
				rw.write(m_vec[idx], stream);
			}
		}
		void load(TReaderWriter& rw, CommonLib::FxMemoryReadStream& stream)
		{
			uint32 nSize = stream.readInt32();
			if(nSize == 0)
				return;
			for (uint32 idx = 0; idx < nSize; ++idx)
			{
				TValue val;
				//stream.read(val);
				rw.read(val, stream);
				m_vec.push_back(val);
			}
		
		}

		bool remove(const TValue& val)
		{
			typename TvecValues::iterator it = std::find(m_vec.begin(), m_vec.end(), val);
			if(it == m_vec.end())
				return false;
			m_vec.erase(it);
			return true;
		}
	};


	TPageVector(int64 nPage, uint32 nPageSize, short nObjectPage, short nSubObjectPage) : 
	m_nFirstPage(nPage), m_nPageSize(nPageSize), m_pBeginNode(NULL), m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage)
	{
	
	}

	~TPageVector()
	{
		clear();
	}
	void clear()
	{
		if(!m_pBeginNode)
			return;
		SPageValues *pNode = m_pBeginNode;
		while(pNode)
		{
			SPageValues *pNextNode = pNode->m_pNext;
			delete pNode;
			pNode = pNextNode;
		}
		m_pBeginNode = NULL;
	}
	void setFirstPage(int64 nPage)
	{
		m_nFirstPage = nPage;
	}
	void setPageSize(uint32 nPageSize)
	{
		m_nPageSize = nPageSize;
	}
	template <typename _TStorage>
	bool remove( const TValue& value, _TStorage *pStorage)
	{
		SPageValues *pNode = m_pBeginNode;
		if(!pNode)
			return false;

		while(pNode)
		{
			if(pNode->remove(value))
				break;
			pNode = pNode->m_pNext;
		}
		if(pNode)
		{
			if(pNode->size())
			{
				return SavePage(pNode, pStorage);
			}
			if(pNode == m_pBeginNode)
			{
				if(!pNode->m_pNext)
					return SavePage(pNode, pStorage);

				SPageValues* pNext =  pNode->m_pNext;
				int64 nNextPageAddr = pNext->m_nPageAddr;
				pNext->m_nPageAddr = m_nFirstPage;
				pStorage->dropFilePage(nNextPageAddr, m_nPageSize);
				delete pNode;
				m_pBeginNode = pNext;
				return SavePage(m_pBeginNode, pStorage);
			}
			if(pNode->m_pNext)
				pNode->m_pNext->m_pPrev = pNode->m_pPrev;

			assert(pNode->m_pPrev);
			SPageValues* pPrev = pNode->m_pPrev;
			pPrev = pNode->m_pNext;
			pStorage->dropFilePage(pNode->m_nPageAddr, m_nPageSize);
			delete pNode;
			return SavePage(pPrev, pStorage);
		}
		return false;
	}

	template <typename _TStorage>
	bool push( const TValue& val, _TStorage *pStorage)
	{
		if(!m_pBeginNode)
		{
			m_pBeginNode = new SPageValues(m_nFirstPage);
			m_pBeginNode->insert(val);
			return SavePage(m_pBeginNode, pStorage);
		}
		SPageValues *pNode = m_pBeginNode;
		SPageValues *pLastNode = m_pBeginNode;
		while(pNode)
		{
			pLastNode = pNode;
			if(pNode->isFree(m_nPageSize))
			{
				pNode->insert(val);
				return SavePage(pNode, pStorage);
			}
			pNode = pNode->m_pNext;
		}

		pNode = new SPageValues(-1);
		pNode->insert(val);
		if(!SavePage(pNode, pStorage))
			return false;
		assert(!pLastNode->m_pNext);
		pLastNode->m_pNext = pNode;
		pNode->m_pPrev = pLastNode;
		return SavePage(pLastNode, pStorage);
	}
	template <typename _TStorage>
	bool load(_TStorage *pStorage)
	{
		clear();
		int64 nPage = m_nFirstPage;
		SPageValues* pLastNode = NULL;
		while(nPage != -1)
		{
		//	m_nLastPage = nPage;
			FilePagePtr pPage(pStorage->getFilePage(nPage, m_nPageSize));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream);
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
			int64 nNextPage = stream.readInt64(); // next
			SPageValues* pNode =  new SPageValues(nPage);
			if(!m_pBeginNode)
				m_pBeginNode = pNode;
			else
			{
				pLastNode->m_pNext = pNode;
				pNode->m_pPrev = pLastNode;
			}
			
			pNode->load(m_rw, stream);
			nPage = nNextPage;
			pLastNode = pNode;
		}
		return true;
	}
	template <typename _TStorage>
	bool SavePage(SPageValues* pNode, _TStorage *pStorage)
	{
		FilePagePtr pPage(NULL);
		if(pNode->m_nPageAddr == -1)
		{
			pPage = pStorage->getNewPage(m_nPageSize);
			if(!pPage.get())
				return false;
			pNode->m_nPageAddr = pPage->getAddr();
		}
		else
			pPage = pStorage->getFilePage(pNode->m_nPageAddr, m_nPageSize);
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, m_nObjectPage, m_nSubObjectPage);
		stream.write(pNode->m_pNext ? pNode->m_pNext->m_nPageAddr : -1);
		pNode->save(m_rw, stream);
		header.writeCRC32(stream);
		pPage->setFlag(eFP_CHANGE, true);
		pStorage->saveFilePage(pPage);
		return true;
	}
	template <typename _TStorage>
	bool save(_TStorage *pStorage)
	{
		if(!m_pBeginNode)
		{
			m_pBeginNode = new SPageValues(m_nFirstPage);
		}
		SPageValues *pNode = m_pBeginNode;
		while(pNode)
		{
			if(!SavePage(pNode, pStorage))
				return false;
			pNode = pNode->m_pNext;
		}
		return true;
	}


	struct iterator
	{
	public:
		
		iterator(SPageValues *pNode) : m_nPageIDx(0), m_pPageNode(pNode)
		{}
		bool isNull()
		{
			if(!m_pPageNode)
				return true;
			if(!m_pPageNode->size() || m_nPageIDx >= m_pPageNode->size())
				return true;
			return false;
		}
	TValue& value()
	{
		return m_pPageNode->m_vec[m_nPageIDx];
	}
	bool next()
	{
		m_nPageIDx++;

		if(m_nPageIDx >= m_pPageNode->size())
		{
			m_nPageIDx = 0;
			m_pPageNode = m_pPageNode->m_pNext;
			if(!m_pPageNode)
				return false;
			if(!m_pPageNode->size())
				return false;
		}
		return true;

	}
	private:
		size_t m_nPageIDx;
		SPageValues *m_pPageNode;
	};

	iterator begin()
	{
		return iterator(m_pBeginNode);
	}
private:
	int64 m_nFirstPage;
	//int64 m_nLastPage;
	uint32 m_nPageSize;

	//TPages m_pages;
	SPageValues* m_pBeginNode;
	short m_nObjectPage;
	short m_nSubObjectPage;
	TReaderWriter m_rw;

};

}
#endif