#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"

namespace embDB
{

	template<class _TKey, class _TValue, class _Transaction = IDBTransaction, class _TKeyEncoder = TEmptyValueEncoder<_TKey>, class _TValueEncoder = TEmptyValueEncoder<_TValue>
		, class _TCompressorParams = CompressorParamsBaseImp>
			class TBaseNodeCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _TValueEncoder TValueEncoder;
			typedef _Transaction Transaction;

			typedef _TCompressorParams TCompressorParams;
			typedef _TCompressorParams TInnerCompressorParams; //TO DO temporary
			typedef STLAllocator<TKey> TKeyAlloc;
			typedef STLAllocator<TValue> TValueAlloc;
			typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
			typedef std::vector<TValue, TValueAlloc> TValueMemSet;


			TBaseNodeCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc = nullptr, TCompressorParams *pParams = nullptr) : m_nCount(0),
				m_nPageSize(nPageSize), m_KeyEncoder(nPageSize, pAlloc,  pParams), m_ValueEncoder(nPageSize, pAlloc, pParams)
			{}

			template<typename _Transactions  >
			static TCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new TCompressorParams();
			}

			template<typename _Transactions  >
			bool  init(TCompressorParams *pParams, _Transactions *pTran)
			{

				m_KeyEncoder.init(pParams, pTran);
				m_ValueEncoder.init(pParams, pTran);
				return true;
			}

			virtual ~TBaseNodeCompressor() {}
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

				if (!m_KeyEncoder.BeginEncoding(vecKeys))
					return false;
				if (!m_ValueEncoder.BeginEncoding(vecValues))
					return false;

				uint32 nKeySize = m_KeyEncoder.GetCompressSize();
				uint32 nValueSize = m_ValueEncoder.GetCompressSize();


				stream.write(nKeySize);
				stream.write(nValueSize);

				KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

				stream.seek(stream.pos() + nKeySize + nValueSize, CommonLib::soFromBegin);
				
				if (!m_KeyEncoder.encode(vecKeys, &KeyStream))
					return false;
				return m_ValueEncoder.encode(vecValues, &ValueStream);
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
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
					m_ValueEncoder.AddSymbol(m_nCount, i, vecValues[i], vecValues);
					
				}

				return true;
			}
			virtual bool updateValue(uint32 nIndex, TValue& newValue, const TValue& OldValue, const TValueMemSet& vecValues, const TKeyMemSet& vecKeys)
			{
				//	m_ValueEncoder.RemoveSymbol(m_nCount, nIndex, OldValue, vecValues);
				//	m_ValueEncoder.AddSymbol(m_nCount, nIndex, newValue, vecValues);

				m_ValueEncoder.UpdateSymbol(nIndex, newValue, OldValue, vecValues);

				return true;
			}
			virtual bool updateKey(uint32 nIndex, const TKey& NewKey, const TKey& OldTKey,  const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, OldTKey, vecKeys);
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, NewKey, vecKeys);
				return true;
			}
			virtual bool remove(uint32 nIndex, const TKey& key, const TValue& value, const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
			{
				m_nCount--;
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, key, vecKeys);
				m_ValueEncoder.RemoveSymbol(m_nCount, nIndex, value, vecValues);

				return true;
			}
			virtual uint32 size() const
			{
				return rowSize() + headSize();
			}
			virtual bool isNeedSplit() const
			{
				if (m_nCount > m_nPageSize * 8) //max bits for elem
					return true;

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
				return  m_ValueEncoder.GetCompressSize() + m_KeyEncoder.GetCompressSize();
			}
			uint32 tupleSize() const
			{
				return  (sizeof(TKey) + sizeof(TValue));
			}
			
			 

			virtual void recalcKey(const TKeyMemSet& vecKeys)
			{
				m_nCount = 0;
				m_KeyEncoder.clear();
				for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
				{
					m_nCount++;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);
				}
			}


			bool IsHaveUnion(TBaseNodeCompressor& pCompressor) const
			{
				if ((m_nCount + pCompressor.m_nCount) > m_nPageSize * 8) //max bits for elem
					return false;
				
				return (rowSize() + pCompressor.rowSize()) < (m_nPageSize - headSize());
			}
			bool IsHaveAlignment(TBaseNodeCompressor& pCompressor) const
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

