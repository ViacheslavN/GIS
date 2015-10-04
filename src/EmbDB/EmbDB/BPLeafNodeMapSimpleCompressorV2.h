#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TKey, typename _TValue>
	class BPLeafNodeMapSimpleCompressorV2
	{
	public:	
		typedef _TKey TKey;
		typedef _TValue TValue;
		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;

		BPLeafNodeMapSimpleCompressorV2(CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL) : m_nSize(0)
		{}
		
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			return NULL;
		}

		virtual ~BPLeafNodeMapSimpleCompressorV2(){}
		virtual bool Load(TLeafKeyMemSet& vecKeys, TLeafValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValueStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			vecKeys.reserve(m_nSize);
			vecValues.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(TKey);
			uint32 nValueSize =  m_nSize * sizeof(TValue);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			TKey key;
			TValue value;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
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
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream valueStreams;

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nValuesSize =  nSize * sizeof(TValue);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			valueStreams.attach(stream.buffer() + stream.pos() + nKeySize, nValuesSize);
			stream.seek(stream.pos() + nKeySize + nValuesSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				KeyStreams.write(vecKeys[i]);
				valueStreams.write(vecValues[i]);
			}
			
			return true;
		}
		virtual bool insert(uint32 nIndex, const TKey& key, const TValue& value)
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
			return (sizeof(TKey) + sizeof(TValue)) *  m_nSize +  sizeof(uint32);
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
			return  (sizeof(TKey) + sizeof(TValue)) *  m_nSize ;
		}
		size_t tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) ;
		}
	private:
		size_t m_nSize;
	};
}

#endif