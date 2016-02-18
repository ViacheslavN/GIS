#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "BPTreeLeafNodeMapv2.h"

namespace embDB
{



	template<class _TKey, class _TValue,/* class _TComp,*/ class _Transaction, 
	class _TInnerCompressor, class _TLeafCompressor, 	class _TInnerNode,	class _TLeafNode >
	class BPTreeNodeMapv2 : public BPTreeNodeSetv2<_TKey,/* _TComp,*/ _Transaction, 
		_TInnerCompressor, _TLeafCompressor,  _TInnerNode,	 _TLeafNode >
	{
	public:

		typedef BPTreeNodeSetv2<_TKey, _Transaction, 	_TInnerCompressor, 
			_TLeafCompressor,  _TInnerNode,	 _TLeafNode > TBase;

		typedef _TValue TValue;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		//typedef typename TBase::TComp TComp;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompressor TInnerCompressor;
		typedef typename TBase::TLeafCompressor TLeafCompressor;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode  TLeafNode;

		typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;


	

	
		BPTreeNodeMapv2(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32, uint32 nPageSize,
			TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) : 
			TBase( nParentAddr, pAlloc, nPageAddr, bMulti,  bIsLeaf, bCheckCRC32, nPageSize, pInnerCompParams,pLeafCompParams)
			{}
		~BPTreeNodeMapv2()
		{}
		template<class TComp>
		int insertInLeaf(TComp& comp, const TKey& key, const TValue& val, int nInsertInIndex = -1)
		{
			assert(this->m_bIsLeaf);
			return this->m_LeafNode.insert(comp, key, val, nInsertInIndex);
		}


		const TValue& value(uint32 nIndex) const
		{
			assert(this->m_bIsLeaf);
			return  this->m_LeafNode.value(nIndex);
		}
		TValue& value(uint32 nIndex)
		{
			assert(this->m_bIsLeaf);
			return this->m_LeafNode.value(nIndex);
		}


		 


		 
	};
}
#endif