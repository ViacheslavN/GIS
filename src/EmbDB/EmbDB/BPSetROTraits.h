#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_TRAITS_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_TRAITS_H_
#include "simple_vector.h"

#include "BPLeafNodeSetSimpleCompressor.h"
#include "BPInnerNodeSetSimpleCompressor.h"
#include "BPIteratorSet.h"
#include "BPTreeInnerNodeSet.h"
#include "BPTreeNodeSet.h"
#include "BPTreeLeafNodeSet.h"


namespace embDB
{
	template <class _TKey, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>,	
	class _TLeafMemSet = RBSet<_TKey, _TComp>,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<_TKey, _TLink, _TInnerMemSet> ,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressor<_TKey, _TLeafMemSet>,
	class _TInnerNode = BPTreeInnerNodeSet<_TKey, _TLink, _TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeSet<_TKey, _TLink, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeSet<_TKey, _TLink, _TComp, _Transaction,_TInnerMemSet, _TLeafMemSet, _TInnerNode, _TLeafNode>
	>
	class TBPSetTraitsRBTreeBase
	{
	public:
		typedef _TKey TKey;
		typedef  _TComp TComp;
		typedef _TLink TLink;
		typedef	_Transaction  Transaction;
		typedef _TInnerMemSet TInnerMemSet;	
		typedef _TLeafMemSet TLeafMemSet;
		typedef _TInnerCompess TInnerCompressor;
		typedef _TLeafCompess TLeafCompressor;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode TLeafNode;
		typedef _TBTreeNode  TBTreeNode;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;




	};
}
#endif