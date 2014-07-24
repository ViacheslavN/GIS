#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SP_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SP_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "RBMap.h"
#include "BPTreeLeafNodeMap.h"
#include "BPLeafNodeMapSimpleCompressor.h"
#include "RectSpatialBPMapTree.h"

namespace embDB
{
	template<typename _TLink,  typename _TCompressor, class _TLeafMemSet>
	class BPTreeSPLeafNodeMap  : public  BPTreeLeafNodeMap<_TLink, _TCompressor, _TLeafMemSet>
	{
	public:

		typedef  BPTreeLeafNodeMap<_TLink, _TCompressor, _TLeafMemSet> TBase;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef _TCompressor TCompressor;
		typedef typename TBase::TKey TKey;
		typedef typename TLeafMemSet::TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TLeafMemSet::TTreeNode TTreeNode;
		typedef typename TKey::TPointType TPointType;
		typedef TRect2D<TPointType>   TRect;

		BPTreeSPLeafNodeMap( CommonLib::alloc_t *pAlloc, bool bMulti) : TBase(pAlloc, bMulti)
		{

		}
		~BPTreeSPLeafNodeMap()
		{

		}

		TRect& getNodeRect()
		{
			return m_nNodeRect;
		}

	
	public:
		TRect m_nNodeRect;
		//TCompressor * m_pCompressor;
		//TLeafMemSet m_leafMemSet;
	};

}
#endif