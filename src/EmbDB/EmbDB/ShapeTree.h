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


	template<class _Transaction>
	class TBPShapeTree : public TBPBlobTree<_Transaction, ShapeFieldCompressorParams> 
	
	{
	public:

		typedef TBPBlobTree<_Transaction, ShapeFieldCompressorParams>  TBase;
		typedef typename TBase::TBase      TSubBase;
		typedef typename TBase::TBTreeNode      TBTreeNode;
		typedef typename TBase::iterator      iterator;
		typedef typename TBase::TKey      TKey;

		TBPShapeTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodesPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodesPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPShapeTree()
		{
			this->DeleteNodes();
		}

		virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, this->m_pAlloc, nAdd, this->m_bMulti, bIsLeaf, this->m_bCheckCRC32, this->m_nNodesPageSize,  this->m_InnerCompParams.get(),
				this->m_LeafCompParams.get());
			pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
			return pNode;
		}


		CommonLib::eCompressDataType SpatialDataToCompressData(embDB::eSpatialType spatialType)
		{
			switch(spatialType)
			{
				case stPoint16:
				case stRect16:
					return CommonLib::dtType16;
					break;
				case stPoint32:
				case stRect32:
					return CommonLib::dtType32;
					break;
				case stPoint64:
				case stRect64:
					return CommonLib::dtType64;
					break;
			}

			return CommonLib::dtType64;
		}

		void convert(const CommonLib::IGeoShapePtr& shape, sBlobVal& sValue)
		{
			CommonLib::CWriteMemoryStream stream;

			ShapeFieldCompressorParams *pShapeParams = TBase::GetLeafCompressorParams();
			assert(pShapeParams);

			CommonLib::CGeoShape::compress_params shp_params;

			shp_params.m_dOffsetX = pShapeParams->GetOffsetX();
			shp_params.m_dOffsetY = pShapeParams->GetOffsetY();
			shp_params.m_nScaleX = pShapeParams->GetScaleX();
			shp_params.m_nScaleY = pShapeParams->GetScaleY();
			shp_params.m_PointType = SpatialDataToCompressData(pShapeParams->GetCoordType());

			shape->compress(&stream, &shp_params);

			sValue.m_nPage = -1;
			sValue.m_nBeginPos = 0;
			if(stream.size())
			{
				sValue.m_nSize = stream.size();
				sValue.m_pBuf = (byte*)this->m_pAlloc->alloc(stream.size());
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

			if(blobVal.m_nSize < this->m_LeafCompParams->GetMaxPageBlobSize())
			{
				 stream.attachBuffer(blobVal.m_pBuf, blobVal.m_nSize);
			}
			else
			{
				m_CacheBlob.resize(blobVal.m_nSize);
				embDB::ReadStreamPagePtr pReadStream = this->m_LeafCompParams->GetReadStream(this->m_pTransaction, blobVal.m_nPage, blobVal.m_nBeginPos);
				pReadStream->read(m_CacheBlob.buffer(), blobVal.m_nSize);
				stream.attachBuffer(m_CacheBlob.buffer(), m_CacheBlob.size());
			}

			CommonLib::CGeoShape::compress_params shp_params;
			ShapeFieldCompressorParams *pShapeParams = TBase::GetLeafCompressorParams();
			assert(pShapeParams);

			shp_params.m_dOffsetX = pShapeParams->GetOffsetX();
			shp_params.m_dOffsetY = pShapeParams->GetOffsetY();
			shp_params.m_nScaleX = pShapeParams->GetScaleX();
			shp_params.m_nScaleY = pShapeParams->GetScaleY();
			shp_params.m_PointType = SpatialDataToCompressData(pShapeParams->GetCoordType());

			shape->decompress(&stream, &shp_params);
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