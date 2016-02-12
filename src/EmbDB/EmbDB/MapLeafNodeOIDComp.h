#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
#include <set>
#include <vector>
#include <map>
#include "NumLenCompress.h"
#include "NumLenDiffCompress.h"
#include "IntegerDiffCompress.h"
#include "ArithmeticCoder.h"
#include "RangeCoder.h"
#include "BPVector.h"
#include "OIDCompress.h"
namespace embDB
{

	template<typename _TValue, class _Transaction = IDBTransaction>
	class TMapLeafNodeOIDComp
	{
	public:	
	 
		typedef int64 TOID;
		typedef _TValue TValue;
		typedef  TBPVector<TOID> TOIDMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;

		enum eSchemeCompress
		{
			eCompressDiff = 1,
			eCopmressNumLen

		};


		typedef TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef TACEncoder<uint64, 32> TACEncoder;
		typedef TACDecoder<uint64, 32> TACDecoder;

		typedef TUnsignedDiffNumLenCompressor<int64, TFindMostSigBit, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder, 64> TNumLenCompressor;

		typedef embDB::TUnsignedIntegerDiffCompress<int64, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder> TDiffCompressor;

		TMapLeafNodeOIDComp(_Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
			TOIDMemSet *pOIDMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nSize(0), m_pOIDMemSet(pOIDMemset)
		{

			assert(m_pOIDMemSet);

		}

		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}

		virtual ~TMapLeafNodeOIDComp(){}
		virtual bool Load(TOIDMemSet& vecOIDs, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream OIDStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecOIDs.reserve(m_nSize);
			vecValues.reserve(m_nSize);

			uint32 nOIDSize =  stream.readIntu32();
			uint32 nValueSize =  m_nSize * sizeof(TValue);

			OIDStreams.attachBuffer(stream.buffer() + stream.pos(), nOIDSize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			m_OIDCompressor.decompress(m_nSize, vecOIDs, &OIDCompress);
			TValue value;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
	 
				ValueStreams.read(value);
				vecValues.push_back(value);
			}

			return true;
		}
		virtual bool Write(TOIDMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nSize == nSize);
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
			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i]);
				valueStreams.write(vecValues[i]);
			}

			return true;
		}
		virtual bool insert(uint32 nIndex, const TOID& key, const TValue& value)
		{
			m_nSize++;
			if(m_nSize > 1)
			{

				if(nIndex == 0)
				{
					m_OIDCompressor.AddDiffSymbol((*m_pOIDMemSet)[nIndex + 1] - key); 
				}
				else
				{
					TOID nPrevOID =  (*m_pOIDMemSet)[nIndex - 1];
					if(nIndex == (m_pOIDMemSet->size() - 1))
					{
						m_OIDCompressor.AddDiffSymbol(key - nPrevOID); 
					}
					else
					{
						TOID nNextOID =  (*m_pOIDMemSet)[nIndex + 1];
						int64 nOldSymbol = nNextOID - nPrevOID;

						m_OIDCompressor.RemoveDiffSymbol(nOldSymbol);


						m_OIDCompressor.AddDiffSymbol(key - nPrevOID); 
						m_OIDCompressor.AddDiffSymbol(nNextOID - key); 
					}
				}
			}
			return true;
		}
		virtual bool add(const TOIDMemSet& vecOIDs, const TOIDMemSet& vecValues)
		{
			uint32 nOff = m_nSize;
			for (uint32 i = 0, sz = vecOIDs.size(); i < sz; 	++i)
			{
				insert(i + nOff, vecOIDs[i], vecValues[i]);
			}
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize = keySet.size();
			m_LinkCompressor.clear();
			m_OIDCompressor.clear();
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				//insert(i, keySet[i], linkSet[i]);
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				m_LinkCompressor.AddLink(linkSet[i]);
			}
			return true;
		}
		virtual bool update(uint32 nIndex, const TKey& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nSize +  headSize();
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
			return  sizeof(uint32) + sizeof(uint32);
		}
		size_t rowSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) *  m_nSize ;
		}
		size_t tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) ;
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeMapSimpleCompressorV2 *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		size_t m_nSize;
		OIDCompress m_OIDCompressor;
		TOIDMemSet* m_pOIDMemSet;
		 
	};
}

#endif