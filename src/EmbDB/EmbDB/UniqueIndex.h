#ifndef _EMBEDDED_DATABASE_UNIQUE_INDEX_H_
#define _EMBEDDED_DATABASE_UNIQUE_INDEX_H_
#include "BaseBPMapv2.h"
#include "embDBInternal.h"
#include "DBFieldInfo.h"
#include "Index.h"

namespace embDB
{


	template<class _TIndexType, class _TBTree, int FieldDataType>
	class CUniqueIndex : public CIndexBase<_TBTree>, public  IndexFiled
	{
	public:


		typedef CIndexBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef _TIndexType FType;
		typedef typename TBTree::iterator  iterator;
		typedef TIndexIterator<iterator> IndexIterator;


		CUniqueIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize) :
			TBase(pTransactions, pAlloc,  nPageSize)
			{}
		~CUniqueIndex(){}
		virtual indexTypes GetType() const  {return itUnique;}

		virtual bool insert (CommonLib::CVariant* pIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				IndexIterator *pFieldIterator = (IndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}

			FType val;
			pIndexKey->getVal(val);
			bool bRet =  this->m_tree.insert(val, nOID, pFromIterator, pRetIter ? &RetIterator : NULL);


			if(pRetIter)
			{
				if(*pRetIter) 
					((IndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new IndexIterator(RetIterator, this); 
			}
			return bRet;
		}
		virtual bool update (CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant* pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;

		}
		virtual bool remove (CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove (IIndexIterator* pIter )
		{
			return true;
		}
		virtual IIndexIteratorPtr find(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			typename TBTree::iterator it = this->m_tree.find(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this); // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);

			typename TBTree::iterator it = this->m_tree.lower_bound(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this);  // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			typename TBTree::iterator it = this->m_tree.upper_bound(val);
			IndexIterator *pIndexIterator = new IndexIterator(it, this);  // TO DO use
			return IIndexIteratorPtr(pIndexIterator);
		}

		bool remove (CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			return this->m_tree.remove(val);
		}

		virtual bool commit()
		{
			return this->m_tree.commit();
		}
	};

	template<class _FType, int FieldDataType,
	class _TKeyCompressor = TEmptyDiffValueCompress<_FType>,
	class _TComp = embDB::comp<_FType> >
	class UniqueIndexFieldHandler : public CIndexHandlerBase 
	{
	public:

		typedef _FType FType;
		typedef _TComp TComp;
		typedef _TKeyCompressor TKeyCompressor;


		typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TOIDCompressor;
		typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress;
		typedef embDB::TBPBaseInnerNodeDiffCompressor2<_FType, TKeyCompressor, TInnerLinkCompress>  TInnerCompressor;

		typedef  embDB::TBaseLeafNodeDiffComp2<FType, int64, embDB::IDBTransaction, TKeyCompressor, TOIDCompressor> TLeafCompressor;



		typedef embDB::TBPMapV2<FType, int64, TComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		typedef CUniqueIndex<FType, TBTree, FieldDataType> TIndex;

		UniqueIndexFieldHandler(IDBFieldHandler *pField, CommonLib::alloc_t* pAlloc, int64 nIndexPage,const SIndexProp &ip) : CIndexHandlerBase(pField, pAlloc, itUnique, nIndexPage, ip)
		{

		}
		~UniqueIndexFieldHandler()
		{

		}
	
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{
			TLeafCompressorParams leafCompParams;
			TInnerCompressorParams innerCompParams;
			return CIndexHandlerBase::save<TIndex, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &innerCompParams, &leafCompParams);
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

			TIndex * pIndex = new  TIndex(pTransactions, m_pAlloc, m_nNodePageSize);
			pIndex->load(m_nBTreeRootPage, pTransactions->getType());
			return IndexFiledPtr(pIndex);

			return IndexFiledPtr();
		}


		virtual bool release(IndexFiled* pIndex)
		{
			/*TIndex* pDelIndex = (TIndex*)pIndex;

			TIndex::TBTree *pBTree = pDelIndex->getBTree();

			delete pDelIndex;*/
			return true;
		}


	};


	typedef TBaseValueDiffCompress<int16, int16, SignedDiffNumLenCompressor16i> TInt16KeyCompress;
	typedef TBaseValueDiffCompress<uint16, uint16, UnsignedDiffNumLenCompressor16u> TUInt16KeyCompress;

	typedef TBaseValueDiffCompress<int32, int32, SignedDiffNumLenCompressor32i> TInt32KeyCompress;
	typedef TBaseValueDiffCompress<uint32, uint32, UnsignedDiffNumLenCompressor32u> TUInt32KeyCompress;
	
	typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInt64KeyCompress;
	typedef TBaseValueDiffCompress<uint64, uint64, UnsignedDiffNumLenCompressor64u> TUInt64KeyCompress;
 

	typedef UniqueIndexFieldHandler<int64,  dtInteger64, TInt64KeyCompress > TUniqueIndexNT64;
	typedef UniqueIndexFieldHandler<uint64, dtUInteger64, TUInt64KeyCompress > TUniqueIndexUINT64;
	typedef UniqueIndexFieldHandler<int32,  dtInteger32, TInt32KeyCompress> TUniqueIndexINT32;
	typedef UniqueIndexFieldHandler<uint32, dtUInteger32, TUInt32KeyCompress> TUniqueIndexUINT32;
	typedef UniqueIndexFieldHandler<int16,  dtInteger16, TInt16KeyCompress> TUniqueIndexINT16;
	typedef UniqueIndexFieldHandler<uint16, dtUInteger16, TUInt16KeyCompress> TUniqueIndexUINT16;
	typedef UniqueIndexFieldHandler<int32,  dtUInteger8> TUniqueIndexINT8;
	typedef UniqueIndexFieldHandler<uint32, dtInteger8> TUniqueIndexUINT8;
	typedef UniqueIndexFieldHandler<double, dtDouble> TUniqueIndexDouble;
	typedef UniqueIndexFieldHandler<float,  dtFloat> TUniqueIndexFloat;

}


#endif