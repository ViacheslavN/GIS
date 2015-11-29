#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_HANDLER_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_HANDLER_H_
#include "SpatialIndexBase.h"
namespace embDB
{
	template<class _TSpatialIndex>
	class TSpatialIndexHandler : public IDBIndexHandler
	{
	public:

		 
		typedef _TSpatialIndex TSpatialIndex;

		TSpatialIndexHandler(IDBShapeFieldHandler* pField, CommonLib::alloc_t* pAlloc, int64 nPageAddr, uint32 nPageNodeSize) : m_pAlloc(pAlloc),m_nPageAddr(nPageAddr), m_nBTreeRootPage(-1), m_nPageNodeSize(nPageNodeSize)
		{

			m_pField = pField;
		}
		~TSpatialIndexHandler()
		{}


		

		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			m_nBTreeRootPage = pStream->readInt64();
			return true;
		}
		virtual bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran)
		{
			FilePagePtr pRootPage(pTran->getNewPage(MIN_PAGE_SIZE));
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			pStream->write(m_nBTreeRootPage);

			TSpatialIndex index(pTran, m_pAlloc, m_nPageNodeSize);
			index.init(m_nBTreeRootPage);
			return true;
		}

		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TSpatialIndex * pIndex = new  TSpatialIndex(pTransactions, m_pAlloc, m_nPageNodeSize);
			pIndex->load(m_nBTreeRootPage, m_pField.get());
			return IndexFiledPtr(pIndex);
		}
		virtual bool release(IndexFiled* pIndex)
		{
			//delete pIndex;
			return true;
		}

		virtual bool lock()
		{
			return true;
		}
		virtual bool unlock()
		{
			return true;
		}
		virtual bool isCanBeRemoving()
		{
			return true;
		}
		virtual indexTypes GetType() const
		{
			return itSpatial;
		}
		virtual  IFieldPtr GetField() const 
		{
			return IFieldPtr(m_pField.get());
		}
	 
	
	private:
		int64 m_nBTreeRootPage;
		int64 m_nPageAddr;
		uint32 m_nPageNodeSize;
		CommonLib::alloc_t* m_pAlloc;
		IDBShapeFieldHandlerPtr m_pField;

	};


	typedef TSpatialIndexHandler<TIndexPoint16> THandlerIndexPoint16;
	typedef TSpatialIndexHandler<TIndexPoint32> THandlerIndexPoint32;
	typedef TSpatialIndexHandler<TIndexPoint64> THandlerIndexPoint64;

	typedef TSpatialIndexHandler<TIndexRect16> THandlerIndexRect16;
	typedef TSpatialIndexHandler<TIndexRect32> THandlerIndexRect32;
	typedef TSpatialIndexHandler<TIndexRect64> THandlerIndexRect64;
}

#endif