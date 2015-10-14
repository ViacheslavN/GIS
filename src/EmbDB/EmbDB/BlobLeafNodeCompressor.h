#ifndef _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "BlobCompressorParams.h"
#include "CommonLibrary/PodVector.h"
#include "BlobVal.h"
#include "WriteStreamPage.h"
#include "ReadStreamPage.h"
namespace embDB
{


	template<class _TKey = int64>
	class BlobLeafNodeCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<sBlobVal> TLeafValueMemSet;

		typedef BlobFieldCompressorParams TLeafCompressorParams;



		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams(nPage);
			pInnerComp->read(pTran);
			return pInnerComp;
		}

		BlobLeafNodeCompressor(CommonLib::alloc_t *pAlloc = 0, 
			TLeafCompressorParams *pParams = 0,
			TLeafKeyMemSet *pKeyMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nCount(0), m_pAlloc(pAlloc), m_pLeafCompParams(pParams),
			m_nBlobDataSize(0), m_pValueMemset(pValueMemSet) 
		{

			assert(m_pAlloc);
			assert(m_pLeafCompParams);
			assert(m_pValueMemset);

		}


		virtual ~BlobLeafNodeCompressor()
		{
			Clear();
		}
		void Clear()
		{
			if(!m_pValueMemset)
				return;

			for (size_t i = 0; i < m_pValueMemset->size(); ++i )
			{
				sStringVal& val = (*m_pValueMemset)[i];
				m_pAlloc->free(val.m_pBuf);
			}
		}
		virtual bool Load(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{
		

			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;


			uint32 nMaxBlobSize  = m_pLeafCompParams->GetMaxPageBlobSize();

			m_nBlobDataSize = 0;
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

		
			keySet.reserve(m_nCount);
			valueSet.reserve(m_nCount);

			uint32 nKeySize =  m_nCount * sizeof(TKey);
 
			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, stream.size() -  stream.pos() -  nKeySize);

			TKey nKey;

			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStream.read(nKey);
 


				sBlobVal sBlobValue;
				sBlobValue.m_nSize = ValueStream.readIntu32();
				if(sBlobValue.m_nSize < nMaxBlobSize)
				{
					sBlobValue.m_pBuf = m_pAlloc->alloc(sBlobValue.m_nSize);
					ValueStream.read(sBlobValue.m_pBuf, sBlobValue.m_nSize);
					m_nBlobDataSize += sBlobValue.m_nSize + sizeof(uint32) ;
				}
				else
				{
					sBlobValue.m_nPage = ValueStream.readInt64();
					sBlobValue.m_nBeginPos = ValueStream.readintu16();

					m_nBlobDataSize += (sizeof(int64) + sizeof(uint16) + sizeof(uint32));
				}

			 
				valueSet.push_back(sBlobValue);
				keySet.push_back(nKey);
			}
			assert(KeyStream.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nCount= (uint32)keySet.size();
			assert(m_nCount == m_nCount);
			stream.write(m_nCount);
			if(!m_nCount)
				return true;

			CommonLib::FxMemoryWriteStream KeyStream;
			CommonLib::FxMemoryWriteStream ValueStream;

			uint32 nMaxBlobSize  = m_pLeafCompParams->GetMaxPageBlobSize();
			uint32 nKeySize =  m_nSize * sizeof(TKey);
	

			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, m_nBlobDataSize);
			stream.seek(stream.pos() + nKeySize + m_nBlobDataSize, CommonLib::soFromBegin);		

			CommonLib::CBlob bufForUff8;
		 	for (size_t i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);
				const sBlobVal&  sBlobValue  = valueSet[i];
				ValueStream.write(sBlobValue.m_nSize);
				if(sBlobValue.m_nSize < nMaxBlobSize)
				{					
					ValueStream.write(sBlobValue.m_pBuf, sBlobValue.m_nSize);
				 }
				else
				{
					ValueStream.write(sBlobValue.m_nPage);
					ValueStream.write(sBlobValue.m_nBeginPos);
			 
				}
		 

			}
			return true;
		}

		virtual bool insert(int nIndex, TKey key,  const sBlobVal& blob)
		{
			m_nCount++;
			m_nBlobDataSize += GetBlobSize(blob);

	
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valuekSet)
		{
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valuekSet[i]);
			}

			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valueSet)
		{

		
			m_nCount = 0;
			m_nBlobDataSize = 0;
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valueSet[i]);
			}
		

			return true;
		}
		virtual bool remove(int nIndex, TKey key, const sBlobVal& blob)
		{
			m_nCount--;
			m_nBlobDataSize -= GetBlobSize(blob);
			
 			return true;
		}
		virtual bool update(int nIndex, TKey key, const sBlobVal& blob)
		{
			assert(m_pValueMemset);
			const sBlobVal& oldBlob = (*m_pValueMemset)[nIndex]; 
			m_nBlobDataSize -= GetBlobSize(oldBlob);
			m_nBlobDataSize += GetBlobSize(blob);
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) *  m_nSize )  + sizeof(uint32) + m_nBlobDataSize;
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize()
		{
			return (sizeof(TKey) *  m_nSize ) + m_nBlobDataSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (m_pLeafCompParams->GetMaxPageBlobSize() + sizeof(TKey));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPFixedStringLeafNodeCompressor *pCompressor)
		{
			uint32 nSplitBlobDataSize = 0;
			for (size_t i  = nBegin; i < nEnd; ++i)
			{
				nSplitBlobDataSize += GetBlobSize((*m_pValueMemset)[i]);
			}
			

			uint32 nCount = nEnd - nBegin;
			pCompressor->m_nSize = m_nSize - nCount;
			pCompressor->m_nBlobDataSize = m_nBlobDataSize - nSplitBlobDataSize;

			m_nSize = nCount;
			m_nBlobDataSize = nSplitBlobDataSize; 
  

		}
	private:
			uint32 GetBlobSize(const sBlobVal& blob)
			{
				uint32 nMaxBlobSize  = m_pLeafCompParams->GetMaxPageBlobSize();
				if(blob.m_nSize < nMaxBlobSize)
				{
					return (blob.m_nSize + sizeof(uint32)) ;
				}
				else
				{
					return (sizeof(int64) + sizeof(uint16) + sizeof(uint32));
				}
			}
	private:
 
		size_t m_nBlobDataSize;
		size_t m_nCount;
 
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams *m_pLeafCompParams;
		TLeafValueMemSet *m_pValueMemset;
	};
}

#endif