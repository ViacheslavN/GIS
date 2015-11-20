#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#include "BaseBPSetv2.h"
#include "MultiIndexBase.h"
#include "BPMultiInnerIndexNodeCompressor.h"
#include "BPMultiIndexLeafNodeCompressor.h"
#include "MultiIndex.h"
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
			pVal->setVal(m_ParentIt.key().m_key);
			return true;
		}
		virtual int64 getRowID()
		{
			return m_ParentIt.key().m_nObjectID;
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


		MultiIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) :
		TBase(pTransactions, pAlloc)
		{

		}
		~MultiIndex(){}

		virtual indexTypes GetType() const  {return itMultiRegular;}

		virtual bool insert (/*IVariant* pIndexKey*/CommonLib::CVariant *pValue, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
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
			bool bRet =  m_tree.insert(index, pFromIterator, pRetIter ? &RetIterator : NULL);


			if(pRetIter)
			{
				if(*pRetIter) 
					((TMultiIndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new TMultiIndexIterator(RetIterator, this); 
			}
			return bRet;
		}
		virtual bool update (CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant *pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
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
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.find(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.lower_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		virtual IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.upper_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}


		IIndexIteratorPtr find(CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.find(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.lower_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.upper_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it, this);
			return IIndexIteratorPtr(pIndexIterator);
		}
		bool remove (CommonLib::CVariant* pIndexKey, int64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			return m_tree.remove(index);
		}

		virtual bool commit()
		{
			return m_tree.commit();
		}
	};

	template<class _FType, int FieldDataType,
	class _TLeafCompressor = embDB::BPLeafNodeMultiIndexCompressor<_FType >,
	class _TKeyComp = embDB::MultiIndexKeyOnlyComp<_FType>,
	class _TBaseComp = MultiIndexBaseComp<_FType>
	>
	class MultiIndexFieldHandler  : public CIndexHandlerBase 
	{
	public:

		typedef _FType FType;
		typedef embDB::BPMultiIndexInnerNodeCompressor<FType> TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;
 		typedef IndexTuple<FType> TIndexTuple;

		typedef _TBaseComp  TBaseComp;
		typedef _TKeyComp  TKeyComp;

		typedef embDB::TBPSetV2<TIndexTuple, TBaseComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;

		typedef MultiIndex<FType, TBTree, FieldDataType, TBaseComp, TKeyComp> TMultiIndex;

		MultiIndexFieldHandler(CommonLib::alloc_t* pAlloc) : CIndexHandlerBase(pAlloc, itMultiRegular)
		{

		}
		~MultiIndexFieldHandler()
		{

		}
		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			return true;//CIndexHandlerBase::save<TMultiIndex>(nAddr, pTran, m_pAlloc, INDEX_PAGE, MULTI_INDEX_INFO_PAGE);
		}
		
		virtual IndexFiledPtr getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

		/*	TMultiIndex * pIndex = new  TMultiIndex(pTransactions, m_pAlloc);
			pIndex->load(m_fi.m_nFieldPage, pTransactions->getType());
			return IndexFiledPtr(pIndex);	*/

			return IndexFiledPtr();
		}


		virtual bool release(IndexFiled* pIndex)
		{
			/*TMultiIndex* pMultiIndex = (TMultiIndex*)pIndex;

			TMultiIndex::TBTree *pBTree = pMultiIndex->getBTree();

			delete pIndex;*/
			return true;
		}

	};



	typedef MultiIndexFieldHandler<int64,  dtInteger64> TMultiIndexNT64;
	typedef MultiIndexFieldHandler<uint64, dtUInteger64> TMultiIndexUINT64;
	typedef MultiIndexFieldHandler<int32,  dtInteger32> TMultiIndexINT32;
	typedef MultiIndexFieldHandler<uint32, dtUInteger32> TMultiIndexUINT32;
	typedef MultiIndexFieldHandler<int16,  dtInteger16> TMultiIndexINT16;
	typedef MultiIndexFieldHandler<uint16, dtUInteger16> TMultiIndexUINT16;
	typedef MultiIndexFieldHandler<int32,  dtUInteger8> TMultiIndexINT8;
	typedef MultiIndexFieldHandler<uint32, dtInteger8> TMultiIndexUINT8;
	typedef MultiIndexFieldHandler<double, dtDouble> TMultiIndexDouble;
	typedef MultiIndexFieldHandler<float,  dtFloat> TMultiIndexFloat;
}

#endif
