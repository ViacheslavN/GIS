#ifndef _EMBEDDED_DATABASE_FIELD_BASE_LEAF_NODE_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_BASE_LEAF_NODE_COMPRESS_H_


namespace embDB
{

	template<class _TKey, class _TValue, class _Transaction = IDBTransaction, class _TKeyCommpressor = TEmptyValueCompress<_TKey>, class _TValueCompressor  = TEmptyValueCompress<_TValue>
	,class _TLeafCompressorParams = CompressorParamsBaseImp>
	class TBaseLeafNodeComp
	{
		public:
			typedef _TKey TKey;
			typedef _TValue TValue;
			typedef _TKeyCommpressor TKeyCommpressor;
			typedef _TValueCompressor TValueCompressor;
			typedef _Transaction Transaction;


			typedef  TBPVector<TKey> TKeyMemSet;
			typedef  TBPVector<TValue> TValueMemSet;
			typedef _TLeafCompressorParams TLeafCompressorParams;


			TBaseLeafNodeComp(uint32 nPageSize, Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
					TKeyMemSet *pOIDMemset= NULL, TValueMemSet *pValueMemSet = NULL) : m_nCount(0), m_pKeyMemSet(pOIDMemset), m_pValueMemSet(pValueMemSet),
				m_nPageSize(nPageSize), m_KeyCompressor(pAlloc, nPageSize, pParams), m_ValueCompressor(pAlloc, nPageSize, pParams)
			{

				assert(m_pKeyMemSet);
				assert(m_pValueMemSet);

			}

		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}

		virtual ~TBaseLeafNodeComp(){}
		virtual bool Load(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream& stream)
		{

			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;

			m_nCount = stream.readIntu32();
			if(!m_nCount)
				return true;



			vecKeys.reserve(m_nCount);
			vecValues.reserve(m_nCount);

			uint32 nKeySize = stream.readIntu32();
			uint32 nValueSize = stream.readIntu32();


	
			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			m_KeyCompressor.decompress(m_nCount, vecKeys, &KeyStream);
			m_ValueCompressor.decompress(m_nCount, vecValues, &ValueStream);

			return true;
		}
		virtual bool Write(TKeyMemSet& vecKeys, TValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecKeys.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream ValueStream;
			CommonLib::FxMemoryWriteStream KeyStream;

			uint32 nKeySize = m_KeyCompressor.GetComressSize();
			uint32 nValueSize = m_ValueCompressor.GetComressSize();


			stream.write(nKeySize);
			stream.write(nValueSize);

			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValueSize);

			stream.seek(stream.pos() + nKeySize + nValueSize, CommonLib::soFromBegin);			 


			

			m_KeyCompressor.compress(vecKeys, &KeyStream);
			m_ValueCompressor.compress(vecValues, &ValueStream);
			return true;
		}
		virtual bool insert(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_nCount++;			
			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_ValueCompressor.AddSymbol(m_nCount, nIndex, value, *m_pValueMemSet);

			return true;
		}
		virtual bool add(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
		{

			
		
			for (uint32 i = 0, sz = vecKeys.size(); i < sz; ++i)
			{
				//insert(i + nOff, vecOIDs[i], vecValues[i]);
				m_KeyCompressor.AddSymbol(m_nCount, m_nCount + i, vecKeys[0], (*m_pKeyMemSet));
				m_ValueCompressor.AddSymbol(m_nCount, m_nCount + i, vecValues[i], (*m_pValueMemSet));

				m_nCount++;
			}

			return true;
		}
		virtual bool recalc(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
		{
			m_nCount = vecKeys.size();
			m_KeyCompressor.clear();
			m_ValueCompressor.clear();
			for (uint32 i = 0, sz = vecKeys.size(); i < sz; 	++i)
			{

				m_KeyCompressor.AddSymbol(m_nCount, i, vecKeys[i], vecKeys); 
				m_ValueCompressor.AddSymbol(m_nCount, i, vecValues[i], vecValues);
			}
	
			return true;
		}
		virtual bool update(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_ValueCompressor.RemoveSymbol(m_nCount, nIndex, value, *m_pValueMemSet);
			m_ValueCompressor.AddSymbol(m_nCount, nIndex, value, *m_pValueMemSet);

			return true;
		}
		virtual bool remove(uint32 nIndex, const TKey& key, const TValue& value)
		{
			m_nCount--;
			m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_ValueCompressor.RemoveSymbol(m_nCount, nIndex, value, *m_pValueMemSet);
			
			return true;
		}
		virtual uint32 size() const
		{
			return rowSize() +  headSize();
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
			return  sizeof(uint32) + sizeof(uint32)+ sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return  m_ValueCompressor.GetComressSize() + m_KeyCompressor.GetComressSize();
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue)) ;
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, TBaseLeafNodeComp *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
			if(bRecalcSrc)
				recalc();
			if(bRecalcDst)
				pCompressor->recalc();
 
		}

		virtual void recalc()
		{
			m_nCount = m_pKeyMemSet->size();
			m_KeyCompressor.clear();
			m_ValueCompressor.clear();
			for (uint32 i = 0, sz = m_pKeyMemSet->size(); i < sz; 	++i)
			{
				//insert(i, keySet[i], linkSet[i]);
				if(i != 0)
					m_KeyCompressor.AddDiffSymbol((*m_pKeyMemSet)[i] - (*m_pKeyMemSet)[i - 1]); 

				m_ValueCompressor.AddSymbol(m_nCount, i, (*m_pValueMemSet)[i], (*m_pValueMemSet));
			}


		}

		bool IsHaveUnion(TBaseLeafNodeComp *pCompressor) const
		{
			/*uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TValue));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TOID) + sizeof(TValue));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());*/


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
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
		void clear()
		{
			m_nCount = 0;
			m_KeyCompressor.clear();
			m_ValueCompressor.clear();
		}
	protected:

		uint32 m_nCount;
		TKeyCommpressor m_KeyCompressor;
		TValueCompressor m_ValueCompressor;
		TKeyMemSet* m_pKeyMemSet;
		TValueMemSet* m_pValueMemSet;

		uint32 m_nPageSize;

	};
}

#endif