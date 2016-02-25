#ifndef _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_INNER_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "BPVector.h"
#include "SpatialPointCompressor.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "LinkCompress.h"
namespace embDB
{

	template<typename _ZOrderType>
	class BPSpatialPointInnerCompressor  
	{
	public:

		typedef _ZOrderType ZOrderType;
		typedef typename ZOrderType::TPointType TPointType;
		typedef int64 TLink;
		typedef  TBPVector<ZOrderType> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;
		typedef TSpatialPointCompress<ZOrderType, TPointType> TZOrderCompressor;


		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;




		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}

		BPSpatialPointInnerCompressor(uint32 nPageSize, TKeyMemSet* pKeyMemset, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParms = NULL) : m_nCount(0),
			m_nPageSize(nPageSize), m_pKeyMemSet(pKeyMemset), m_pLinkMemSet(pLinkMemSet)
		{}
		virtual ~BPSpatialPointInnerCompressor(){}

		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			keySet.reserve(m_nCount);
			linkSet.reserve(m_nCount);

			uint32 nKeySize =  stream.readIntu32();
			uint32 nLinkSize =  stream.readIntu32();

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			m_ZOrderCompressor.decompress(m_nCount, keySet, &KeyStreams);
			m_LinkCompressor.decompress(m_nCount, linkSet, &LinkStreams);
			 
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  m_ZOrderCompressor.GetComressSize();
			uint32 nLinkSize =  m_LinkCompressor.GetComressSize();


			stream.write(nKeySize);
			stream.write(nLinkSize);


			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);


			m_ZOrderCompressor.compress(keySet, &KeyStreams);
			m_LinkCompressor.compress(linkSet, &LinkStreams);

			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			 

			return true;
		}

		virtual bool insert(int32 nIndex, const ZOrderType& key, TLink link )
		{
			m_nCount++;
			m_ZOrderCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.AddLink(link);
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount += keySet.size();
			uint32 nOff = m_nCount;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], linkSet[i]);
			}
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount = keySet.size();
			m_LinkCompressor.clear();
			m_ZOrderCompressor.clear();
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				if(i != 0)
					m_ZOrderCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				m_LinkCompressor.AddLink(linkSet[i]);
			}

			return true;
		
		}
		virtual bool remove(int nIndex, const ZOrderType& key, TLink link)
		{
			m_nCount--;
			m_ZOrderCompressor.RemoveSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.RemoveLink(link);
			return true;
		}
		virtual bool update(int nIndex, const ZOrderType& key, TLink link)
		{

			m_ZOrderCompressor.RemoveSymbol(m_nCount, nIndex, (*m_pKeyMemSet)[nIndex], *m_pKeyMemSet);
			m_LinkCompressor.RemoveLink((*m_pLinkMemSet)[nIndex]);


			m_ZOrderCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.AddLink(link);
			return true;
		}
		virtual uint32 size() const
		{
			return  3 *sizeof(uint32) + rowSize();
		}
		virtual bool isNeedSplit() const
		{
			return m_nPageSize < size();
		}
		virtual uint32 count() const
		{
			return m_nCount;
		}
		uint32 headSize() const
		{
			return  3 * sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return m_ZOrderCompressor.GetComressSize() + m_LinkCompressor.GetComressSize();
		}
		void clear()
		{
			m_nCount = 0;
			m_LinkCompressor.clear();
			m_ZOrderCompressor.clear();
		}
		uint32 tupleSize() const
		{
			return  (ZOrderType::SizeInByte + sizeof(TLink));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPSpatialPointInnerCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}

		bool IsHaveUnion(BPSpatialPointInnerCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TLink));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(ZValueType) + sizeof(TLink));

			return (nNoCompSize + nNoCompSizeUnion + tupleSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPSpatialPointInnerCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
		InnerLinkCompress m_LinkCompressor;
		TZOrderCompressor m_ZOrderCompressor;
		TKeyMemSet* m_pKeyMemSet;
		TLinkMemSet* m_pLinkMemSet;

	};
}

#endif