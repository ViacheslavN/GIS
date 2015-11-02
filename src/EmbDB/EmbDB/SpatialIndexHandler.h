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

		TSpatialIndexHandler(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc)
		{}
		~TSpatialIndexHandler()
		{}


		eDataTypes getType() const
		{
			return (eDataTypes)m_fi.m_nFieldDataType;
		}
		const CommonLib::CString& getName() const
		{
			return m_fi.m_sFieldName;
		}

		virtual sFieldInfo* getFieldInfoType()
		{
			return (sFieldInfo*)&m_fi;
		}
		virtual void setFieldInfoType(sFieldInfo* fi)
		{
			 sSpatialFieldInfo *pSpFi = dynamic_cast<sSpatialFieldInfo *>(fi);
			 assert(pSpFi);
			 if(pSpFi)
				 m_fi = *pSpFi;
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}
		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(nAddr);
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, FIELD_PAGE, TABLE_INDEX_PAGE);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage());
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();

			stream.write(m_nBTreeRootPage);
			stream.write(m_fi.m_dOffsetX);
			stream.write(m_fi.m_dOffsetY);
			stream.write(m_fi.m_dScaleX);
			stream.write(m_fi.m_dScaleY);
			stream.write(m_fi.m_extent.xMin);
			stream.write(m_fi.m_extent.yMin);
			stream.write(m_fi.m_extent.xMax);
			stream.write(m_fi.m_extent.yMax);

			stream.write((uint32)m_fi.m_nFieldType);
			stream.write((uint32)m_fi.m_ShapeType);

			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);

			TSpatialIndex index(pTran, m_pAlloc);
			index.init(m_nBTreeRootPage, &m_fi);
			return index.save();
		}

		virtual IndexFiled* getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TSpatialIndex * pIndex = new  TSpatialIndex(pTransactions, m_pAlloc);
			pIndex->load(m_fi.m_nFieldPage, pTransactions->getType());
			return pIndex;
		}
		virtual bool release(IndexFiled* pIndex)
		{
			delete pIndex;
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

	 
	
	private:
		sSpatialFieldInfo m_fi;
		CommonLib::alloc_t* m_pAlloc;
		IDBIndexHandler* m_pIndexHandler;
	};


	typedef TSpatialIndexHandler<TIndexPoint16> THandlerIndexPoint16;
	typedef TSpatialIndexHandler<TIndexPoint32> THandlerIndexPoint32;
	typedef TSpatialIndexHandler<TIndexPoint64> THandlerIndexPoint64;

	typedef TSpatialIndexHandler<TIndexRect16> THandlerIndexRect16;
	typedef TSpatialIndexHandler<TIndexRect32> THandlerIndexRect32;
	typedef TSpatialIndexHandler<TIndexRect64> THandlerIndexRect64;
}

#endif