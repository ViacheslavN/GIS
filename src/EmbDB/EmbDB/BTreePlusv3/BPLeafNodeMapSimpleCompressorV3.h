#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../CompressorParams.h"
#include "../STLAlloc.h"
namespace embDB
{

	template<typename _TKey, typename _TValue>
	class BPLeafNodeMapSimpleCompressorV3
	{
	public:
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef CompressorParamsBaseImp TLeafCompressorParams;
		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;
		typedef std::vector<TValue, TAlloc> TValueMemSet;


		BPLeafNodeMapSimpleCompressorV3(uint32 nPageSize, CommonLib::alloc_t *pAlloc = nullptr, TLeafCompressorParams *pParams = nullptr) : m_nCount(0),
			m_nPageSize(nPageSize)
		{}
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TLeafCompressorParams();
		}
		virtual ~BPLeafNodeMapSimpleCompressorV3() {}

		template<typename _Transactions  >
		bool  init(TLeafCompressorParams *pParams, _Transactions *pTran)
		{
			return true;
		}

		virtual bool Load(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nCount = stream.readInt32();
			if (!m_nCount)
				return true;

			vecKeys.reserve(m_nCount);
			vecValues.reserve(m_nCount);

			uint32 nKeySize = m_nCount * sizeof(TKey);
			uint32 nValueSize = m_nCount * sizeof(TValue);

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
		virtual bool Write(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if (!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;

			uint32 nKeySize = nSize * sizeof(TKey);
			uint32 nValuesSize = nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);
			for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i]);
				valueStreams.write(vecValues[i]);
			}

			return true;
		}

		virtual bool insert(int nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vector, const TValueMemSet& vecValues)
		{
			m_nCount++;
			return true;
		}
		virtual bool add(const TKeyMemSet& Set, const TValueMemSet& vecValues)
		{
			m_nCount += Set.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& Set, const TValueMemSet& vecValues)
		{
			m_nCount = Set.size();
			return true;
		}
		virtual bool update(int nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vector, const TValueMemSet& vecValues)
		{
			return true;
		}
		virtual bool remove(int nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vector, const TValueMemSet& vecValues)
		{
			m_nCount--;
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nCount + sizeof(uint32);
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
			return  (sizeof(TKey) + sizeof(TValue)) *  m_nCount;
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeMapSimpleCompressorV3& pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd - nBegin;

			m_nCount -= nSize;
			pCompressor.m_nCount += nSize;
		}
		bool IsHaveUnion(BPLeafNodeMapSimpleCompressorV3 &pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			uint32 nNoCompSizeUnion = pCompressor.m_nCount * (sizeof(TKey) + sizeof(TValue));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());
		}
		bool IsHaveAlignment(BPLeafNodeMapSimpleCompressorV3 &pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{			
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize()) / 2;
		}

		void clear()
		{
			m_nCount = 0;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
	};
}


