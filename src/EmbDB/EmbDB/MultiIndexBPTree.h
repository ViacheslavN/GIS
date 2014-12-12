#ifndef _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#define _EMBEDDED_DATABASE_B_MULTI_INDEX_BP_TREE_H_
#include "BaseBPSetv2.h"
#include "MultiIndexBase.h"
#include "BPMultiInnerIndexNodeCompressor.h"
#include "BPMultiIndexLeafNodeCompressor.h"
#include "MultiIndex.h"
namespace embDB
{
	


	template <class _TKey,	class _TComp, class _Transaction,
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


	};



	template<class TBTree>
	class MultiIndexIterator : public IndexIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		virtual bool next()
		{
			return m_ParentIt.next();
		}
		virtual bool isNull()
		{
			return m_ParentIt.isNull();
		}
		virtual bool getVal(IFieldVariant* pIndexKey)
		{
			pIndexKey->setVal(m_ParentIt.getKey().key);
		}
		virtual uint64 getObjectID()
		{
			return m_ParentIt.getKey().nObjectID;
		}

	private:
		iterator m_ParentIt;

	};

	template<class _TIndexType, class _TBTree, int FieldDataType>
	class MultiIndex : public CIndexBase<_TBTree>, public  MultiIndexFiled
	{
	public:


		typedef OIDFieldBase<_TBTree> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef _TIndexType FType;
		typedef IndexTuple<FType> TIndexTuple;

		MultiIndex( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
		TBase(pTransactions, pAlloc)
		{

		}
		~MultiIndex(){}


		virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID)
		{
			TIndexTuple index;
			pIndexKey->getVal(index.key);
			index.nObjectID = nOID;

			return m_tree.insert(index);

		}
		virtual bool commit()
		{
			return m_tree.commit();
		}

	};
}

#endif
