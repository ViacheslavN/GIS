#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "StringBPLeafNode.h"
#include "LeafStringCompress.h"

namespace embDB
{


	template<class _TKey, class _Transaction>
	class BPStringTreeNodeMapv2 : public BPTreeNodeMapv2<_TKey, sStringVal, _Transaction, 	BPInnerNodeSimpleCompressorV2<_TKey>,
		BPStringLeafNodeCompressor<_TKey,  _Transaction>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >,
		TStringLeafNode<_TKey, _Transaction> >
	{
	public:
		typedef BPInnerNodeSimpleCompressorV2<_TKey> TInnerCompressor;
		typedef BPStringLeafNodeCompressor<_TKey,  _Transaction> TLeafCompressor;
		typedef BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> > TInnerNode;
		typedef TStringLeafNode<_TKey, _Transaction>	TLeafNode;

		typedef BPTreeNodeMapv2<TKey, sStringVal, _Transaction, TInnerCompressor, TLeafCompressor, TInnerNode,	 TLeafNode > TBase;

		typedef sStringVal TValue;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::Transaction  Transaction;


		typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;





		BPStringTreeNodeMapv2(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32,
			TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) : 
		TBase( nParentAddr, pAlloc, nPageAddr, bMulti,  bIsLeaf, bCheckCRC32, pInnerCompParams,pLeafCompParams)
		{}
		~BPStringTreeNodeMapv2()
		{}


	 

	};
}
#endif