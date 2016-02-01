#ifndef _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_64_H_
#define _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_64_H_
#include "SpatialPointQuery.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
namespace embDB
{


	class BPSpatialPointInnerNodeSimpleCompressor64  
	{
	public:

		typedef  TBPVector<ZOrderPoint2DU64> TKeyMemSet;
		typedef  TBPVector<int64> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;
		BPSpatialPointInnerNodeSimpleCompressor64(TKeyMemSet* pKeyMemset, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParms = NULL) : m_nSize(0)
		{}



		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}

		virtual ~BPSpatialPointInnerNodeSimpleCompressor64(){}

		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);


			uint32 nKeySize =  m_nSize * 2 * sizeof(int64);
			uint32 nLinkSize =  m_nSize * sizeof(int64);


			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			ZOrderPoint2DU64 zPoint;
			int64 nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(zPoint.m_nZValue[0]);
				KeyStreams.read(zPoint.m_nZValue[1]);
				LinkStreams.read(nlink);

				keySet.push_back(zPoint);
				linkSet.push_back(nlink);
			}
			stream.seek(LinkStreams.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * 2 *sizeof(int64);
			uint32 nLinkSize =  nSize * sizeof(int64);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 

	 
			for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				ZOrderPoint2DU64& zOrder = keySet[i];
				KeyStreams.write(zOrder.m_nZValue[0]);
				KeyStreams.write(zOrder.m_nZValue[1]);
				LinkStreams.write(linkSet[i]);
			}
 
			return true;
		}


		virtual bool insert(int nIndex, const ZOrderPoint2DU64& key, int64 link )
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize += keySet.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(int nIndex, const ZOrderPoint2DU64& key, int64 link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(int nIndex, const ZOrderPoint2DU64& key, int64 link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (2 *sizeof(uint64) + sizeof(int64) ) *  m_nSize + sizeof(uint32) ;
		}
		virtual bool isNeedSplit(uint32 nPageSize) const
		{
			return nPageSize < size();
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize()
		{
			return (2 *sizeof(uint64) + sizeof(int64)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (2 *sizeof(uint64) + sizeof(uint64));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPSpatialPointInnerNodeSimpleCompressor64 *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		size_t m_nSize;
	};
}

#endif