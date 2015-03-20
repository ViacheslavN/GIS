#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#include "BaseBPSetv2.h"
#include "MultiIndexBase.h"
#include "BPMultiInnerIndexNodeCompressor.h"
#include "BPMultiIndexLeafNodeCompressor.h"
#include "MultiIndex.h"
namespace embDB
{
	template<class TBTree>
	class MultiIndexIterator : public IIndexIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		MultiIndexIterator(iterator& it) : m_ParentIt(it){}
		MultiIndexIterator() {}
		virtual ~MultiIndexIterator(){}

		virtual bool next()
		{
			return m_ParentIt.next();
		}
		virtual bool isNull()
		{
			return m_ParentIt.isNull();
		}
		virtual bool isValid()
		{
			return !m_ParentIt.isNull();
		}
		virtual bool getKey(IFieldVariant* pVal)
		{
			return pVal->setVal(m_ParentIt.key().m_key);
		}
		virtual uint64 getObjectID()
		{
			return m_ParentIt.key().m_nObjectID;
		}
		virtual int64 addr() const
		{
			return m_ParentIt.addr();
		}
		virtual int32 pos() const
		{
			return m_ParentIt.pos();
		}
		virtual bool copy(IIndexIterator *pIter)
		{
			return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
		}
		void set(iterator it)
		{
			m_ParentIt = it;
		}

