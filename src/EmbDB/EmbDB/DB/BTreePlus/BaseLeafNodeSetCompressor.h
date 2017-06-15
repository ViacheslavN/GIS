#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"

namespace embDB
{

		template<class _TKey,class _Transaction = IDBTransaction, class _TKeyEncoder = TEmptyValueEncoder<_TKey>, class _TCompressorParams = CompressorParamsBaseImp>
		class TBaseLeafNodeSetCompressor
		{
		public:
			typedef _TKey TKey;
			typedef _TKeyEncoder TKeyEncoder;
			typedef _Transaction Transaction;

			typedef _TCompressorParams TCompressorParams;
			typedef STLAllocator<TKey> TAlloc;
			typedef std::vector<TKey, TAlloc> TKeyMemSet;
 


			TBaseLeafNodeSetCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc = nullptr, TCompressorParams *pParams = nullptr) : m_nCount(0),
				m_nPageSize(nPageSize), m_KeyEncoder(nPageSize, pAlloc, pParams) 
			{}

			template<typename _Transactions  >
			static TCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new TCompressorParams();
			}

			template<typename _Transactions  >
			bool  init(TCompressorParams *pParams, _Transactions *pTran)
			{
				return true;
			}

			virtual ~TBaseLeafNodeSetCompressor() {}
			virtual bool Load(TKeyMemSet& vecKeys,  CommonLib::FxMemoryReadStream& stream)
			{

				CommonLib::FxMemoryReadStream KeyStream;
				m_nCount = stream.readIntu32();
				if (!m_nCount)
					return true;



				vecKeys.reserve(m_nCount);
				uint32 nKeySize = stream.readIntu32();

				KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
				m_KeyEncoder.decode(m_nCount, vecKeys, &KeyStream);
				return true;
			}
			virtual bool Write(TKeyMemSet& vecKeys,   CommonLib::FxMemoryWriteStream& stream)
			{
				uint32 nSize = (uint32)vecKeys.size();
				assert(m_nCount == nSize);
				stream.write(nSize);
				if (!nSize)
					return true;

	 
				uint32 nKeySize = m_KeyEncoder.GetCompressSize();

				stream.write(nKeySize);
 				return m_KeyEncoder.encode(vecKeys, &stream);
			}
			virtual bool insert(int nIndex, const TKey& key,const TKeyMemSet& vecKeys)
			{
				m_nCount++;
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, key, vecKeys);
				return true;
			}
			virtual bool add(const TKeyMemSet& vecKeys)
			{

				for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
				{
					m_KeyEncoder.AddSymbol(m_nCount, m_nCount + i, vecKeys[i], vecKeys);
					m_nCount++;
				}
				return true;
			}
			virtual bool recalc(const TKeyMemSet& vecKeys)
			{
				clear();
				for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
				{
					m_nCount += 1;
					m_KeyEncoder.AddSymbol(m_nCount, i, vecKeys[i], vecKeys);

				}

				return true;
			}
			virtual bool updateKey(uint32 nIndex, const TKey& NewKey, const TKey& OldTKey, const TKeyMemSet& vecKeys)
			{
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, OldTKey, vecKeys);
				m_KeyEncoder.AddSymbol(m_nCount, nIndex, NewKey, vecKeys);
				return true;
			}
			virtual bool remove(uint32 nIndex, const TKey& key,const TKeyMemSet& vecKeys)
			{
				m_nCount--;
				m_KeyEncoder.RemoveSymbol(m_nCount, nIndex, key, vecKeys);
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
				return  sizeof(uint32) + sizeof(uint32);
			}
			uint32 rowSize() const
			{
				return   m_KeyEncoder.GetCompressSize();
			}
			uint32 tupleSize() const
			{
				return  sizeof(TKey);
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


			bool IsHaveUnion(TBaseLeafNodeSetCompressor& pCompressor) const
			{
				if ((m_nCount + pCompressor.m_nCount) > m_nPageSize * 8) //max bits for elem
					return false;

				return (rowSize() + pCompressor.rowSize()) < (m_nPageSize - headSize());
			}
			bool IsHaveAlignment(TBaseLeafNodeSetCompressor& pCompressor) const
			{
				uint32 nNoCompSize = m_nCount * sizeof(TKey);
				return nNoCompSize < (m_nPageSize - headSize());
			}
			bool isHalfEmpty() const
			{
				uint32 nNoCompSize = m_nCount * sizeof(TKey);
				return nNoCompSize < (m_nPageSize - headSize()) / 2;
			}
			void clear()
			{
				m_nCount = 0;
				m_KeyEncoder.clear();
			}
		protected:

			uint32 m_nCount;
			TKeyEncoder m_KeyEncoder;
			uint32 m_nPageSize;

		};
}

