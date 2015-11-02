#ifndef _EMBEDDED_DATABASE_COMPOSITE_UNIQUE_INDEX_H_
#define _EMBEDDED_DATABASE_COMPOSITE_UNIQUE_INDEX_H_
#include "BaseBPMapv2.h"
#include "IDBField.h"
#include "DBFieldInfo.h"
#include "CompositeIndexKey.h"
#include "CompIndexInnerCompressor.h"
#include "CompIndexLeafCompressor.h"
namespace embDB
{

	typedef TIndexFiled<CompositeIndexKey, IIndexIterator, IndexIteratorPtr> CompositeIndexFiled;

	template<class _TBTree>
	class CCompUniqueIndex : public CIndexBase<_TBTree>, public  CompositeIndexFiled
	{
	public:


		typedef CIndexBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;
		typedef TIndexIterator<TBTree> TIndexIterator;


		CCompUniqueIndex( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) :
			TBase(pTransactions, pAlloc)
			{}
		~CCompUniqueIndex(){}

		virtual bool insert (CompositeIndexKey* pIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				TIndexIterator *pFieldIterator = (TIndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}
			bool bRet =  m_tree.insert(*pIndexKey, nOID, pFromIterator, pRetIter ? &RetIterator : NULL);


			if(pRetIter)
			{
				if(*pRetIter) 
					((TIndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new TIndexIterator(RetIterator); 
			}
			return bRet;
		}
		virtual bool update (CompositeIndexKey* pOldIndexKey, CompositeIndexKey* pNewIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;

		}
		virtual bool remove (CompositeIndexKey* pIndexKey, CompositeIndexKey** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove (CompositeIndexKey* pIter )
		{
			return true;
		}
		virtual IndexIteratorPtr find(CompositeIndexKey* pIndexKey)
		{
 
			TBTree::iterator it = m_tree.find(*pIndexKey);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		virtual IndexIteratorPtr lower_bound(CompositeIndexKey* pIndexKey)
		{
	 

			TBTree::iterator it = m_tree.lower_bound(*pIndexKey);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		virtual IndexIteratorPtr upper_bound(CompositeIndexKey* pIndexKey)
		{
			TBTree::iterator it = m_tree.upper_bound(*pIndexKey);
			TIndexIterator *pIndexIterator = new TIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}

		bool remove (CompositeIndexKey* pIndexKey)
		{
				pIndexKey->getVal(*pIndexKey);
			return m_tree.remove(val);
		}

		virtual bool commit()
		{
			return m_tree.commit();
		}
	};

	template<class _TInnerCompressor = BPInnerCompIndexCompressor,
	class _TLeafCompressor = embDB::BPLeafCompIndexCompressor<uint64 >,
	class _TComp = embDB::comp<CompositeIndexKey> >
	class UniqueCompIndexFieldHandler : public CIndexHandlerBase 
	{
	public:

		typedef _TComp TComp;
		typedef _TInnerCompressor TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;


		typedef embDB::TBPMapV2<CompositeIndexKey, uint64, TComp, 
			embDB::IDBTransaction, TInnerCompressor, TLeafCompressor> TBTree;

		typedef CCompUniqueIndex<TBTree> TIndex;

		UniqueCompIndexFieldHandler(CommonLib::alloc_t* pAlloc) : CIndexHandlerBase(pAlloc)
		{

		}
		~UniqueCompIndexFieldHandler()
		{

		}
	
		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			return CIndexHandlerBase::save<TIndex>(nAddr, pTran, m_pAlloc, INDEX_PAGE, INDEX_INFO_PAGE);
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

		virtual IndexFiled* getIndex(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{

			TIndex * pIndex = new  TIndex(pTransactions, m_pAlloc);
			pIndex->load(m_fi.m_nFieldPage, pTransactions->getType());
			return pIndex;	
		}


		virtual bool release(IndexFiled* pIndex)
		{
			TIndex* pDelIndex = (TIndex*)pIndex;

			TIndex::TBTree *pBTree = pDelIndex->getBTree();

			delete pDelIndex;
			return true;
		}


	};
 
	typedef UniqueCompIndexFieldHandler<> TUniqueCompIndexFieldHandler;
}


#endif