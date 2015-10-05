#ifndef _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "StringCompressorParams.h"
#include "CommonLibrary/PodVector.h"
#include "StringVal.h"
namespace embDB
{


	template<class _TKey = int64>
	class BPStringLeafNodeSimpleCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<sStringVal> TLeafValueMemSet;

		typedef StringFieldCompressorParams TLeafCompressorParams;



		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams(nPage);
			pInnerComp->read(pTran);
			return pInnerComp;
		}

		BPStringLeafNodeSimpleCompressor(CommonLib::alloc_t *pAlloc = 0, 
			TLeafCompressorParams *pParams = 0,
			TLeafKeyMemSet *pKeyMemset= NULL, TLeafValueMemSet *pValueMemSet = NULL) : m_nSize(0), m_pAlloc(pAlloc), m_pLeafCompParams(pParams),
			m_nStringDataSize(0), m_pValueMemset(pValueMemSet) 
		{

			assert(m_pAlloc);
			assert(m_pLeafCompParams);
			assert(m_pValueMemset);

		}
		virtual ~BPStringLeafNodeSimpleCompressor()
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


			eStringCoding sCode = m_pLeafCompParams->GetStringCoding();

			m_nStringDataSize = 0;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

		
			keySet.reserve(m_nSize);
			valueSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(TKey);
 
			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, stream.size() -  stream.pos() -  nKeySize);

			TKey nKey;

			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStream.read(nKey);
				//CommonLib::CString sString;
	 


				sStringVal sString;

			/*	if(sCode == scASCII)
				{
					nStrSize = sString.loadFromASCII((const char*)ValueStream.buffer() + ValueStream.pos());
				}
				else if(sCode == scUTF8)
				{
					nStrSize = sString.loadFromUTF8((const char*)ValueStream.buffer()+ ValueStream.pos()) + 1;
				}*/

			/*	if(sCode == scASCII)
				{
					nStrSize = strlen((const char*)ValueStream.buffer() + ValueStream.pos());//sString.loadFromASCII((const char*)ValueStream.buffer() + ValueStream.pos());
				}
				else if(sCode == scUTF8)
				{
					nStrSize = sString.loadFromUTF8((const char*)ValueStream.buffer()+ ValueStream.pos()) + 1;
				}*/

				sString.m_nLen  = strlen((const char*)ValueStream.buffer() + ValueStream.pos()) + 1;
				m_nStringDataSize += sString.m_nLen;

				sString.m_pBuf = (byte*)m_pAlloc->alloc(sString.m_nLen);
				memcpy(sString.m_pBuf, ValueStream.buffer() + ValueStream.pos(), sString.m_nLen);
				

				ValueStream.seek(sString.m_nLen, CommonLib::soFromCurrent);

				//m_vecStringSize.push_back(sString.m_nLen);
				valueSet.push_back(sString);
				keySet.push_back(nKey);
			}
			assert(KeyStream.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStream;
			CommonLib::FxMemoryWriteStream ValueStream;

			eStringCoding sCode = m_pLeafCompParams->GetStringCoding();
			uint32 nKeySize =  m_nSize * sizeof(TKey);
	

			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, m_nStringDataSize);
			stream.seek(stream.pos() + nKeySize + m_nStringDataSize, CommonLib::soFromBegin);		

			CommonLib::CBlob bufForUff8;
		 	for (size_t i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);
				ValueStream.write(valueSet[i].m_pBuf, valueSet[i].m_nLen);
				/*if(sCode == scASCII)
				{
					ValueStream.write(valueSet[i].cstr());
				}
				else if(sCode == scUTF8)
				{
					int utf8Len = valueSet[i].calcUTF8Length() + 1;
					bufForUff8.resize(utf8Len);

					valueSet[i].exportToUTF8((char*)bufForUff8.buffer(), utf8Len);
					ValueStream.write((const byte*)bufForUff8.buffer(), utf8Len);
				}*/

			}
			return true;
		}

		virtual bool insert(int nIndex, TKey key, /*const CommonLib::CString&*/ const sStringVal& sStr)
		{
			m_nSize++;
			uint32 nStrSize = sStr.m_nLen;//GetStingSize(sStr);
			//m_vecStringSize.insert(nIndex, nStrSize);
			m_nStringDataSize += nStrSize;

	
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

		
			m_nSize = 0;
			m_nStringDataSize = 0;
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valueSet[i]);
			}
		

			return true;
		}
		virtual bool remove(int nIndex, TKey key, const sStringVal& sStr)
		{
			m_nSize--;
			uint32 nStrSize = sStr.m_nLen;//GetStingSize(sStr);
			m_nStringDataSize -= nStrSize;
			//m_vecStringSize.remove(nIndex);
			return true;
		}
		virtual bool update(int nIndex, TKey key, const sStringVal& sStr)
		{
			assert(m_pValueMemset);
			int oldSize = (*m_pValueMemset)[nIndex].m_nLen;//GetStingSize((*m_pValueMemset)[nIndex]);
			int newSize =sStr.m_nLen; //GetStingSize(sStr);
			m_nStringDataSize += (newSize - oldSize);
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) *  m_nSize )  + sizeof(uint32) + m_nStringDataSize;
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
			return (sizeof(TKey) *  m_nSize ) + m_nStringDataSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (m_pLeafCompParams->GetStringLen() + sizeof(TKey));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPStringLeafNodeSimpleCompressor *pCompressor)
		{
			uint32 nSplitStringDataSize = 0;
			for (size_t i  = nBegin; i < nEnd; ++i)
			{
				nSplitStringDataSize += (*m_pValueMemset)[i].m_nLen;
			}
			


			uint32 nCount = nEnd - nBegin;


			pCompressor->m_nSize = m_nSize - nCount;
			pCompressor->m_nStringDataSize = m_nStringDataSize - nSplitStringDataSize;

			m_nSize = nCount;
			m_nStringDataSize = nSplitStringDataSize; 

	
			//pCompressor->m_vecStringSize.copy(m_vecStringSize, 0, nEnd,  m_vecStringSize.size());
			//m_vecStringSize.resize(nCount);

		}
	private:
		int GetStingSize(const CommonLib::CString& sStr) const 
		{
			switch(m_pLeafCompParams->GetStringCoding())
			{
			case scASCII:
				return sStr.length();
				break;
			case  scUTF8:
				return sStr.calcUTF8Length() + 1;
				break;
			default:
				assert(false);
				return 0;
				break;
			}

		}
	
	private:
	//	CommonLib::TPodVector<uint32> m_vecStringSize;
		size_t m_nStringDataSize;
		size_t m_nSize;
 
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams *m_pLeafCompParams;
		TLeafValueMemSet *m_pValueMemset;
	};
}

#endif