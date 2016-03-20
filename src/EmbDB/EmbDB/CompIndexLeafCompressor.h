#ifndef _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "CompositeIndexKey.h"
#include "CompressCompIndexParams.h"
#include "BPVectorNoPod.h"
namespace embDB
{

	template<typename _TValue = int64, class _Transaction = IDBTransaction>
	class BPLeafCompIndexCompressor
	{
	public:	
		typedef _TValue TValue;
		typedef TBPVectorNoPOD<CompositeIndexKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;

		typedef CompIndexParams TLeafCompressorParams;

		BPLeafCompIndexCompressor(_Transaction *pTran, CommonLib::alloc_t *pAlloc, TLeafCompressorParams *pParams,
			TLeafKeyMemSet *pKeyMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) 
			: m_nSize(0), m_pAlloc(pAlloc), m_pCompParams(pParams)
		{
			assert(m_pCompParams);
		}
		virtual ~BPLeafCompIndexCompressor(){}


		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{

			CompIndexParams *pCompParams = new  CompIndexParams();
			return pCompParams;
		}


		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * m_pCompParams->getRowSize();
			uint32 nValueSize =  m_nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

		 
			TValue value;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				CompositeIndexKey key(m_pAlloc);
			
				if(!key.load(m_pCompParams->getScheme(), KeyStreams))
				{
					return false;
				}
				ValueStreams.read(value);

				vecKeys.push_back(key);
				vecValues.push_back(value);
			}

			return true;
		}
		virtual bool Write(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;

			uint32 nKeySize =  nSize  * m_pCompParams->getRowSize();
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 
			for(uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				vecKeys[i].write(KeyStreams);
				valueStreams.write(vecValues[i]);
			}

			return true;
		}

		virtual bool insert(uint32 nIndex, const CompositeIndexKey& key, const TValue& value)
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize += vecKeys.size();
			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& vecKeys, const TLeafValueMemSet& vecValues)
		{
			m_nSize = vecKeys.size();
			return true;
		}
		virtual bool update(uint32 nIndex, const CompositeIndexKey& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(uint32 nIndex, const CompositeIndexKey& key, const TValue& value)
		{
			m_nSize--;
			return true;
		}
		virtual uint32 size() const
		{
			return (m_pCompParams->getRowSize() + sizeof(TValue))*  m_nSize +  sizeof(uint32);
		}
		virtual bool isNeedSplit(uint32 nPageSize) const
		{
			return nPageSize < size();
		}
		virtual uint32 count() const
		{
			return m_nSize;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return (m_pCompParams->getRowSize() + sizeof(TValue)) *  m_nSize;
		}
		uint32 tupleSize() const
		{
			return  m_pCompParams->getRowSize() + sizeof(TValue);
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafCompIndexCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		uint32 m_nSize;
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams* m_pCompParams;

	};
}

#endif