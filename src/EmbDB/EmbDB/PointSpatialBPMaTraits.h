#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_SPATIAL_POINT_TRAITS_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_SPATIAL_POINT_TRAITS_H_
#include "RBMap.h"
#include "RBSet.h"
#include "PoinMapLeafCompressor.h"
#include "PoinMapInnerCompressor.h"
#include "BPTreeInnerNodeSet.h"
#include "BPTreeNodeMap.h"
#include "BPTreeLeafNodeMap.h"
#include "PoinMapLeafCompressor64.h"
#include "PoinMapInnerCompressor64.h"
#include "SpatialPointQuery.h"
namespace embDB
{
	template <class _TCoord, class _TValue, class _TLink, class _TComp, class _Transaction>
	class TBPMapSpatialPointsTraits
	{
	public:
		typedef _TCoord TKey;
		typedef typename _TCoord::TPointType TPointType;
		typedef _TValue TValue;
		typedef  _TComp TComp;
		typedef _TLink TLink;
		typedef	_Transaction  Transaction;

		typedef RBMap<TKey, TLink, TComp>   TInnerMemSet;
		typedef RBMap<TKey, TValue, TComp>  TLeafMemSet;
		typedef BPSpatialPointInnerNodeSimpleCompressor<TInnerMemSet> TInnerCompressor;
		typedef BPSpatialPointLeafNodeMapSimpleCompressor<TLeafMemSet> TLeafCompressor;
		typedef BPTreeInnerNodeSet<TInnerCompressor, TInnerMemSet> TInnerNode;
		typedef BPTreeLeafNodeMap<TLink, TLeafCompressor, TLeafMemSet> TLeafNode;
		typedef BPTreeNodeMap<Transaction, TInnerNode, TLeafNode> TBTreeNode;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;
	};


	template <class _TValue, class _TLink, class _Transaction>
	class TBPMapSpatialPoints64Traits
	{
	public:
		typedef ZOrderPoint2DU64 TKey;
		typedef uint64 TPointType;
		typedef _TValue TValue;
		typedef  ZPointComp64 TComp;
		typedef _TLink TLink;
		typedef	_Transaction  Transaction;

		typedef RBMap<TKey, TLink, TComp>   TInnerMemSet;
		typedef RBMap<TKey, TValue, TComp>  TLeafMemSet;
		typedef BPSpatialPointInnerNodeSimpleCompressor64<TLink> TInnerCompressor;
		typedef BPSpatialPointLeafNodeMapSimpleCompressor64<TValue> TLeafCompressor;
		typedef BPTreeInnerNodeSet<TInnerCompressor, TInnerMemSet> TInnerNode;
		typedef BPTreeLeafNodeMap<TLink, TLeafCompressor, TLeafMemSet> TLeafNode;
		typedef BPTreeNodeMap<Transaction, TInnerNode, TLeafNode> TBTreeNode;

		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeafMemSetNode;
	};

}
#endif