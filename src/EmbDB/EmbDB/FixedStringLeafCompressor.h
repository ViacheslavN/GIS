#ifndef _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "StringCompressorParams.h"
#include "CommonLibrary/PodVector.h"
#include "StringVal.h"
#include "FixedStringCompressor.h"
#include "OIDCompress.h"
namespace embDB
{


	template<class _TKey = int64,	class _Transaction = IDBTransaction>
	class BPFixedStringLeafNodeCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVector<sFixedStringVal> TLeafValueMemSet;
		typedef StringFieldCompressorParams TLeafCompressorParams;

		typedef _Transaction Transaction;

 
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams();
			return pInnerComp;
		}

		BPFixedStringLeafNodeCompressor(uint32 nPageSize, Transaction *pTran , CommonLib::alloc_t *pAlloc,	TLeafCompressorParams* pParams,
			TLeafKeyMemSet *pKeyMemset, TLeafValueMemSet *pValueMemSet) : m_nCount(0), m_pAlloc(pAlloc), m_pLeafCompParams(pParams),
			m_nStringDataSize(0), m_pValueMemset(pValueMemSet), m_pKeyMemSet(pKeyMemset), m_nPageSize(nPageSize), m_Compress(pAlloc, pParams), m_OIDCompressor(pAlloc, pParams)
		{

			assert(m_pAlloc);
			assert(m_pLeafCompParams);
			assert(m_pValueMemset);

			m_Compress.init(m_pValueMemset);

		}


		virtual ~BPFixedStringLeafNodeCompressor()
		{
			Clear();

		}
		void Clear()
		{

			m_Compress.Clear();
			return;

			if(!m_pValueMemset)
				return;

			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sFixedStringVal& val = (*m_pValueMemset)[i];
				m_pAlloc->free(val.m_pBuf);
			}
		}
		virtual bool Load(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{


			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;
			m_nCount = stream.readIntu32();
			if(!m_nCount)
				return true;


			keySet.reserve(m_nCount);
			valueSet.reserve(m_nCount);

			uint32 nKeySize =  stream.readIntu32();
			m_nStringDataSize =  stream.readIntu32();
			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, m_nStringDataSize);

			/*TKey nKey;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStream.read(nKey);

				sFixedStringVal sString;

				sString.m_nLen  = strlen((const char*)ValueStream.buffer() + ValueStream.pos()) + 1;
				m_nStringDataSize += sString.m_nLen;

				sString.m_pBuf = (byte*)m_pAlloc->alloc(sString.m_nLen);
				memcpy(sString.m_pBuf, ValueStream.buffer() + ValueStream.pos(), sString.m_nLen);


				ValueStream.seek(sString.m_nLen, CommonLib::soFromCurrent);
				valueSet.push_back(sString);

				keySet.push_back(nKey);
			}*/
			m_OIDCompressor.decompress(m_nCount, keySet, &KeyStream);
			m_Compress.decompress(m_nCount, valueSet, &ValueStream);

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

			eStringCoding sCode = m_pLeafCompParams->GetStringCoding();
			uint32 nKeySize =  m_OIDCompressor.GetComressSize();//m_nCount * sizeof(TKey);
			//uint32 nKeySize =  m_nCount * sizeof(TKey);
			m_nStringDataSize = m_Compress.GetComressSize();

			stream.write(nKeySize);
			stream.write(m_nStringDataSize);
			

			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, m_nStringDataSize);
			stream.seek(stream.pos() + nKeySize + m_nStringDataSize, CommonLib::soFromBegin);		

			
			/*for (uint32 i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);
				//ValueStream.write(valueSet[i].m_pBuf, valueSet[i].m_nLen);

			}*/
			m_OIDCompressor.compress(keySet, &KeyStream);
			m_Compress.compress(valueSet, &ValueStream);
			return true;
		}

		virtual bool insert(int nIndex, TKey key, /*const CommonLib::CString&*/ const sFixedStringVal& sStr)
		{
			m_nCount++;
			uint32 nStrSize = sStr.m_nLen;
			m_nStringDataSize += nStrSize;


			m_Compress.AddSymbol(m_nCount, nIndex, sStr, *m_pValueMemset);
			m_OIDCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			return true;
		}
		virtual bool add(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valuekSet)
		{	
			uint32 nOff = m_nCount;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], valuekSet[i]);
			}

			return true;
		}
		virtual bool recalc(const TLeafKeyMemSet& keySet, const TLeafValueMemSet& valueSet)
		{


			m_nCount = 0;
			m_nStringDataSize = 0;
			m_Compress.clear();
			m_OIDCompressor.clear();

			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				m_nCount++;
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				m_Compress.AddSymbol(m_nCount, i, valueSet[i], *m_pValueMemset);


				m_nStringDataSize += valueSet[i].m_nLen;
			}

			/*for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valueSet[i]);
			}*/


			return true;
		}
		virtual bool remove(int nIndex, TKey key, const sFixedStringVal& sStr)
		{
			m_nCount--;
			uint32 nStrSize = sStr.m_nLen;
			m_nStringDataSize -= nStrSize;

			m_Compress.RemoveSymbol(m_nCount, nIndex, sStr, *m_pValueMemset);
			m_OIDCompressor.RemoveSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			return true;
		}
		virtual bool update(int nIndex, TKey key, const sFixedStringVal& sStr)
		{
			assert(m_pValueMemset);
			int oldSize = (*m_pValueMemset)[nIndex].m_nLen;
			int newSize =sStr.m_nLen; 
			m_nStringDataSize += (newSize - oldSize);

			m_OIDCompressor.RemoveSymbol(m_nCount, nIndex, (*m_pKeyMemSet)[nIndex], *m_pKeyMemSet);
			m_OIDCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);

			m_Compress.RemoveSymbol(m_nCount, nIndex, (*m_pValueMemset)[nIndex], *m_pValueMemset);
			m_Compress.AddSymbol(m_nCount, nIndex, sStr, *m_pValueMemset);
			return true;
		}
		virtual uint32 size() const
		{
			//return (sizeof(TKey) *  m_nCount )  + sizeof(uint32) +  m_nStringDataSize*/;

			return headSize() + rowSize();
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
			return  3 * sizeof(uint32);
		}
		uint32 rowSize() const
		{
			//return (sizeof(TKey) *  m_nCount ) + /*m_nStringDataSize*/ m_Compress.GetComressSize();
			return m_Compress.GetComressSize() + m_OIDCompressor.GetComressSize();//(sizeof(TKey) *  m_nCount ) + m_nStringDataSize;
		}
		void clear()
		{
			m_nCount = 0;
		}
		uint32 tupleSize() const
		{
			return  (m_pLeafCompParams->GetStringLen() + sizeof(TKey));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPFixedStringLeafNodeCompressor *pCompressor)
		{
			uint32 nSplitStringDataSize = 0;
			for (uint32 i  = nBegin; i < nEnd; ++i)
			{
				nSplitStringDataSize += (*m_pValueMemset)[i].m_nLen;
			}



			uint32 nCount = nEnd - nBegin;


			pCompressor->m_nCount = m_nCount - nCount;
			pCompressor->m_nStringDataSize = m_nStringDataSize - nSplitStringDataSize;

			m_nCount = nCount;
			m_nStringDataSize = nSplitStringDataSize; 

		}

		bool IsHaveUnion(BPFixedStringLeafNodeCompressor *pCompressor) const
		{

			return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPFixedStringLeafNodeCompressor *pCompressor) const
		{
			return rowSize() < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{ 
			return rowSize()  < (m_nPageSize - headSize())/2;
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

		uint32 m_nStringDataSize;
		uint32 m_nCount;

		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams *m_pLeafCompParams;
		TLeafValueMemSet *m_pValueMemset;
		TLeafKeyMemSet* m_pKeyMemSet;

		uint32 m_nPageSize;

		TFixedCompress m_Compress;
		OIDCompressor  m_OIDCompressor;
	};
}

#endif