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

		TBPShapeTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, uint32 nNodesPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodesPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPShapeTree()
		{
			DeleteNodes();
		}

		virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, nAdd, m_bMulti, bIsLeaf, m_bCheckCRC32, m_nNodesPageSize,  m_InnerCompParams.get(),
				m_LeafCompParams.get());
			pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
			return pNode;
		}
		void convert(const CommonLib::IGeoShapePtr& shape, sBlobVal& sValue)
		{
			CommonLib::MemoryStream stream;
			shape->write(&stream);

			sValue.m_nPage = -1;
			sValue.m_nBeginPos = 0;
			if(stream.size())
			{
				sValue.m_nSize = stream.size();
				sValue.m_pBuf = (byte*)m_pAlloc->alloc(stream.size());
				memcpy(sValue.m_pBuf, stream.buffer(), stream.size());
			}
			else
			{
				sValue.m_nSize = 0;
				sValue.m_pBuf = NULL;
			}
		}
		void convert(const sBlobVal& blobVal, CommonLib::IGeoShapePtr& shape) 
		{
 

			CommonLib::FxMemoryReadStream stream;
			if(!blobVal.m_nSize)
				return;

			if(blobVal.m_nSize < m_LeafCompParams->GetMaxPageBlobSize())
			{
				 stream.attach(blobVal.m_pBuf, blobVal.m_nSize);
			}
			else
			{
				m_CacheBlob.resize(blobVal.m_nSize);
				embDB::ReadStreamPagePtr pReadStream = m_LeafCompParams->GetReadStream(m_pTransaction, blobVal.m_nPage, blobVal.m_nBeginPos);
				pReadStream->read(m_CacheBlob.buffer(), blobVal.m_nSize);
				stream.attach(m_CacheBlob.buffer(), m_CacheBlob.size());
			}
			shape->read(&stream);
		}

		bool insert(int64 nValue, const CommonLib::IGeoShapePtr& shape, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::insert(nValue, sValue, pFromIterator, pRetItertor);

		}
		bool update(const TKey& key, const CommonLib::IGeoShapePtr& shape)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::update(key, sValue);
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::IGeoShapePtr& shape, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
		{
			sBlobVal sValue;
			convert(shape, sValue);
			return TSubBase::insertLast(keyFunctor, sValue, pKey, pFromIterator, pRetIterator);
		}

	private:
		CPageAlloc m_PageAlloc;
		CommonLib::CBlob m_CacheBlob; //

	};

}

#endif