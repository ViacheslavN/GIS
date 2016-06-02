#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "FixedStringBPLeafNode.h"
#include "FixedStringLeafCompressor.h"

#include "BaseInnerNodeDIffCompress.h"
#include "BaseLeafNodeCompDiff.h"
#include "BaseInnerNodeDIffCompress2.h"
#include "BaseValueDiffCompressor.h"
#include "SignedNumLenDiffCompress.h"



namespace embDB
{

	typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress; 


	template<class _TKey, class _Transaction>
	class BPFixedStringTreeNodeMapv2 : public BPTreeNodeMapv2<_TKey, sFixedStringVal, _Transaction, 
		TBPBaseInnerNodeDiffCompressor2<_TKey,  embDB::OIDCompressor, TInnerLinkCompress>,
		TBPFixedStringLeafCompressor<_TKey, _Transaction>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, TBPBaseInnerNodeDiffCompressor2<_TKey,  embDB::OIDCompressor, TInnerLinkCompress> >,
		TFixedStringLeafNode<_TKey, _Transaction> >
	{
	public:
		typedef TBPBaseInnerNodeDiffCompressor2<_TKey,  embDB::OIDCompressor, TInnerLinkCompress> TInnerCompressor;
		typedef TBPFixedStringLeafCompressor<_TKey, _Transaction> TLeafCompressor;
		typedef BPTreeInnerNodeSetv2<_TKey, _Transaction, TInnerCompressor > TInnerNode;
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
		{
 

		}
		~BPFixedStringTreeNodeMapv2()
		{
 
		}
	
	};
}
#endif