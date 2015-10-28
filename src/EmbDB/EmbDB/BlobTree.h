#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_

#include "BaseBPMapv2.h"
#include "BlobBPNode.h"
#include "PageAlloc.h"
namespace embDB
{


	template<class _TKey, class _Transaction>
	class TBPBlobTree : public TBPMapV2<_TKey, sBlobVal, comp<_TKey>, _Transaction, 
		BPInnerNodeSimpleCompressorV2<_TKey> ,
		BPStringLeafNodeCompressor<_TKey,  _Transaction>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		TBlobLeafNode<_TKey, _Transaction>,
		TBlobNodeMap<_TKey, _Transaction>	>
	{
	public:

		typedef TBPMapV2<_TKey, sBlobVal, comp<_TKey>, _Transaction, 
			BPInnerNodeSimpleCompressorV2<_TKey> ,
			BPStringLeafNodeCompressor<_TKey,  _Transaction>, 
			BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
			TBlobLeafNode<_TKey, _Transaction>,
			TBlobNodeMap<_TKey, _Transaction>	> TBase;

		TBPBlobTree(int64 nPageBTreeInfo, embDB::IDBTransactions* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPBlobTree()
		{
			DeleteNodes();
		}

		virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, nAdd, m_bMulti, bIsLeaf, m_bCheckCRC32,  m_InnerCompParams.get(),
				m_LeafCompParams.get());
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
				sValue.m_pBuf = m_pAlloc->alloc(blob.size());
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

			if(blobVal.m_nSize < m_LeafCompParams->GetMaxPageStringSize())
			{
				blob.copy(blobVal.m_pBuf, blobVal.m_nSize);
			}
			else
			{
				embDB::ReadStreamPagePtr pReadStream = m_LeafCompParams->GetReadStream(m_pTransaction, blobVal.m_nPage, blobVal.m_nPos);
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