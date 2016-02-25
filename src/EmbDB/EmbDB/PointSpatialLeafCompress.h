#ifndef _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_SPATIAL_POINT_BTREE_PLUS_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "BPVector.h"
#include "SpatialPointCompressor.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "LinkCompress.h"
namespace embDB
{

	template<typename _ZOrderType, typename _TValue, class _Transaction = IDBTransaction>
	class BPSpatialPointLeafCompressor  
	{
	public:

		typedef _ZOrderType ZOrderType;
		typedef typename ZOrderType::TPointType TPointType;
		typedef _TValue TValue;
		typedef  TBPVector<ZOrderType> TKeyMemSet;
		typedef  TBPVector<TValue> TValueMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;
		typedef TSpatialPointCompress<ZOrderType, TPointType> TZOrderCompressor;


		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;


		typedef TUnsignedNumLenCompressor<TValue, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder, sizeof(TValue) * 8> TValueCompressor;




		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}


		

		BPSpatialPointLeafCompressor(uint32 nPageSize, _Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
			TKeyMemSet *pKeyMemset = NULL, TValueMemSet *pValueMemSet = NULL) : m_nCount(0),
			m_nPageSize(nPageSize), m_pKeyMemSet(pKeyMemset), m_pValueMemSet(pValueMemSet)
		{}
		virtual ~BPSpatialPointLeafCompressor(){}

		virtual bool Load(TKeyMemSet& keySet, TValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			keySet.reserve(m_nCount);
			valueSet.reserve(m_nCount);

			uint32 nKeySize =  stream.readIntu32();
			uint32 nValueSize =  stream.readIntu32();

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);


			m_ZOrderCompressor.decompress(m_nCount, keySet, &KeyStreams);
			m_ValueCompressor.decompress(valueSet, &ValueStreams);

			stream.seek(stream.pos() + nKeySize + nValueSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TValueMemSet& valueSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValueStreams;

			uint32 nKeySize =  m_ZOrderCompressor.GetComressSize();
			uint32 nValueSize =  m_ValueCompressor.GetCompressSize();


			stream.write(nKeySize);
			stream.write(nValueSize);


			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);


			m_ZOrderCompressor.compress(keySet, &KeyStreams);
			m_ValueCompressor.compress(valueSet, &ValueStreams);

			stream.seek(stream.pos() + nKeySize + nValueSize, CommonLib::soFromBegin);			 


			return true;
		}

		virtual bool insert(int32 nIndex, const ZOrderType& key, TValue value )
		{
			if(m_nCount > 0)
			assert(m_nCount == (m_ZOrderCompressor.count()/2 + 1));
			m_nCount++;
			m_ZOrderCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_ValueCompressor.AddSymbol(value);

				assert(m_nCount == (m_ZOrderCompressor.count()/2 + 1));
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TValueMemSet& valueSet)
		{
			m_nCount += keySet.size();
			uint32 nOff = m_nCount;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], valueSet[i]);
			}
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TValueMemSet& valueSet)
		{
			m_nCount = keySet.size();
			m_ValueCompressor.clear();
			m_ZOrderCompressor.clear();
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				if(i != 0)
					m_ZOrderCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				m_ValueCompressor.AddSymbol(valueSet[i]);
			}

			return true;

		}
		virtual bool remove(int nIndex, const ZOrderType& key, TValue value)
		{
			m_nCount--;
			m_ZOrderCompressor.RemoveSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_ValueCompressor.RemoveSymbol(value);
			return true;
		}
		virtual bool update(int nIndex, const ZOrderType& key, TValue value)
		{

			m_ZOrderCompressor.RemoveSymbol(m_nCount, nIndex, (*m_pKeyMemSet)[nIndex], *m_pKeyMemSet);
			m_ValueCompressor.RemoveSymbol((*m_pValueMemSet)[nIndex]);


			m_ZOrderCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_ValueCompressor.AddSymbol(value);
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
			return m_ZOrderCompressor.GetComressSize() + m_ValueCompressor.GetCompressSize();
		}
		void clear()
		{
			m_nCount = 0;
			m_ValueCompressor.clear();
			m_ZOrderCompressor.clear();
		}
		uint32 tupleSize() const
		{
			return  (ZOrderType::SizeInByte + sizeof(TValue));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPSpatialPointLeafCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;

			recalc(*m_pKeyMemSet, *m_pValueMemSet);
			pCompressor->recalc(*pCompressor->m_pKeyMemSet, *pCompressor->m_pValueMemSet);
		}

		bool IsHaveUnion(BPSpatialPointLeafCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TValue));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(ZValueType) + sizeof(TValue));

			return (nNoCompSize + nNoCompSizeUnion + tupleSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPSpatialPointLeafCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(ZValueType) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
		TValueCompressor m_ValueCompressor;
		TZOrderCompressor m_ZOrderCompressor;
		TKeyMemSet* m_pKeyMemSet;
		TValueMemSet* m_pValueMemSet;

	};
}

#endif