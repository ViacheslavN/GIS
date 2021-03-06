#pragma once
#include "BaseBPSet.h"
#include "BPIteratorSet.h"

namespace embDB
{

	template <	class _TKey, class _TComp, class _Transaction,
		class _TInnerCompess = BPInnerNodeSimpleCompressor<_TKey>,
		class _TLeafCompess = BPLeafNodeSetSimpleCompressor<_TKey>,
		class _TInnerNode = BPTreeInnerNodeSet<_TKey, _Transaction, _TInnerCompess>,
		class _TLeafNode = BPTreeLeafNodeSet<_TKey, _Transaction, _TLeafCompess>,
		class _TBTreeNode = BPTreeNodeSet<_TKey, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
	class TBPSet : public TBPlusTreeSetBase<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{

		typedef TBPlusTreeSetBase<_TKey, _TComp, _Transaction, _TInnerCompess,
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
	public:
		typedef TBPSetIterator<TKey,  TLeafNode, TBTreeNode, TBase> iterator;

		TBPSet(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodePageSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodePageSize, bMulti, bCheckCRC32)
		{

		}

		iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
		}
		template<class _TCustComp>
		iterator find(_TCustComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, _TCustComp>(comp, key, pFromIterator, bFindNext);
		}

		iterator upper_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		iterator lower_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		template<class _Comp>
		iterator upper_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}
		template<class _Comp>
		iterator lower_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
		}
	};
}