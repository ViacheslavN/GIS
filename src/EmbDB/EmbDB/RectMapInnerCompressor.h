#ifndef _EMBEDDED_DATABASE_SPATIAL_RECT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_RECT_BTREE_PLUS_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _CoordPoint >
	class BPSpatialRectInnerNodeSimpleCompressor  
	{
	public:

		typedef _CoordPoint TCoordPoint;
		typedef int64 TLink;
		typedef  TBPVector<TCoordPoint> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;

	
		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}

		BPSpatialRectInnerNodeSimpleCompressor(uint32 nPageSize, TKeyMemSet* pKeyMemset, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = 0) : m_nSize(0),
			m_nPageSize(nPageSize)
		{}
		virtual ~BPSpatialRectInnerNodeSimpleCompressor(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * TCoordPoint::SizeInByte;
			uint32 nLinkSize =  m_nSize * sizeof(int64);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TCoordPoint zPoint;
			TLink nlink;
			size_t nCount = TCoordPoint::SizeInByte/8;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				for (size_t i = 0; i < nCount; ++i )
				{
					KeyStreams.read(zPoint.m_nZValue[i]);
				}
				LinkStreams.read(nlink);

				keySet.push_back(zPoint);
				linkSet.push_back(nlink);
			}
			assert(LinkStreams.pos() < stream.size());
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

			uint32 nKeySize =  nSize * TCoordPoint::SizeInByte;
			uint32 nLinkSize =  nSize * sizeof(int64);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);	
			
			size_t nCount =  TCoordPoint::SizeInByte/8;

			
			for (size_t i = 0, sz = keySet.size(); i < sz; ++i )
			{
			
				TCoordPoint& coord = keySet[i];
				for (size_t j = 0; j < nCount; ++j )
				{
					KeyStreams.write(coord.m_nZValue[j]);
				}
				LinkStreams.write(linkSet[i]);
			}
			return true;
		}

		virtual bool insert(int nIndex, const TCoordPoint& key, TLink link )
		{
			m_nSize++;
			assert(link!= 0);
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
		virtual bool remove(int nIndex, const TCoordPoint& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(int nIndex, const TCoordPoint& key, TLink link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (TCoordPoint::SizeInByte + sizeof(TLink) ) *  m_nSize + sizeof(uint32) ;
		}
		virtual bool isNeedSplit() const
		{
			return m_nPageSize < size();
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
			return (TCoordPoint::SizeInByte+ sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (TCoordPoint::SizeInByte + sizeof(TLink));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPSpatialRectInnerNodeSimpleCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		size_t m_nSize;
		uint32 m_nPageSize;
	};
}

#endif