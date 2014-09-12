#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "BPTreeLeafNodeMapv2.h"

namespace embDB
{



	template<class _TKey, class _TValue, class _TComp, class _Transaction, 
	class _TInnerCompressor, class _TLeafCompressor, 	class _TInnerNode,	class _TLeafNode >
	class BPTreeNodeMapv2 : public BPTreeNodeSetv2<_TKey, _TComp, _Transaction, 
		_TInnerCompressor, _TLeafCompressor,  _TInnerNode,	 _TLeafNode >
	{
	public:

		typedef BPTreeNodeSetv2<TKey, _TComp, _Transaction, 	_TInnerCompressor, 
			_TLeafCompressor,  _TInnerNode,	 _TLeafNode > TBase;

		typedef _TValue TValue;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompressor TInnerCompressor;
		typedef typename TBase::TLeafCompressor TLeafCompressor;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode  TLeafNode;



	
		BPTreeNodeMapv2(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,
			ICompressorParams *pInnerCompParams = NULL, ICompressorParams *pLeafCompParams = NULL) : 
			TBase( nParentAddr, pAlloc, nPageAddr, bMulti,  bIsLeaf, bCheckCRC32, pInnerCompParams,pLeafCompParams)
			{}
		~BPTreeNodeMapv2()
		{}

		bool insertInLeaf( const TKey& key, const TValue& val)
		{
			assert(m_bIsLeaf);
			return m_LeafNode.insert(key, val);
		}


		const TValue& value(uint32 nIndex) const
		{
			assert(m_bIsLeaf);
			return m_LeafNode.value(nIndex);
		}
		TValue& value(uint32 nIndex)
		{
				assert(m_bIsLeaf);
			return m_LeafNode.value(nIndex);
		}


	};
}
#endif