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
class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess>,
class _TLeafNode =  BPTreeLeafNodeMapv2<_TKey, _TValue,  _TComp, _Transaction, _TLeafCompess>, 
class _TBTreeNode = BPTreeNodeMapv2<_TKey, _TValue,  _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
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

	bool insert(const TKey& key, const TValue& value )
	{
		bool bRet = false;
		TBTreeNode* pNode = findLeafNodeForInsert(key);
		if(pNode)
		{
			bRet = InsertInLeafNode(pNode, key, value);
		}
		ClearChache();
		if(bRet)
			m_BTreeInfo.AddKey(1);
		return bRet;	
	}
	bool InsertInLeafNode(TBTreeNode *pNode, const TKey& key, const TValue& value)
	{
		assert(pNode->isLeaf());
		if(!pNode->insertInLeaf(key, value))
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("BTREE: Error insert"));
			m_pTransaction->error(sMsg);
			return false;
		}
		pNode->setFlags(CHANGE_NODE |BUSY_NODE, true);

		m_ChangeNode.insert(pNode);
		m_nStateTree |= eBPTChangeLeafNode;

		return CheckLeafNode(pNode);
	}
	iterator find(const TKey& key)  
	{
		return TBase::find<iterator>(key);
	}
	iterator begin()
	{
		return TBase::begin<iterator>();
	}

	iterator last()
	{
		return TBase::last<iterator>();
	}
	iterator upper_bound(const TKey& key)
	{
		return TBase::upper_bound<iterator>(key);
	}
	iterator lower_bound(const TKey& key)
	{
		return TBase::lower_bound<iterator>(key);
	}
	bool remove(const TKey& key)
	{
		iterator it = find(key);
		if(it.isNull())
			return false;
		return TBase::remove<iterator>(it);
	}
};

}
#endif