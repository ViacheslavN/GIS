#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CompressorParams.h"
#include "BPTreeNodeSetV2.h"
#include "BlobLeafNode.h"
#include "BlobLeafNodeCompressor.h"
#include "BaseValueDiffCompressor.h"
#include "BaseInnerNodeDIffCompress2.h"
namespace embDB
{
	typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress;
	typedef TBPBaseInnerNodeDiffCompressor2<int64, embDB::OIDCompressor, TInnerLinkCompress>  TInnerCompressor;


	template<class _TKey, class _Transaction, class _TCompParams = BlobFieldCompressorParams>
	class TBlobNodeMap : public BPTreeNodeMapv2<_TKey, sBlobVal, _Transaction, 	TInnerCompressor,
		BlobLeafNodeCompressor<_Transaction, _TCompParams, BlobCompressor <_Transaction, _TCompParams> >, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, TInnerCompressor >,
		TBlobLeafNode<_TKey, _Transaction, _TCompParams> >
	{
	public:
	 
		typedef BlobLeafNodeCompressor< _Transaction, _TCompParams, BlobCompressor <_Transaction, _TCompParams> > TLeafCompressor;
		typedef BPTreeInnerNodeSetv2<_TKey, _Transaction, TInnerCompressor > TInnerNode;
		typedef TBlobLeafNode<_TKey, _Transaction, _TCompParams>	TLeafNode;

		typedef BPTreeNodeMapv2<_TKey, sBlobVal, _Transaction, TInnerCompressor, TLeafCompressor, TInnerNode,	 TLeafNode > TBase;

		typedef sBlobVal TValue;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::Transaction  Transaction;


		typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;





		TBlobNodeMap(int64 nParentAddr, CommonLib::alloc_t *pAlloc, int64 nPageAddr, bool bMulti, bool  bIsLeaf, bool bCheckCRC32, uint32 nPageSize,
			TInnerCompressorParams *pInnerCompParams = NULL, TLeafCompressorParams *pLeafCompParams = NULL) : 
		TBase( nParentAddr, pAlloc, nPageAddr, bMulti,  bIsLeaf, bCheckCRC32, nPageSize, pInnerCompParams,pLeafCompParams)
		{}
		~TBlobNodeMap()
		{}




	};
}
#endif