#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_TRAITS_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_TRAITS_H_
#include "RBMap.h"
#include "RBSet.h"
#include "BPLeafNodeMapSimpleCompressor.h"
#include "BPInnerNodeSetSimpleCompressor.h"
#include "BPTreeInnerNodeSet.h"
#include "BPTreeNodeMap.h"
#include "BPTreeLeafNodeMap.h"

namespace embDB
{
	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>,	
	class _TLeafMemSet = RBMap<_TKey, _TValue, _TComp>,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<_TInnerMemSet> ,
	class _TLeafCompess = BPLeafNodeMapSimpleCompressor<_TLeafMemSet>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeMap< _TLink, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeMap<_Transaction, _TInnerNode, _TLeafNode>

	>
	class TBPMapTraitsRBTreeBase 
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  _TComp TComp;
		typedef _TLink TLink;
		typedef	_Transaction  Transaction;
		typedef _TInnerMemSet TInnerMemSet;	
		typedef _TLeafMemSet TLeafMemSet;
		typedef _TInnerCompess TInnerCompressor;
		typedef _TLeafCompess TLeafCompressor;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode TLeafNode;
		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;
		typedef _TBTreeNode TBTreeNode;
	};





	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerCompess ,	class _TLeafCompess >
	class TBPMapTraitsRBTreeBaseCustomCompress
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  _TComp TComp;
		typedef _TLink TLink;
		typedef	_Transaction  Transaction;
		typedef _TInnerCompess TInnerCompressor;
		typedef _TLeafCompess TLeafCompressor;
		typedef RBMap<TKey, TLink, TComp>   TInnerMemSet;
		typedef RBMap<TKey, TValue, TComp>  TLeafMemSet;
		typedef BPTreeInnerNodeSet<TInnerCompressor, TInnerMemSet> TInnerNode;
		typedef BPTreeLeafNodeMap<TLink, _TLeafCompess, TLeafMemSet> TLeafNode;
		typedef BPTreeNodeMap<Transaction, TInnerNode, TLeafNode> TBTreeNode;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;

	};
}
#endif