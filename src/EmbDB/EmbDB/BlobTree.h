#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_

#include "BaseBPMapv2.h"
#include "BlobBPNode.h"
#include "PageAlloc.h"
namespace embDB
{
	
	template<class _Transaction, class _TCompParams = BlobFieldCompressorParams>
	class TBPBlobTree : public TBPMapV2<int64, sBlobVal, comp<int64>, _Transaction, 
		TInnerCompressor ,
		BlobLeafNodeCompressor<_Transaction, _TCompParams, BlobCompressor <_Transaction, _TCompParams> >, 
		BPTreeInnerNodeSetv2<int64, _Transaction, TInnerCompressor >, 
		TBlobLeafNode<int64, _Transaction, _TCompParams>,
		TBlobNodeMap<int64, _Transaction, _TCompParams>	>
	{
	public:

		typedef TBPMapV2<int64, sBlobVal, comp<int64>, _Transaction, 
			TInnerCompressor,
			BlobLeafNodeCompressor< _Transaction, _TCompParams, BlobCompressor <_Transaction, _TCompParams> >, 
			BPTreeInnerNodeSetv2<int64, _Transaction, TInnerCompressor >, 
			TBlobLeafNode<int64, _Transaction, _TCompParams>,
			TBlobNodeMap<int64, _Transaction, _TCompParams>	> TBase;


		typedef typename TBase::TBTreeNode	TBTreeNode;
		typedef typename TBase::iterator	iterator;
		typedef typename TBase::TKey	TKey;

		TBPBlobTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPBlobTree()
		{
			this->DeleteNodes();
		}

		virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, this->m_pAlloc, nAdd, this->m_bMulti, bIsLeaf, this->m_bCheckCRC32, this->m_nNodesPageSize, this->m_InnerCompParams.get(),
				this->m_LeafCompParams.get());
			pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
			return pNode;
		}
		void convert(const CommonLib::CBlob& blob, sBlobVal& sValue)
		{

			sValue.m_nPage = -1;
			sValue.m_nBeginPos = 0;
			if(blob.size())
			{
				sValue.m_nSize = blob.size();
				sValue.m_pBuf = (byte*)this->m_pAlloc->alloc(blob.size());
				memcpy(sValue.m_pBuf, blob.buffer(), blob.size());
			}
			else
			{
				sValue.m_nSize = 0;
				sValue.m_pBuf = NULL;
			}
		}
		void convert(const sBlobVal& blobVal, CommonLib::CBlob& blob) 
		{

			blob.resize(blobVal.m_nSize);
			if(!blobVal.m_nSize)
				return;

			if(blobVal.m_nSize < this->m_LeafCompParams->GetMaxPageBlobSize())
			{
				blob.copy(blobVal.m_pBuf, blobVal.m_nSize);
			}
			else
			{
				embDB::ReadStreamPagePtr pReadStream = this->m_LeafCompParams->GetReadStream(this->m_pTransaction, blobVal.m_nPage, blobVal.m_nBeginPos);
				pReadStream->read(blob.buffer(), blobVal.m_nSize);
				 
			}

		}

		bool insert(int64 nValue, const CommonLib::CBlob& blob, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::insert(nValue, sValue, pFromIterator, pRetItertor);

		}
		bool update(const TKey& key, const CommonLib::CBlob& blob)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::update(key, sValue);
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::CBlob& blob, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::insertLast(keyFunctor, sValue, pKey, pFromIterator, pRetIterator);
		}
	
	private:
		CPageAlloc m_PageAlloc;
 
	};

}

#endif