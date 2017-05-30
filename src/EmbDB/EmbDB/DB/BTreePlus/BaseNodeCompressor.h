#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../CompressorParams.h"
#include "../Utils/alloc/STLAlloc.h"

namespace embDB
{

	template<class _TKey, class _TValue, class _Transaction = IDBTransaction, class _TKeyEncoder = TEmptyValueEncoder<_TKey>, class _TValueEncoder = TEmptyValueEncoder<_TValue>
		, class _TCompressorParams = CompressorParamsBaseImp>
			class TBaseLeafNodeCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TValueEncoder TValueEncoder;
			typedef _Transaction Transaction;

			typedef _TCompressorParams TCompressorParams;
			typedef STLAllocator<TKey> TAlloc;
			typedef std::vector<TKey, TAlloc> TKeyMemSet;
			typedef std::vector<TValue, TAlloc> TValueMemSet;


			TBaseLeafNodeCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc = nullptr, TCompressorParams *pParams = nullptr) : m_nCount(0)
				m_nPageSize(nPageSize), m_KeyEncoder(pAlloc,  pParams), m_ValueEncoder(pAlloc, pParams)
			{}

			template<typename _Transactions  >
			static TCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new TCompressorParams();
			}

			virtual ~TBaseLeafNodeCompressor() {}
			virtual bool Load(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
			{

				CommonLib::FxMemoryReadStream KeyStream;
				CommonLib::FxMemoryReadStream ValueStream;

				m_nCount = stream.readIntu32();
				if (!m_nCount)
					return true;



				vecKeys.reserve(m_nCount);
				vecValues.reserve(m_nCount);

				uint32 nKeySize = stream.readIntu32();
				uint32 nValueSize = stream.readIntu32();
				
				KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

				m_KeyEncoder.decode(m_nCount, vecKeys, &KeyStream);
				m_ValueEncoder.decode(m_nCount, vecValues, &ValueStream);

				return true;
			}
			virtual bool Write(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
			{
				uint32 nSize = (uint32)vecKeys.size();
				assert(m_nCount == nSize);
				stream.write(nSize);
				if (!nSize)
					return true;

				CommonLib::FxMemoryWriteStream ValueStream;
				CommonLib::FxMemoryWriteStream KeyStream;

				uint32 nKeySize = m_KeyCompressor.GetCompressSize();
				uint32 nValueSize = m_ValueCompressor.GetCompressSize();


				stream.write(nKeySize);
				stream.write(nValueSize);

				KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

				stream.seek(stream.pos() + nKeySize + nValueSize, CommonLib::soFromBegin);
				
				m_KeyEncoder.encode(vecKeys, &KeyStream);
				m_ValueEncoder.encode(vecValues, &ValueStream);
				return true;
			}
			virtual bool insert(int nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_nCount++;
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, key, vecKeys);
				m_ValueEncoder.AddSymbol(m_nCount, nIndex, value, vecValues);

				return true;
			}
			virtual bool add(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{

				for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
				{	 
					m_KeyEncoder.AddSymbol(m_nCount, m_nCount + i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, m_nCount + i, vecValues[i], vecValues);
					m_nCount++;
				}
				return true;
			}
			virtual bool recalc(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				clear();
				for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
				{
					m_nCount += 1;
					m_KeyCompressor.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, i, vecValues[i], vecValues);
					
				}

				return true;
			}
			virtual bool update(uint32 nIndex, const TKey& key, const TValue& OldValue, const TValue& newValue, const TValueMemSet& vecValues)
			{
				m_ValueCompressor.RemoveSymbol(m_nCount, nIndex, OldValue, newValue, vecValues);
				m_ValueEncoder.AddSymbol(m_nCount, nIndex, newValue, vecValues);

				return true;
			}
			virtual bool remove(uint32 nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_nCount--;
				m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, key, vecKeys);
				m_ValueEncoder.RemoveSymbol(m_nCount, nIndex, value, vecValues);

				return true;
			}
			virtual uint32 size() const
			{
				return rowSize() + headSize();
			}
			virtual bool isNeedSplit() const
			{
				return !(m_nPageSize > size());
			}
			virtual uint32 count() const
			{
				return m_nCount;
			}
			uint32 headSize() const
			{
				return  sizeof(uint32) + sizeof(uint32) + sizeof(uint32);
			}
			uint32 rowSize() const
			{
				return  m_ValueEncoder.GetCompressSize() + m_KeyCompressor.GetCompressSize();
			}
			uint32 tupleSize() const
			{
				return  (sizeof(TKey) + sizeof(TValue));
			}
			
			virtual void recalc(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				clear();
				for (uint32 i = 0, sz = m_pKeyMemSet->size(); i < sz; ++i)
				{			 
					m_nCount++;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, i, vecValues[i], vecValues);
				}


			}

			virtual void recalcKey(const TKeyMemSet& vecKeys)
			{
				m_nCount = 0;
				m_KeyEncoder.clear();
				for (uint32 i = 1, sz = vecKeys; i < sz; ++i)
				{
					m_nCount++;
					m_KeyEncoder.AddSymbol(m_nCount, nIndex, vecKeys[i], vecValues);
				}
			}


			bool IsHaveUnion(TBaseLeafNodeComp *pCompressor) const
			{
				return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());
			}
			bool IsHaveAlignment(TBaseLeafNodeComp *pCompressor) const
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
				m_KeyEncoder.clear();
				m_ValueEncoder.clear();
			}
		protected:

			uint32 m_nCount;
			TKeyEncoder m_KeyEncoder;
			TValueEncoder m_ValueEncoder;

			uint32 m_nPageSize;

		};
}