	public:
		iterator m_ParentIt;

	};

	template<class _TIndexType, class _TBTree, int FieldDataType>
	class MultiIndex : public CIndexBase<_TBTree>, public  MultiIndexFiled
	{
	public:


		typedef CIndexBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef _TIndexType FType;
		typedef IndexTuple<FType> TIndexTuple;
		typedef typename TBTree::iterator  iterator;
		typedef MultiIndexIterator<TBTree> TMultiIndexIterator;
		typedef MultiIndexKeyOnlyComp<FType> TCompKeyOnly;
		TCompKeyOnly m_CompKeyOnly;


		MultiIndex( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
		TBase(pTransactions, pAlloc)
		{

		}
		~MultiIndex(){}

 
	/*	virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID, IndexIterator* pIter = NULL)
		{
			TIndexTuple index;
			pIndexKey->getVal(index.key);
			index.nObjectID = nOID;

			if(!pIter)
				return m_tree.insert(index);
			else
			{
				iterator it;
				bool bRet = m_tree.insert(index, &it);

			}
		}*/
		virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{

			iterator *pFromIterator = NULL;
			iterator RetIterator;
			if(pFromIterator)
			{
				TMultiIndexIterator *pFieldIterator = (TMultiIndexIterator*)pFromIter;
				assert(pFromIterator);
				pFromIterator = &pFieldIterator->m_ParentIt;
			}

			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, nOID);
			bool bRet =  m_tree.insert(index, pFromIterator, pRetIter ? &RetIterator : NULL);


			if(pRetIter)
			{
				if(*pRetIter) 
					((TMultiIndexIterator*)(*pRetIter))->set(RetIterator);
				else
					*pRetIter = new TMultiIndexIterator(RetIterator); 
			}
			return bRet;
		}
		virtual bool update (IFieldVariant* pOldIndexKey, IFieldVariant* pNewIndexKey, uint64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		{
			//FType val;
			//pIndexKey->getVal(val);
			//return  m_tree.update(nOID, val);
			return true;
			 
		}
		virtual bool remove (IFieldVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		{
			return true;
		}
		virtual bool remove (IIndexIterator* pIter )
		{
			return true;
		}
		virtual IndexIteratorPtr find(IFieldVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.find(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		virtual IndexIteratorPtr lower_bound(IFieldVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.lower_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		virtual IndexIteratorPtr upper_bound(IFieldVariant* pIndexKey)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val, 0);

			TBTree::iterator it = m_tree.upper_bound(m_CompKeyOnly, index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}


		IndexIteratorPtr find(IFieldVariant* pIndexKey, uint64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.find(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		IndexIteratorPtr lower_bound(IFieldVariant* pIndexKey, uint64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.lower_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		IndexIteratorPtr upper_bound(IFieldVariant* pIndexKey, uint64 nOID)
		{
			FType val;
			pIndexKey->getVal(val);
			TIndexTuple index(val,nOID);

			TBTree::iterator it = m_tree.upper_bound(index);
			TMultiIndexIterator *pIndexIterator = new TMultiIndexIterator(it);
			return IndexIteratorPtr(pIndexIterator);
		}
		bool remove (IFieldVariant* pIndexKey, uint64 nOID)
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

	/*
	/*template <class _TKey,  MultiIndexBaseComp<>, class _Transaction,
	class _TInnerCompess = BPMultiIndexInnerNodeCompressor<_TKey>,
	class _TLeafCompess = BPLeafNodeMultiIndexCompressor<_TKey>,  
	class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeSetv2<_TKey,  _TComp, _Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
		>
	class BPMultiIndex  : public TBPSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:
		typedef TBPSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, 
			_TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode > TBase;

		typedef typename TBase::iterator   iterator;
		
		BPMultiIndex(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, false, bCheckCRC32 )
		{

		}


	};*/

	
 


	template<class _FType, int FieldDataType,
	class _TLeafCompressor = embDB::BPLeafNodeMultiIndexCompressor<_FType > 	
	>
	class MultiIndexFieldHandler : public IDBIndexHandler
	{
	public:

		typedef _FType FType;
		typedef embDB::BPMultiIndexInnerNodeCompressor<FType> TInnerCompressor;
		typedef _TLeafCompressor TLeafCompressor;
 		typedef IndexTuple<FType> TIndexTuple;

		typedef embDB::TBPSetV2<TIndexTuple, MultiIndexBaseComp<FType>, 
			embDB::IDBTransactions, TInnerCompressor, TLeafCompressor> TBTree;

		typedef MultiIndex<FType, TBTree, FieldDataType> TMultiIndex;

		MultiIndexFieldHandler(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc)
		{

		}
		~MultiIndexFieldHandler()
		{

		}
		virtual bool lock()
		{
			return true;
		}
		virtual bool unlock()
		{
			return true;
		}

		eDataTypes getType() const
		{
			return (eDataTypes)m_fi.m_nFieldDataType;
		}
		const CommonLib::str_t& getName() const
		{
			return m_fi.m_sFieldName;
		}

		virtual sFieldInfo* getFieldInfoType()
		{
			return &m_fi;
		}
		virtual void setFieldInfoType(sFieldInfo& fi)
		{
			m_fi = fi;
		}
		virtual bool save(int64 nAddr, IDBTransactions *pTran)
		{
			//m_nFieldInfoPage = nAddr;
			FilePagePtr pPage(pTran->getFilePage(nAddr));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, FIELD_PAGE, FIELD_INFO_PAGE);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage());
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			stream.write(m_nBTreeRootPage);
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);

			TMultiIndex field(pTran, m_pAlloc);
			field.init(m_nBTreeRootPage);
			return field.save();
		}
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

		virtual IndexFiled* getIndex(IDBTransactions* pTransactions, IDBStorage *pStorage)
		{

			TMultiIndex * pIndex = new  TMultiIndex(pTransactions, m_pAlloc);
			pIndex->load(m_fi.m_nFieldPage);
			return pIndex;	
		}


		virtual bool release(IndexFiled* pIndex)
		{
			TMultiIndex* pMultiIndex = (TMultiIndex*)pIndex;

			TMultiIndex::TBTree *pBTree = pMultiIndex->getBTree();

			delete pIndex;
			return true;
		}

		bool isCanBeRemoving()
		{
			return true;
		}
	private:
		sFieldInfo m_fi;
		CommonLib::alloc_t* m_pAlloc;

	};



	typedef MultiIndexFieldHandler<int64, ftInteger64> TMultiIndexNT64;
	typedef MultiIndexFieldHandler<uint64, ftUInteger64> TMultiIndexUINT64;
	typedef MultiIndexFieldHandler<int32, ftInteger32> TMultiIndexINT32;
	typedef MultiIndexFieldHandler<uint32, ftUInteger32> TMultiIndexUINT32;
	typedef MultiIndexFieldHandler<int16, ftInteger16> TMultiIndexINT16;
	typedef MultiIndexFieldHandler<uint16, ftUInteger16> TMultiIndexUINT16;
	typedef MultiIndexFieldHandler<int32, ftUInteger8> TMultiIndexINT8;
	typedef MultiIndexFieldHandler<uint32, ftInteger8> TMultiIndexUINT8;
	typedef MultiIndexFieldHandler<double, ftDouble> TMultiIndexDouble;
	typedef MultiIndexFieldHandler<float, ftFloat> TMultiIndexFloat;
}

#endif
