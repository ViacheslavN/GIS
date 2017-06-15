#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#include "../../../BTreePlus/BPSetv3.h"
#include "MultiIndexBase.h"
//#include "BPMultiInnerIndexNodeCompressor.h"
//#include "BPMultiIndexLeafNodeCompressor.h"
#include "../MultiIndex.h"
 
#include "MultiKeyCompressor.h"
#include "../../../Fields/BaseFieldEncoders.h"
#include "../../../BTreePlus/BaseDIffEncoder.h"
#include "../../../BTreePlus/BaseLeafNodeSetCompressor.h"
#include "EmptyMultiKeyCompress.h"
namespace embDB
{
	template<class TIterator>
	class MultiIndexIterator : public TIndexIteratorBase<TIterator, IIndexIterator>
	{
	public:
		typedef TIndexIteratorBase<TIterator, IIndexIterator> TBase;
		typedef typename TBase::iterator  iterator;

		MultiIndexIterator(iterator& it, IndexFiled *pIndex) : TBase(it, pIndex)
		{
		}
		MultiIndexIterator() {}
	
		virtual bool getKey(CommonLib::CVariant* pVal)
		{
			pVal->setVal(this->m_ParentIt.key().m_key);
			return true;
		}
		virtual int64 getRowID()
		{
			return this->m_ParentIt.key().m_nRowID;
		}
	};

	template<class _TIndexType, class _TBTree, int FieldDataType,
	class _TBaseComp ,	class _TKeyComp>
	class MultiIndex : public CIndexBase<_TBTree>, public  MultiIndexFiled
	{
	public:


		typedef CIndexBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef _TIndexType FType;
		typedef IndexTuple<FType> TIndexTuple;
		typedef typename TBTree::iterator  iterator;
		typedef MultiIndexIterator<iterator> TMultiIndexIterator;
		//typedef TIndexIteratorBase<TBTree, IIndexIterator> TMultiIndexIterator;
		typedef _TKeyComp TKeyComp;
		TKeyComp m_CompKeyOnly;


		MultiIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nNodePageSize) :
		TBase(pTransactions, pAlloc, nNodePageSize)
		{

		}
		~MultiIndex(){}

		virtual indexTypes GetType() const  {return itMultiRegular;}

