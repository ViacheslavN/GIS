#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TKey, typename _TValue, class _Transaction = IDBTransaction>
	class BPLeafNodeMapSimpleCompressorV2
	{
	public:	
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;

		BPLeafNodeMapSimpleCompressorV2(uint32 nPageSize, _Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
			TLeafKeyMemSet *pKeyMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nCount(0),
			m_nPageSize(nPageSize)
		{}
		
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TLeafCompressorParams();
		}

		virtual ~BPLeafNodeMapSimpleCompressorV2(){}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			vecKeys.reserve(m_nCount);
			vecValues.reserve(m_nCount);

			uint32 nKeySize =  m_nCount * sizeof(TKey);
			uint32 nValueSize =  m_nCount * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			TKey key;
			TValue value;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStreams.read(key);
				ValueStreams.read(value);

				vecKeys.push_back(key);
				vecValues.push_back(value);
			}
 
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 
			for(uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i]);
				valueStreams.write(vecValues[i]);
			}
			
			return true;
		}
		virtual bool insert(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_nCount++;
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nCount += vecKeys.size();
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nCount = vecKeys.size();
			return true;
		}
		virtual bool update(uint32 nIndex, const TKey& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_nCount--;
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nCount +  sizeof(uint32);
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
			return  sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) *  m_nCount ;
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) ;
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeMapSimpleCompressorV2 *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}
		bool IsHaveUnion(BPLeafNodeMapSimpleCompressorV2 *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TKey) + sizeof(TValue));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPLeafNodeMapSimpleCompressorV2 *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{ 
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			return nNoCompSize  < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
	};
}

#endif