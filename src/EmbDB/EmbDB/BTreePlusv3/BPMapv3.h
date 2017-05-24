#include "BaseBPSetv3.h"
#include "BPTreeNodeMapV3.h"
#include "BPIteratorMapV3.h"
#include "BPLeafNodeMapSimpleCompressorV3.h"
namespace embDB
{

	template <class _TKey, class _TValue, class _TComp, class _Transaction,
		class _TInnerCompess = BPInnerNodeSimpleCompressorV3<_TKey>,
		class _TLeafCompess = BPLeafNodeMapSimpleCompressorV3<_TKey, _TValue>,
		class _TInnerNode = BPTreeInnerNodeSetv3<_TKey,_Transaction, _TInnerCompess>,
		class _TLeafNode = BPTreeLeafNodeMapv3<_TKey, _TValue, _Transaction, _TLeafCompess>,
		class _TBTreeNode = BPTreeNodeMapv3<_TKey, _TValue, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
		class TBPMapV3 : public TBPlusTreeSetBaseV3<_TKey, _TComp, _Transaction, _TInnerCompess,
		_TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:
		typedef TBPlusTreeSetBaseV3<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode> TBase;

		typedef typename TBase::TKey      TKey;
		typedef typename TBase::TComp	   TComp;
		typedef typename TBase::TLink     TLink;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompess  TInnerCompess;
		typedef typename TBase::TLeafCompess  TLeafCompess;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode	TLeafNode;
		typedef typename TBase::TBTreeNode TBTreeNode;
		typedef typename TBase::TBTreeNodePtr TBTreeNodePtr;
		typedef _TValue  TValue;
		typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;


		typedef TBPMapIteratorV3<TKey, TValue, TLeafNode, TBTreeNode, TBase> iterator;

		TBPMapV3(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodesPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodesPageSize, bMulti, bCheckCRC32)
		{}







		bool insert(const TKey& key, const TValue& value )
		{
			

			bool bRet = false;
			TBTreeNodePtr pNode = this->findLeafNodeForInsert(key);
			if (pNode.get())
			{
				bRet = InsertInLeafNode(pNode.get(), key, value);
			}
 			return bRet;
		}
		bool InsertInLeafNode(TBTreeNode *pNode, const TKey& key, const TValue& value)
		{
			assert(pNode->isLeaf());
			int32 nRetIndex = pNode->insertInLeaf(this->m_comp, key, value);
			if (nRetIndex == -1)
			{
				this->m_pTransaction->error(L"BTREE: Error insert");
				return false;
			}
			pNode->setFlags(CHANGE_NODE, true);
			this->CheckLeafNode(pNode, false);
			return true;
		}

		iterator begin()
		{
			return TBase::template begin<iterator>();
		}

		iterator last()
		{
			return TBase::template last<iterator>();
		}

		iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		template<class TComp>
		iterator find(const TComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template find<iterator, TComp>(comp, key, pFromIterator, bFindNext);
		}
		/*iterator find(iterator& itFrom, const TKey& key, bool bFoundNext = true)
		{
		return TBase::find<iterator, TComp>(itFrom, m_comp, key, bFoundNext);
		}
		template<class TComp>
		iterator find(iterator& itFrom, const TKey& key, bool bFoundNext = true)
		{
		return TBase::find<iterator, TComp>(itFrom, m_comp, key, bFoundNext);
		}*/

		template<class TComp>
		iterator upper_bound(const TComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator>(comp, key, pFromIterator, bFindNext);
		}
		template<class TComp>
		iterator lower_bound(const TComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator>(comp, key, pFromIterator, bFindNext);
		}




		iterator upper_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template upper_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}
		iterator lower_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
		{
			return TBase::template lower_bound<iterator>(this->m_comp, key, pFromIterator, bFindNext);
		}


		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const TValue& value, TKey* pKey = NULL)
		{

			iterator it = last();

			int nIndex = -1;
			if (!it.m_pCurLeafNode)
				return false;

			TKey key;
			if (it.m_nIndex == -1)
				key = keyFunctor.begin();
			else
				key = keyFunctor.inc(it.key());

			if (pKey)
				*pKey = key;
			return InsertInLeafNode(it.m_pCurNode.get(), key, value);
		}

		bool update(const TKey& key, const TValue& value)
		{
			iterator it = find(key);
			if (it.isNull())
				return false;
			it.value() = value; //TO DO set node change....
			return true;
		}
	};

}
