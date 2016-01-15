#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "FixedStringBPLeafNode.h"
#include "FixedStringLeafCompressor.h"
namespace embDB
{
 

	template<class _TKey, class _Transaction>
	class BPFixedStringTreeNodeMapv2 : public BPTreeNodeMapv2<_TKey, sFixedStringVal, _Transaction, 	BPInnerNodeSimpleCompressorV2<_TKey>,
		BPFixedStringLeafNodeCompressor<_TKey,  _Transaction>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >,
		TFixedStringLeafNode<_TKey, _Transaction> >
	{
	public:
		typedef BPInnerNodeSimpleCompressorV2<_TKey> TInnerCompressor;
		typedef BPFixedStringLeafNodeCompressor<_TKey,  _Transaction> TLeafCompressor;
		typedef BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> > TInnerNode;
		typedef TFixedStringLeafNode<_TKey, _Transaction>	TLeafNode;

		typedef BPTreeNodeMapv2<_TKey, sFixedStringVal, _Transaction, TInnerCompressor, TLeafCompressor, TInnerNode,	 TLeafNode > TBase;

		typedef sFixedStringVal TValue;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::Transaction  Transaction;
 

		typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;





		BPFixedStringTreeNodeMapv2(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,  uint32 nPageSize, 
			TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) : 
		TBase( nParentAddr, pAlloc, nPageAddr, bMulti,  bIsLeaf, bCheckCRC32, nPageSize, pInnerCompParams,pLeafCompParams)
		{}
		~BPFixedStringTreeNodeMapv2()
		{}
	
	};
}
#endif