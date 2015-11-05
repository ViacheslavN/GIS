#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_V2_H_

#include "BaseBPSetv2.h"
#include "BPTreeLeafNodeMapv2.h"
#include "BPIteratorMapV2.h"
#include "BPLeafNodeMapSimpleCompressorV2.h"
namespace embDB
{

template <class _TKey,	class _TValue, class _TComp, class _Transaction,
class _TInnerCompess = BPInnerNodeSimpleCompressorV2<_TKey>,
class _TLeafCompess = BPLeafNodeMapSimpleCompressorV2<_TKey, _TValue>,  
class _TInnerNode = BPTreeInnerNodeSetv2<_TKey,/* _TComp, */_Transaction, _TInnerCompess>,
class _TLeafNode =  BPTreeLeafNodeMapv2<_TKey, _TValue, /* _TComp, */_Transaction, _TLeafCompess>, 
class _TBTreeNode = BPTreeNodeMapv2<_TKey, _TValue, /* _TComp,*/ _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
>
class TBPMapV2 : public TBPlusTreeSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, 
	_TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
{
public:
	typedef TBPlusTreeSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode> TBase;
	
	typedef typename TBase::TKey      TKey;
	typedef typename TBase::TComp	   TComp;
	typedef typename TBase::TLink     TLink;
	typedef typename TBase::Transaction  Transaction;
	typedef typename TBase::TInnerCompess  TInnerCompess;
	typedef typename TBase::TLeafCompess  TLeafCompess;
	typedef typename TBase::TInnerNode TInnerNode;
	typedef typename TBase::TLeafNode	TLeafNode;
	typedef typename TBase::TBTreeNode TBTreeNode;
	typedef _TValue  TValue;



	typedef TBPMapIteratorV2<TKey, TValue, TComp,Transaction, TInnerCompess, TLeafCompess,
		TInnerNode, TLeafNode, TBTreeNode> iterator;

	TBPMapV2(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32 )
		{}


	bool insert( const TKey& key, const TValue& value, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL )
	{
		bool bRet = false;
		TBTreeNodePtr pNode;
		int nIndex = -1;
		if(pFromIterator)
		{
			pNode = InsertInLeafNode(pFromIterator->m_pCurNode.get(), key, value, nIndex, pFromIterator->m_nIndex + 1);
		}
		else
		{
			pNode = findLeafNodeForInsert(key);
			if(pNode.get())
			{
				bRet = InsertInLeafNode(pNode.get(), key, value, nIndex) != NULL;
			}
		}
		
		
		ClearChache();
		if(bRet)
			m_BTreeInfo.AddKey(1);


		if(pRetItertor)
		{
			*pRetItertor = iterator(this, pNode.get(), nIndex);
		}
		return bRet;	
	}
	TBTreeNodePtr InsertInLeafNode(TBTreeNode *pNode, const TKey& key, const TValue& value, int& nRetIndex, int nToIndex = -1 )
	{
		assert(pNode->isLeaf());
		nRetIndex = pNode->insertInLeaf(m_comp, key, value, nToIndex);
		if(nRetIndex == -1)
		{
			m_pTransaction->error(_T("BTREE: Error insert"));
			return TBTreeNodePtr(NULL);
		}
		pNode->setFlags(CHANGE_NODE |BUSY_NODE, true);

		//m_ChangeNode.insert(TBTreeNodePtr(pNode));
		m_nStateTree |= eBPTChangeLeafNode;
		return CheckLeafNode(pNode, &nRetIndex);
	}
	
	iterator begin()
	{
		return TBase::begin<iterator>();
	}

	iterator last()
	{
		return TBase::last<iterator>();
	}

	iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)  
	{
		return TBase::find<iterator>(m_comp, key, pFromIterator, bFindNext);
	}
	template<class TComp>
	iterator find(const TComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)  
	{
		return TBase::find<iterator, TComp>(comp, key, pFromIterator, bFindNext);
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
		return TBase::upper_bound<iterator>(comp, key, pFromIterator, bFindNext);
	}
	template<class TComp>
	iterator lower_bound(const TComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
	{
		return TBase::lower_bound<iterator>(comp, key, pFromIterator, bFindNext);
	}
	

	

	iterator upper_bound( const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
	{
		return TBase::upper_bound<iterator>(m_comp, key, pFromIterator, bFindNext);
	}
	iterator lower_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
	{
		return TBase::lower_bound<iterator>(m_comp, key, pFromIterator, bFindNext);
	}


	bool remove(const TKey& key)
	{
		iterator it = find(m_comp, key);
		if(it.isNull())
			return false;
		return TBase::remove<iterator>(it);
	}
	template<class TKeyFunctor>
	bool insertLast(TKeyFunctor& keyFunctor, const TValue& value, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
	{

		iterator it;
		if(pFromIterator)
			it = *pFromIterator;
		else
			it = last();

		int nIndex = -1;
		if(!it.m_pCurLeafNode)
			return false;

		TKey key;
		if(it.m_nIndex == -1)
			key = keyFunctor.begin();
		else
			key = keyFunctor.inc(it.key());

		if(pKey)
			*pKey = key;
		TBTreeNodePtr pNode = InsertInLeafNode(it.m_pCurNode.get(), key, value, nIndex, pFromIterator ? pFromIterator->m_nIndex + 1 : -1);
		ClearChache();
		bool bRet = pNode.get() ? true : false;
		if(bRet)
			m_BTreeInfo.AddKey(1);


		if(pRetIterator)
		{
			*pRetIterator = iterator(this, pNode.get(), nIndex);
		}
		return bRet;	
	}

	bool update(const TKey& key, const TValue& value )
	{
		iterator it = find(key);
		if(it.isNull())
			return false;
		it.value() = value; //TO DO set node change....
		return true;
	}
};

}
#endif