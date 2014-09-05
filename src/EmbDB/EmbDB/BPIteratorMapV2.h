#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_ITERATOR_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_ITERATOR_V2_H_
#include "BPTreeNodeMapV2.h"

namespace embDB
{



	template <class _TKey,  class _TValue, class _TComp, class _Transaction, class _TInnerCompess, class _TLeafCompess, 
	class _TInnerNode, 	class _TLeafNode, class _TBTreeNode>
	class TBPMapIteratorV2 : public TBPSetIteratorV2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess,
	_TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:
		typedef TBPSetIteratorV2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess,
		_TInnerNode, _TLeafNode, _TBTreeNode> TBase;
		typedef typename TBase::TKey      TKey;
		typedef typename TBase::TComp	   TComp;
		typedef int64     TLink;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompess TInnerCompess;  
		typedef typename TBase::TLeafCompess TLeafCompess; 
		typedef typename TBase::TInnerNode	TInnerNode;
		typedef typename TBase::TBTreeLeafNode TBTreeLeafNode;
		typedef typename TBase::TBTreeNode TBTreeNode;
		typedef _TValue TValue;


		typedef TBPlusTreeSetV2<TKey,  TComp, Transaction, TInnerCompess, TLeafCompess
		, TInnerNode, TBTreeLeafNode, TBTreeNode> TBTree;

		TBPMapIteratorV2(TBTree *pTree, TBTreeNode *pCurNode, int32 nIndex) :
			TBase(pTree, pCurNode, nIndex)
		{		
		}
		const TValue& value() const
		{
			return m_pCurLeafNode->value((uint32)m_nIndex);
		}
		TValue& value()
		{
			return m_pCurLeafNode->value((uint32)m_nIndex);
		}

	};
}

#endif