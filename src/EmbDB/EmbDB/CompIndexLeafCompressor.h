#ifndef _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "CompositeIndexKey.h"
#include "CompressCompIndexParams.h"
#include "BPVectorNoPod.h"
namespace embDB
{

	template<typename _TValue = uint64>
	class BPLeafCompIndexCompressor
	{
	public:	
		typedef _TValue TValue;
		typedef TBPVectorNoPOD<CompositeIndexKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;

		typedef CompIndexParams TLeafCompressorParamsBase;

		BPLeafCompIndexCompressor(CommonLib::alloc_t *pAlloc, TLeafCompressorParamsBase *pParams) 
			: m_nSize(0), m_pAlloc(pAlloc), m_pCompParams(pParams)
		{
			assert(m_pCompParams);
		}
		virtual ~BPLeafCompIndexCompressor(){}


		template<typename _Transactions  >
		static TLeafCompressorParamsBase *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{

			CompIndexParams *pCompParams = new  CompIndexParams();
			pCompParams->setRootPage(nPage);
			pCompParams->read(pTran);
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

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValueSize);

		 
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

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				vecKeys[i].write(KeyStreams);
				valueStreams.write(vecValues[i]);
			}

			return true;
		}

		virtual bool insert(const CompositeIndexKey& key, const TValue& value)
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
		virtual bool update(const CompositeIndexKey& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(const CompositeIndexKey& key, const TValue& value)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (m_pCompParams->getRowSize() + sizeof(TValue))*  m_nSize +  sizeof(uint32);
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize() const
		{
			return (m_pCompParams->getRowSize() + sizeof(TValue)) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  m_pCompParams->getRowSize() + sizeof(TValue);
		}
	private:
		size_t m_nSize;
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParamsBase* m_pCompParams;

	};
}

#endif