#ifndef _EMBEDDED_DATABASE_BLOB_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BLOB_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "BlobCompressorParams.h"
#include "BlobVal.h"
#include "ReadStreamPage.h"
#include "WriteStreamPage.h"
namespace embDB
{


	template<class _TKey = int64, 
	 class _Transaction = IDBTransaction,
	 class _TCompParams = BlobFieldCompressorParams
	>
	class BlobLeafNodeCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<sBlobVal> TLeafValueMemSet;

		typedef _TCompParams TLeafCompressorParams;
		typedef _Transaction Transaction;



		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams();
			return pInnerComp;
		}

		BlobLeafNodeCompressor(uint32 nPageSize,  Transaction *pTransaction, CommonLib::alloc_t *pAlloc, 
			TLeafCompressorParams *pParams,
			TLeafKeyMemSet *pKeyMemset, TLeafValueMemSet *pValueMemSet) : m_nCount(0), 
			m_pTransaction(pTransaction),
			m_pAlloc(pAlloc), m_pLeafCompParams(pParams),	m_nBlobDataSize(0), m_pValueMemset(pValueMemSet),
			m_nPageSize(nPageSize)
		{

			assert(m_pTransaction);
			assert(m_pAlloc);
			assert(m_pLeafCompParams);
			assert(m_pValueMemset);


			m_nMaxPageLen = m_pLeafCompParams->GetMaxPageBlobSize();

		}


		virtual ~BlobLeafNodeCompressor()
		{
			Clear();
			
		}
		void Clear()
		{
			if(!m_pValueMemset)
				return;

			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sBlobVal& val = (*m_pValueMemset)[i];
				if(val.m_pBuf)
					m_pAlloc->free(val.m_pBuf);
			}
			m_pValueMemset->clear();
		}
		virtual bool Load(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;	
 
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;
					
			keySet.reserve(m_nCount);
			valueSet.reserve(m_nCount);

			uint32 nKeySize =  m_nCount * sizeof(TKey);
 
			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, stream.size() -  stream.pos() -  nKeySize);

			TKey nKey;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStream.read(nKey);

				sBlobVal blob;
				blob.m_bChange = false;
							 
				blob.m_nSize = ValueStream.readIntu32();
	 			m_nBlobDataSize += sizeof(uint32);
				if(blob.m_nSize  < m_nMaxPageLen)
				{
					blob.m_pBuf = (byte*)m_pAlloc->alloc(blob.m_nSize);
					ValueStream.read(blob.m_pBuf, blob.m_nSize);
					m_nBlobDataSize += blob.m_nSize;
				}
				else
				{
					blob.m_nPage = ValueStream.readIntu64();
					blob.m_nBeginPos = ValueStream.readIntu32();
					m_nBlobDataSize += (sizeof(uint32) + sizeof(uint64));
					 
				}
				
				valueSet.push_back(blob);
				keySet.push_back(nKey);
			}
			assert(KeyStream.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStream;
			CommonLib::FxMemoryWriteStream ValueStream;

		 
			uint32 nKeySize =  m_nCount * sizeof(TKey);
	

			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, m_nBlobDataSize);
			stream.seek(stream.pos() + nKeySize + m_nBlobDataSize, CommonLib::soFromBegin);		
 


		 	for (uint32 i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);
				sBlobVal& blob = valueSet[i];


				ValueStream.write(blob.m_nSize);
				if(blob.m_nSize < m_nMaxPageLen)
				{					
					ValueStream.write(blob.m_pBuf, blob.m_nSize);
				}
				else
				{				
					if(blob.m_bChange || blob.m_nPage == -1)
					{
						WriteStreamPagePtr pWriteStream;
						if(blob.m_nOldSize >= blob.m_nSize && blob.m_nPage != -1)
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction, blob.m_nPage, blob.m_nBeginPos);
						else
						{
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction);
							blob.m_nPage = -1;
						}
						 
						if(blob.m_nPage == -1)
						{
							blob.m_nPage = pWriteStream->GetPage();
							blob.m_nBeginPos = pWriteStream->GetPos();
						}

						pWriteStream->write(blob.m_pBuf, blob.m_nSize);
					}

					ValueStream.write(blob.m_nPage);
					ValueStream.write((uint32)blob.m_nBeginPos);
				}

				

			}
			return true;
		}

		virtual bool insert(int nIndex, TKey key, /*const CommonLib::CString&*/ const sBlobVal& blob)
		{
			m_nCount++;
			m_nBlobDataSize += GetBlobSize(blob);
				
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valuekSet)
		{
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valuekSet[i]);
			}

			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valueSet)
		{

		
			m_nCount = 0;
			m_nBlobDataSize = 0;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
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
		virtual bool update(int nIndex, TKey key,  sBlobVal& blob)
		{
			assert(m_pValueMemset);
			int oldSize = GetBlobSize((*m_pValueMemset)[nIndex]);
			int newSize = GetBlobSize(blob); 
			blob.m_nOldSize = oldSize;
			blob.m_bChange = true;
			m_nBlobDataSize  += (newSize - oldSize);
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) *  m_nCount )  + sizeof(uint32) + m_nBlobDataSize;
		}
		virtual bool isNeedSplit() const
		{
			return m_nPageSize < size();
		}
		virtual uint32 count() const
		{
			return m_nCount;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return (sizeof(TKey) *  m_nCount ) + m_nBlobDataSize;
		}
		void clear()
		{
			m_nCount = 0;
		}
		uint32 tupleSize() const
		{
			return  (m_nMaxPageLen  + sizeof(uint32) + sizeof(TKey));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BlobLeafNodeCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSplitBlobDataSize = 0;
			for (uint32 i  = nBegin; i < nEnd; ++i)
			{
				nSplitBlobDataSize += GetBlobSize((*m_pValueMemset)[i]);
			}
			


			uint32 nCount = nEnd - nBegin;


			pCompressor->m_nCount = m_nCount - nCount;
			pCompressor->m_nBlobDataSize = m_nBlobDataSize - nSplitBlobDataSize;

			m_nCount = nCount;
			m_nBlobDataSize = nSplitBlobDataSize; 

		}

		bool IsHaveUnion(BlobLeafNodeCompressor *pCompressor) const
		{
			//uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TLink));
			//uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TOID) + sizeof(TLink));

			return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BlobLeafNodeCompressor *pCompressor) const
		{
			//uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TLink));
			return false;//nNoCompSize < (m_nPageSize - headSize());
		}

		bool isHalfEmpty() const
		{ 
			return rowSize()  < (m_nPageSize - headSize())/2;
		}
	private:
		int GetBlobSize(const sBlobVal& blob) const 
		{
			
			uint32 nSize = sizeof(uint32);
			if(blob.m_nSize < m_nMaxPageLen)
			{
				nSize += (blob.m_nSize);
			}
			else
			{
				nSize += sizeof(uint32);
				nSize += sizeof(uint64);
			}
			return nSize;

		}
		
	private:

		uint32 m_nBlobDataSize;
		uint32 m_nCount;
		uint32 m_nMaxPageLen;
 
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams *m_pLeafCompParams;
		TLeafValueMemSet *m_pValueMemset;
		Transaction		*m_pTransaction;
		uint32 m_nPageSize;
	};
}

#endif