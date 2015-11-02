#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_SHAPE_BLOB_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_SHAPE_BLOB_H_

#include "BaseBPMapv2.h"
#include "BlobBPNode.h"
#include "PageAlloc.h"
#include "ShapeCompressorParams.h"
#include "CommonLibrary/GeoShape.h"
#include "BlobTree.h"
namespace embDB
{


	template<class _TKey, class _Transaction>
	class TBPShapeTree : public TBPBlobTree<_TKey,  _Transaction, ShapeFieldCompressorParams> 
	
	{
	public:

		typedef TBPBlobTree<_TKey,  _Transaction, ShapeFieldCompressorParams>  TBase;
		typedef typename TBase::TBase      TSubBase;

		TBPShapeTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPShapeTree()
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
		void convert(const CommonLib::CGeoShape& shape, sBlobVal& sValue)
		{

			/*sValue.m_nPage = -1;
			sValue.m_nBeginPos = 0;
			if(blob.size())
			{
				sValue.m_nSize = blob.size();
				sValue.m_pBuf = (byte*)m_pAlloc->alloc(blob.size());
				memcpy(sValue.m_pBuf, blob.buffer(), blob.size());
			}
			else
			{
				sValue.m_nSize = 0;
				sValue.m_pBuf = NULL;
			}*/
		}
		void convert(const sBlobVal& blobVal, CommonLib::CGeoShape& shape) 
		{

			/*blob.resize(blobVal.m_nSize);
			if(!blobVal.m_nSize)
				return;

			if(blobVal.m_nSize < m_LeafCompParams->GetMaxPageBlobSize())
			{
				blob.copy(blobVal.m_pBuf, blobVal.m_nSize);
			}
			else
			{
				embDB::ReadStreamPagePtr pReadStream = m_LeafCompParams->GetReadStream(m_pTransaction, blobVal.m_nPage, blobVal.m_nBeginPos);
				pReadStream->read(blob.buffer(), blobVal.m_nSize);

			}*/

		}

		bool insert(int64 nValue, const CommonLib::CGeoShape& shape, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::insert(nValue, sValue, pFromIterator, pRetItertor);

		}
		bool update(const TKey& key, const CommonLib::CGeoShape& shape)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::update(key, sValue);
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::CGeoShape& shape, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::insertLast(keyFunctor, sValue, pKey, pFromIterator, pRetIterator);
		}

	private:
		CPageAlloc m_PageAlloc;

	};

}

#endif