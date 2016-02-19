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

		TMapLeafNodeOIDComp(uint32 nPageSize, _Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
			TOIDMemSet *pOIDMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nCount(0), m_pOIDMemSet(pOIDMemset),
			m_nPageSize(nPageSize)
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

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;



			vecOIDs.reserve(m_nCount);
			vecValues.reserve(m_nCount);

			uint32 nRowNoCompSize = m_nCount * (sizeof(TValue) + sizeof(TOID));
			uint32 nValueSize =  m_nCount * sizeof(TValue);


			uint32 nOIDSize =  0;

			bool bNoComp = nRowNoCompSize < (m_nPageSize - sizeof(uint32));


			if(bNoComp)
			{
				nOIDSize = m_nCount * sizeof(TOID);
			}
			else
			{
				nOIDSize = stream.readIntu32();
			}
	
			OIDStreams.attachBuffer(stream.buffer() + stream.pos(), nOIDSize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nOIDSize, nValueSize);

			if(bNoComp)
			{
				 
				TValue value;
				TOID oid;
				for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
				{

					OIDStreams.read(oid);
					ValueStreams.read(value);
				
					vecValues.push_back(value);
					vecOIDs.push_back(oid);

					if(nIndex != 0)
						m_OIDCompressor.AddDiffSymbol(vecOIDs[nIndex] - vecOIDs[nIndex - 1]);
				}
			}
			else
			{
				m_OIDCompressor.decompress(m_nCount, vecOIDs, &OIDStreams);
				TValue value;
				for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
				{

					ValueStreams.read(value);
					vecValues.push_back(value);
				}
			}
			
			return true;
		}
		virtual bool Write(TOIDMemSet& vecOIDs, TLeafValueMemSet& vecValues, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)vecOIDs.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream ValueStreams;
			CommonLib::FxMemoryWriteStream OidsStreams;

			uint32 nOidsSize = 0;
			uint32 nValuesSize =  nSize * sizeof(TValue);


			uint32 nRowNoCompSize = m_nCount * (sizeof(TValue) + sizeof(TOID));
			bool bNoComp = nRowNoCompSize < (m_nPageSize - sizeof(uint32));

			if(bNoComp)
			{

				nOidsSize = m_nCount * (sizeof(TOID));
			}
			else
			{
				nOidsSize = m_OIDCompressor.GetComressSize();
				stream.write(nOidsSize);

			}


			OidsStreams.attachBuffer(stream.buffer() + stream.pos(), nOidsSize);
			ValueStreams.attachBuffer(stream.buffer() + stream.pos() + nOidsSize, nValuesSize);

			stream.seek(stream.pos() + nOidsSize + nValuesSize, CommonLib::soFromBegin);			 


			

			if(bNoComp)
			{
				for(size_t i = 0, sz = vecValues.size(); i < sz; ++i)
				{
					ValueStreams.write(vecValues[i]);
					OidsStreams.write(vecOIDs[i]);

				}
			}
			else
			{
				m_OIDCompressor.compress(vecOIDs, &OidsStreams);
				for(size_t i = 0, sz = vecValues.size(); i < sz; ++i)
				{
					ValueStreams.write(vecValues[i]);

				}

			}

			return true;
		}
		virtual bool insert(uint32 nIndex, const TOID& oid, const TValue& value)
		{
			m_nCount++;			
			m_OIDCompressor.AddSymbol(m_nCount, nIndex, oid, *m_pOIDMemSet);
			return true;
		}
		virtual bool add(const TOIDMemSet& vecOIDs, const TLeafValueMemSet& vecValues)
		{
			//uint32 nOff = m_nCount;

			if(!vecOIDs.empty())
			{
				m_OIDCompressor.AddDiffSymbol(vecOIDs[0] - (*m_pOIDMemSet)[m_nCount - 1]);
				m_nCount++;
			}
			
			for (uint32 i = 1, sz = vecOIDs.size(); i < sz; 	++i)
			{
				//insert(i + nOff, vecOIDs[i], vecValues[i]);
				m_OIDCompressor.AddDiffSymbol(vecOIDs[i] - vecOIDs[i - 1]);
				m_nCount++;
			}
			return true;
		}
		virtual bool recalc(const TOIDMemSet& vecOIDs, const TLeafValueMemSet& vecValues)
		{
			m_nCount = vecOIDs.size();
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
			m_nCount--;
			m_OIDCompressor.RemoveSymbol(m_nCount, nIndex, nOID, *m_pOIDMemSet);
			
			
			return true;
		}
		virtual size_t size() const
		{
			return rowSize() +  headSize();
		}
		virtual bool isNeedSplit() const
		{


			uint32 nRowNoCompSize = m_nCount * (sizeof(TValue) + sizeof(TOID));
			if(nRowNoCompSize < (m_nPageSize - sizeof(uint32)))
				return false;


			return m_nPageSize < size();
		}
		virtual size_t count() const
		{
			return m_nCount;
		}
		size_t headSize() const
		{
			return  sizeof(uint32) + sizeof(uint32);
		}
		size_t rowSize() const
		{
			return  (sizeof(TValue) *  m_nCount) + m_OIDCompressor.GetComressSize();
		}
		size_t tupleSize() const
		{
			return  (sizeof(TOID) + sizeof(TValue)) ;
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, TMapLeafNodeOIDComp *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
			recalc();
			pCompressor->recalc();
 
		}

		void recalc()
		{
			m_nCount = m_pOIDMemSet->size();
			m_OIDCompressor.clear();
			for (size_t i = 0, sz = m_pOIDMemSet->size(); i < sz; 	++i)
			{
				//insert(i, keySet[i], linkSet[i]);
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol((*m_pOIDMemSet)[i] - (*m_pOIDMemSet)[i - 1]); 
			}
		}

		bool IsHaveUnion(TMapLeafNodeOIDComp *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TValue));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TOID) + sizeof(TValue));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(TMapLeafNodeOIDComp *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TValue));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	private:
		size_t m_nCount;
		OIDCompress m_OIDCompressor;
		TOIDMemSet* m_pOIDMemSet;
		 uint32 m_nPageSize;
	};
}

#endif