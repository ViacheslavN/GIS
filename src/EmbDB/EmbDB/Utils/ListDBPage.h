#ifndef _EMBEDDED_DATABASE_LIST_DB_FIELD_H_
#define _EMBEDDED_DATABASE_LIST_DB_FIELD_H_
#include "CommonLibrary/FixedMemoryStream.h"
#include "../storage/storage.h"
namespace embDB
{
 
	class CFilePage;

	template <typename _TStorage>
	class ListDBPage
	{
	public:
		typedef _TStorage TStorage;
		ListDBPage(TStorage *pStorage, int64 nKeySymbol) :
			m_pStorage(pStorage)
			,m_nKeySymbol(nKeySymbol)
			,m_nNext(-1)
			,m_nPrev(-1)
			,m_nAddr(-1)
			{

			}
		~ListDBPage(){}
		bool load(CommonLib::FxMemoryReadStream *pStream, int64 nAddr)
		{
			if(nAddr == -1)
				return false;
			CFilePage * pPage = m_pStorage->getFilePage(nAddr);
			if(!pPage)
				return false;
			pStream->attach(pPage->getRowData(), pPage->getPageSize());
			int64 nSymbol = pStream->readInt64();
			if(nSymbol != m_nKeySymbol)
				return false;
			m_nAddr = nAddr;
			m_nPrev = pStream->readInt64();
			m_nNext = pStream->readInt64();
			return true;
		}
		bool next(CommonLib::FxMemoryReadStream *pStream)
		{
			return	load(pStream, m_nNext);
		}
		bool prev(CommonLib::FxMemoryReadStream *pStream)
		{
			return	load(pStream, m_nPrev);
		}
		bool getNext(CFilePage *pFilePage, int64& nNext)
		{
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pFilePage->getRowData(), pFilePage->getPageSize());
			int64 nSymbol = stream.readInt64();
			if(nSymbol != m_nKeySymbol)
				return false;
			stream.readInt64();
			nNext = stream.readInt64();
			return true;
		}

	public:
		TStorage *m_pStorage;
		int64 m_nKeySymbol;
		int64 m_nNext;
		int64 m_nPrev;
		int64 m_nAddr;
	};
}
#endif