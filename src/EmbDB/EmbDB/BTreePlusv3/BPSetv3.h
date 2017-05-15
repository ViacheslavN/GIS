#pragma once
#include "BaseBPSetv3.h"
#include "BPIteratorSetV3.h"

namespace embDB
{

	template <	class _TKey, class _TComp, class _Transaction,
		class _TInnerCompess = BPInnerNodeSimpleCompressorV3<_TKey>,
		class _TLeafCompess = BPLeafNodeSetSimpleCompressorV3<_TKey>,
		class _TInnerNode = BPTreeInnerNodeSetv3<_TKey, _Transaction, _TInnerCompess>,
		class _TLeafNode = BPTreeLeafNodeSetv3<_TKey, _Transaction, _TLeafCompess>,
		class _TBTreeNode = BPTreeNodeSetv3<_TKey, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
	class TBPSetV3 : public TBPlusTreeSetBaseV3<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{

		typedef TBPlusTreeSetBaseV3<_TKey, _TComp, _Transaction, _TInnerCompess,
			_TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode > TBase;


		typedef typename TBase::TKey      TKey;
		typedef typename TBase::TComp	   TComp;
		typedef typename TBase::TLink     TLink;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompess  TInnerCompess;
		typedef typename TBase::TLeafCompess  TLeafCompess;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode	TLeafNode;
		typedef typename TBase::TBTreeNode TBTreeNode;

		typedef TBPSetIteratorV3<TKey,  TLeafNode, TBTreeNode, TBase> iterator;
	public:
		TBPSetV3(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodePageSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodePageSize, bMulti, bCheckCRC32)
		{

		}

		iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
		}


	};
}