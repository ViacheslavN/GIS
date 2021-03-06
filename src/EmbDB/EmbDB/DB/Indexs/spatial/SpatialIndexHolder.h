#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_Holder_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_Holder_H_
#include "SpatialIndexBase.h"
namespace embDB
{
	template<class _TSpatialIndex>
	class TSpatialIndexHolder : public IDBIndexHolder
	{
	public:

		 
		typedef _TSpatialIndex TSpatialIndex;

		TSpatialIndexHolder(IDBShapeFieldHolder* pField, CommonLib::alloc_t* pAlloc, int64 nPageAddr, const SIndexProp& ip) : m_pAlloc(pAlloc),m_nPageAddr(nPageAddr), m_nBTreeRootPage(-1)
		{

			m_pField = pField;
			 m_nPageNodeSize = ip.m_nNodePageSize;

			 m_nCompressType = ip.m_FieldPropExt.m_CompressType;
			 m_nCalcCompressError = ip.m_FieldPropExt.m_nCompCalcError;
			 m_bOnlineCalcCompSize = ip.m_FieldPropExt.m_bOnlineCalcCompSize;
			 m_nBTreeChacheSize = ip.m_FieldPropExt.m_nBTreeChacheSize;

		}
		~TSpatialIndexHolder()
		{}


		

		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			m_nBTreeRootPage = pStream->readInt64();
			m_nCompressType = (EncoderType)pStream->readintu16();
			m_nCalcCompressError = pStream->readIntu32();
			m_bOnlineCalcCompSize = pStream->readBool();
			m_nBTreeChacheSize =  pStream->readIntu32();
			return true;
		}
		virtual bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran)
		{
			FilePagePtr pRootPage(pTran->getNewPage(MIN_PAGE_SIZE));
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			pStream->write(m_nBTreeRootPage);
			pStream->write(uint16(m_nCompressType));
			pStream->write(m_nCalcCompressError);
			pStream->write(m_bOnlineCalcCompSize);
			pStream->write(m_nBTreeChacheSize);

			TSpatialIndex index(pTran, m_pAlloc, m_nPageNodeSize, m_nBTreeChacheSize);
			index.init(m_nBTreeRootPage, m_nCompressType, m_nCalcCompressError, m_bOnlineCalcCompSize);
			return true;
		}

		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TSpatialIndex * pIndex = new  TSpatialIndex(pTransactions, m_pAlloc, m_nPageNodeSize, m_nBTreeChacheSize);
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
		IDBShapeFieldHolderPtr m_pField;

		EncoderType m_nCompressType;
		uint32 m_nCalcCompressError;
		bool m_bOnlineCalcCompSize;
		uint32 m_nBTreeChacheSize;

	};


	typedef TSpatialIndexHolder<TIndexPoint16> THolderIndexPoint16;
	typedef TSpatialIndexHolder<TIndexPoint32> THolderIndexPoint32;
	typedef TSpatialIndexHolder<TIndexPoint64> THolderIndexPoint64;

	typedef TSpatialIndexHolder<TIndexRect16> THolderIndexRect16;
	typedef TSpatialIndexHolder<TIndexRect32> THolderIndexRect32;
	typedef TSpatialIndexHolder<TIndexRect64> THolderIndexRect64;
}

#endif