#pragma once
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../CompressorParams.h"
#include "BPTreeNodeSet.h"
#include "BPTreeLeafNodeMap.h"

namespace embDB
{



	template<class _TKey, class _TValue, class _Transaction,
		class _TInnerCompressor, class _TLeafCompressor, class _TInnerNode, class _TLeafNode >
		class BPTreeNodeMap : public BPTreeNodeSet<_TKey, _Transaction,
		_TInnerCompressor, _TLeafCompressor, _TInnerNode, _TLeafNode >
	{
	public:

		typedef BPTreeNodeSet<_TKey, _Transaction, _TInnerCompressor,
			_TLeafCompressor, _TInnerNode, _TLeafNode > TBase;

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





		BPTreeNodeMap(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32, uint32 nPageSize,
			TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) :
			TBase(nParentAddr, pAlloc, nPageAddr, bMulti, bIsLeaf, bCheckCRC32, nPageSize, pInnerCompParams, pLeafCompParams)
		{}
		~BPTreeNodeMap()
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
