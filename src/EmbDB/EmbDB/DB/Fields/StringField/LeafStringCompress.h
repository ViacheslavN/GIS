#ifndef _EMBEDDED_DATABASE_STRING_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "StringCompressorParams.h"
#include "StringVal.h"
#include "utils/streams/ReadStreamPage.h"
#include "utils/streams/WriteStreamPage.h"
namespace embDB
{


	template<class _TKey = int64, 
	 class _Transaction = IDBTransaction>
	class BPStringLeafNodeCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<sStringVal> TLeafValueMemSet;

		typedef StringFieldCompressorParams TLeafCompressorParams;
		typedef _Transaction Transaction;



		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams();
			return pInnerComp;
		}

		BPStringLeafNodeCompressor(uint32 nPageSize, Transaction *pTransaction, CommonLib::alloc_t *pAlloc, 
			TLeafCompressorParams *pParams,
			TLeafKeyMemSet *pKeyMemset, TLeafValueMemSet *pValueMemSet) : m_nCount(0), 
			m_pTransaction(pTransaction),
			m_pAlloc(pAlloc), m_pLeafCompParams(pParams),		m_nStringDataSize(0), m_pValueMemset(pValueMemSet),
			m_nPageSize(nPageSize)
		{

			assert(m_pTransaction);
			assert(m_pAlloc);
			assert(m_pLeafCompParams);
			assert(m_pValueMemset);


			m_nMaxPageLen = m_pLeafCompParams->GetMaxPageStringSize();

		}


		virtual ~BPStringLeafNodeCompressor()
		{
			Clear();
			
		}
		void Clear()
		{
			if(!m_pValueMemset)
				return;

			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sStringVal& val = (*m_pValueMemset)[i];
				if(val.m_pBuf)
					m_pAlloc->free(val.m_pBuf);
			}
		}
		virtual bool Load(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{
		

			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;


			eStringCoding sCode = m_pLeafCompParams->GetStringCoding();
			uint32 nMaxPageLen = m_pLeafCompParams->GetMaxPageStringSize();

			m_nStringDataSize = 0;
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

				sStringVal sString;
				sString.m_bChange = false;

				/*sString.m_nLen  = strlen((const char*)ValueStream.buffer() + ValueStream.pos()) + 1;
				m_nStringDataSize += sString.m_nLen;

				sString.m_pBuf = (byte*)m_pAlloc->alloc(sString.m_nLen);
				memcpy(sString.m_pBuf, ValueStream.buffer() + ValueStream.pos(), sString.m_nLen);*/

				sString.m_nLen = ValueStream.readIntu32();
	 
				m_nStringDataSize += sizeof(uint32);
				if(sString.m_nLen < nMaxPageLen)
				{
					sString.m_pBuf = (byte*)m_pAlloc->alloc(sString.m_nLen);
					memcpy(sString.m_pBuf, ValueStream.buffer() + ValueStream.pos(), sString.m_nLen);
					ValueStream.seek(sString.m_nLen, CommonLib::soFromCurrent);

					m_nStringDataSize += (sizeof(uint32) + sString.m_nLen );
				}
				else
				{
					sString.m_nPage = ValueStream.readIntu64();
					sString.m_nPos = ValueStream.readIntu32();
					m_nStringDataSize += (sizeof(uint32) + sizeof(uint64));
					 
				}
				
				valueSet.push_back(sString);
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
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, m_nStringDataSize);
			stream.seek(stream.pos() + nKeySize + m_nStringDataSize, CommonLib::soFromBegin);		

			uint32 nMaxPageLen = m_pLeafCompParams->GetMaxPageStringSize();


			CommonLib::CBlob bufForUff8;
		 	for (uint32 i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);

				sStringVal& sString = valueSet[i];


				ValueStream.write(sString.m_nLen);
				if(sString.m_nLen < nMaxPageLen)
				{
					
					ValueStream.write(sString.m_pBuf, sString.m_nLen);
				}
				else
				{
				
					if(sString.m_bChange || sString.m_nPage == -1)
					{
						WriteStreamPagePtr pWriteStream;
						if(sString.m_nOldLen >= sString.m_nLen && sString.m_nPage != -1)
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction, sString.m_nPage, sString.m_nPos);
						else
						{
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction);
							sString.m_nPage = -1;
						}
						 
						if(sString.m_nPage == -1)
						{
							sString.m_nPage = pWriteStream->GetPage();
							sString.m_nPos = pWriteStream->GetPos();
						}

						pWriteStream->write(sString.m_pBuf, sString.m_nLen);
					}

					ValueStream.write(sString.m_nPage);
					ValueStream.write((uint32)sString.m_nPos);
				}

				

			}
			return true;
		}

		virtual bool insert(int nIndex, TKey key, /*const CommonLib::CString&*/ const sStringVal& sStr)
		{
			m_nCount++;
			m_nStringDataSize += GetStingSize(sStr);
				
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
			m_nStringDataSize = 0;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valueSet[i]);
			}
		

			return true;
		}
		virtual bool remove(int nIndex, TKey key, const sStringVal& sStr)
		{
			m_nCount--;
			m_nStringDataSize -=GetStingSize(sStr);
			return true;
		}
		virtual bool update(int nIndex, TKey key, sStringVal& sStr)
		{
			assert(m_pValueMemset);
			int oldSize = GetStingSize((*m_pValueMemset)[nIndex]);
			int newSize = GetStingSize(sStr); 
			sStr.m_nOldLen = oldSize;
			sStr.m_bChange = true;
			m_nStringDataSize += (newSize - oldSize);
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) *  m_nCount )  + sizeof(uint32) + m_nStringDataSize;
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
			return (sizeof(TKey) *  m_nCount ) + m_nStringDataSize;
		}
		void clear()
		{
			m_nCount = 0;
		}
		uint32 tupleSize() const
		{
			return  (m_nMaxPageLen  + sizeof(uint32) + sizeof(TKey));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPStringLeafNodeCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSplitStringDataSize = 0;
			for (uint32 i  = nBegin; i < nEnd; ++i)
			{
				nSplitStringDataSize += GetStingSize((*m_pValueMemset)[i]);
			}
			


			uint32 nCount = nEnd - nBegin;


			pCompressor->m_nCount = m_nCount - nCount;
			pCompressor->m_nStringDataSize = m_nStringDataSize - nSplitStringDataSize;

			m_nCount = nCount;
			m_nStringDataSize = nSplitStringDataSize; 

		}

		bool IsHaveUnion(BPStringLeafNodeCompressor *pCompressor) const
		{

			return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPStringLeafNodeCompressor *pCompressor) const
		{
			 
			return false;
		}
		bool isHalfEmpty() const
		{ 
			return rowSize()  < (m_nPageSize - headSize())/2;
		}
	private:
		int GetStingSize(const sStringVal& sStr) const 
		{
			
			uint32 nSize = sizeof(uint32);
			if(sStr.m_nLen < m_nMaxPageLen)
			{
				nSize += (sStr.m_nLen);
			}
			else
			{
				nSize += sizeof(uint32);
				nSize += sizeof(uint64);
			}
			return nSize;

		}
	
	private:

		uint32 m_nStringDataSize;
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