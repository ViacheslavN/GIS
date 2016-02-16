#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_OID_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_MAP_OID_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
#include <set>
#include <vector>
#include <map>
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
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nOIDSize, nValueSize);

			m_OIDCompressor.decompress(m_nSize, vecOIDs, &OIDStreams);
			TValue value;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
	 
				ValueStreams.read(value);
				vecValues.push_back(value);
			}

			return true;
		}
		virtual bool Write(TOIDMemSet& vecOIDs, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecOIDs.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream ValueStreams;
			CommonLib::FxMemoryWriteStream OidsStreams;

			uint32 nOidsSize = m_OIDCompressor.GetComressSize();
			uint32 nValuesSize =  nSize * sizeof(TValue);


			stream.write(nOidsSize);

			OidsStreams.attachBuffer(stream.buffer() + stream.pos(), nOidsSize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nOidsSize, nValuesSize);
			stream.seek(stream.pos() + nOidsSize + nValuesSize, CommonLib::soFromBegin);			 


			m_OIDCompressor.compress(vecOIDs, &OidsStreams);
			for(size_t i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				ValueStreams.write(vecValues[i]);
			 
			}

			return true;
		}
		virtual bool insert(uint32 nIndex, const TOID& oid, const TValue& value)
		{
			m_nSize++;
			m_OIDCompressor.AddSymbol(m_nSize, nIndex, oid, *m_pOIDMemSet);
			return true;
		}
		virtual bool add(const TOIDMemSet& vecOIDs, const TLeafValueMemSet& vecValues)
		{
			uint32 nOff = m_nSize;
			for (uint32 i = 0, sz = vecOIDs.size(); i < sz; 	++i)
			{
				insert(i + nOff, vecOIDs[i], vecValues[i]);
			}
			return true;
		}
		virtual bool recalc(const TOIDMemSet& vecOIDs, const TLeafValueMemSet& vecValues)
		{
			m_nSize = vecOIDs.size();
			m_OIDCompressor.clear();
			for (size_t i = 0, sz = vecOIDs.size(); i < sz; 	++i)
			{
				//insert(i, keySet[i], linkSet[i]);
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol(vecOIDs[i] - vecOIDs[i - 1]); 
			}
			return true;
		}
		virtual bool update(uint32 nIndex, const TOID& key, const TValue& value)
		{
			return true;
		}
		virtual bool remove(uint32 nIndex, const TOID& nOID, const TValue& value)
		{
			m_nSize--;
			m_OIDCompressor.RemoveSymbol(m_nSize, nIndex, nOID, *m_pOIDMemSet);
			
			
			return true;
		}
		virtual size_t size() const
		{
			return rowSize() +  headSize();
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
			return  (sizeof(TValue) *  m_nSize) + m_OIDCompressor.GetComressSize();
		}
		size_t tupleSize() const
		{
			return  (sizeof(TOID) + sizeof(TValue)) ;
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, TMapLeafNodeOIDComp *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
			recalc();
			pCompressor->recalc();
 
		}

		void recalc()
		{
			m_nSize = m_pOIDMemSet->size();
			m_OIDCompressor.clear();
			for (size_t i = 0, sz = m_pOIDMemSet->size(); i < sz; 	++i)
			{
				//insert(i, keySet[i], linkSet[i]);
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol((*m_pOIDMemSet)[i] - (*m_pOIDMemSet)[i - 1]); 
			}
		}
	private:
		size_t m_nSize;
		OIDCompress m_OIDCompressor;
		TOIDMemSet* m_pOIDMemSet;
		 
	};
}

#endif