		virtual bool insert (/*IVariant* pIndexKey*/const CommonLib::CVariant *pValue, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				TMultiIndexIterator *pFieldIterator = (TMultiIndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}

			/*FType val;
			pIndexKey->getVal(val);*/
			FType val = pValue->Get<FType>();
			TIndexTuple index(val, nOID);
			bool bRet =  this->m_tree.insert(index/*, pFromIterator, pRetIter ? &RetIterator : NULL*/);


			if(pRetIter)
			{
				if(*pRetIter) 
					((TMultiIndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new TMultiIndexIterator(RetIterator, this); 
			}
			return bRet;
		}
		virtual bool update (const CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant *pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;
			 
		}
		virtual bool remove (const CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove (IIndexIterator* pIter )
		{
			return true;
		}
		virtual IIndexIteratorPtr find(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			typename TBTree::iterator it = this->m_tree.find(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}

		virtual bool IsExsist(const CommonLib::CVariant* pIndexKey)
		{

			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			typename TBTree::iterator it = this->m_tree.find(m_CompKeyOnly, index);
			return !it.isNull();
		}

		virtual IIndexIteratorPtr lower_bound(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			typename TBTree::iterator it = this->m_tree.lower_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr upper_bound(const CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			typename TBTree::iterator it = this->m_tree.upper_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}


		IIndexIteratorPtr find(const CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			typename TBTree::iterator it = this->m_tree.find(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		IIndexIteratorPtr lower_bound(const CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			typename TBTree::iterator it = this->m_tree.lower_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		IIndexIteratorPtr upper_bound(const CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			typename TBTree::iterator it = this->m_tree.upper_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		bool remove (const CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			return this->m_tree.remove(index);
		}

		virtual bool commit()
		{
			return this->m_tree.commit();
		}
	};

	template<class _FType, int FieldDataType,
	class _TMultiKeyCompressor = TEmptyMultiKeyCompress<_FType >,
	class _TKeyComp = embDB::MultiIndexKeyOnlyComp<_FType>,
	class _TBaseComp = MultiIndexBaseComp<_FType>
	>
	class MultiIndexFieldHolder  : public CIndexHolderBase 
	{
	public:

		typedef _FType FType;
		typedef IndexTuple<FType> TIndexTuple;
		typedef _TMultiKeyCompressor TMultiKeyCompressor;
		typedef TBaseNodeCompressor<TIndexTuple, int64, embDB::IDBTransaction, TMultiKeyCompressor, TSignedDiffEncoder64> TInnerCompressor;
		typedef TBaseLeafNodeSetCompressor<TIndexTuple, embDB::IDBTransaction, TMultiKeyCompressor>    TLeafCompressor;
 	

		typedef _TBaseComp  TBaseComp;
		typedef _TKeyComp  TKeyComp;

		typedef embDB::TBPSetV3<TIndexTuple, TBaseComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;
		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		typedef MultiIndex<FType, TBTree, FieldDataType, TBaseComp, TKeyComp> TMultiIndex;

		MultiIndexFieldHolder(IDBFieldHolder *pField, CommonLib::alloc_t* pAlloc, int64 nPageAddr, const SIndexProp &ip) : CIndexHolderBase(pField, pAlloc, itMultiRegular, nPageAddr, ip)
		{

		}
		~MultiIndexFieldHolder()
		{

		}
		virtual bool save(CommonLib::IWriteStream * pStream, IDBTransaction *pTran)
		{
			return CIndexHolderBase::save<TMultiIndex, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc);
		}
		
		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

			TMultiIndex * pIndex = new  TMultiIndex(pTransactions, this->m_pAlloc, m_nNodePageSize);
			pIndex->load(this->m_nBTreeRootPage, pTransactions->getType());
			return IndexFiledPtr(pIndex);	
		}


		virtual bool release(IndexFiled* pIndex)
		{
			/*TMultiIndex* pMultiIndex = (TMultiIndex*)pIndex;

			TMultiIndex::TBTree *pBTree = pMultiIndex->getBTree();

			delete pIndex;*/
			return true;
		}

	};

	typedef TMultiKeyCompressor<int64,  TUnsignedNumLenEncoder64> TMultiKeyCompressor64;
	typedef TMultiKeyCompressor<uint64,  TUnsignedNumLenEncoderU64> TMultiKeyCompressor64u;

	typedef TMultiKeyCompressor<int32, TUnsignedNumLenEncoder32> TMultiKeyCompressor32;
	typedef TMultiKeyCompressor<uint32, TUnsignedNumLenEncoderU32> TMultiKeyCompressor32u;

	typedef TMultiKeyCompressor<int16, TUnsignedNumLenEncoder16> TMultiKeyCompressor16;
	typedef TMultiKeyCompressor<uint16, TUnsignedNumLenEncoderU16> TMultiKeyCompressor16u;

	typedef TMultiKeyCompressor<int8, TUnsignedNumLenEncoder8> TMultiKeyCompressor8;
	typedef TMultiKeyCompressor<byte, TUnsignedNumLenEncoderU8> TMultiKeyCompressor8u;

	typedef MultiIndexFieldHolder<int64,  dtInteger64, TMultiKeyCompressor64> TMultiIndexNT64;
	typedef MultiIndexFieldHolder<uint64, dtUInteger64, TMultiKeyCompressor64u> TMultiIndexUINT64;

	typedef MultiIndexFieldHolder<int32,  dtInteger32, TMultiKeyCompressor32> TMultiIndexINT32;
	typedef MultiIndexFieldHolder<uint32, dtUInteger32, TMultiKeyCompressor32u> TMultiIndexUINT32;

	typedef MultiIndexFieldHolder<int16,  dtInteger16, TMultiKeyCompressor16> TMultiIndexINT16;
	typedef MultiIndexFieldHolder<uint16, dtUInteger16, TMultiKeyCompressor16u> TMultiIndexUINT16;

	typedef MultiIndexFieldHolder<byte,  dtUInteger8, TMultiKeyCompressor8u> TMultiIndexINT8;
	typedef MultiIndexFieldHolder<int8, dtInteger8, TMultiKeyCompressor8> TMultiIndexUINT8;

	typedef MultiIndexFieldHolder<double, dtDouble> TMultiIndexDouble;
	typedef MultiIndexFieldHolder<float,  dtFloat> TMultiIndexFloat;
}

#endif
