#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "BaseRBTree.h"
#include "BPTreeLeafNode.h"
#include "BPTreeInnerNodeSet.h"
#include "BPTreeLeafNodeMap.h"
#include "BPLeafNodeMapSimpleCompressor.h"
#include "BPInnerNodeSetSimpleCompressor.h"
#include "BPTreeNodeSet.h"
#include "CompressorParams.h"
namespace embDB
{

	template< class _Transaction,	class _TInnerNode,	class _TLeafNode>
	class BPTreeNodeMap : public BPTreeNodeSet<_Transaction, _TInnerNode, _TLeafNode>
	{
	public:

		
		typedef BPTreeNodeSet<Transaction,	_TInnerNode, _TLeafNode> TBase;

		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::Transaction  Transaction;

		typedef typename TBase::TInnerNode  TInnerNode;
		typedef typename TBase::TLeafNode   TLeafNode;

		typedef typename TLeafNode::TValue TValue;

		typedef typename TBase::TInnerMemSet  TInnerMemSet;
		typedef typename TBase::TLeafMemSet   TLeafMemSet;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;

	
		BPTreeNodeMap(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bChecCRC32, ICompressorParams *pInnerCompParams, ICompressorParams *pLeafCompParams) :
			TBase(nParentAddr, pAlloc, nPageAddr, bMulti, bIsLeaf, bChecCRC32, pInnerCompParams, pLeafCompParams)

		{

		}
		~BPTreeNodeMap()
		{

		}

		BPTreeNodeMap(const BPTreeNodeMap& bp)
		{

		}
		bool insertInLeaf( const TKey& key, const TValue& val)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.insert(key, val);
		}
	
	};
}
#endif