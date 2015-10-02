#ifndef _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_STRING_LEAF_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "BPVectorNoPod.h"
#include "StringCompressorParams.h"
namespace embDB
{


	template<class _TKey = int64>
	class BPStringLeafNodeSimpleCompressor  
	{
	public:


		typedef _TKey TKey;

		typedef  TBPVector<TKey> TLeafKeyMemSet;
		typedef  TBPVectorNoPOD<CommonLib::CString> TLeafValueMemSet;

		typedef StringFieldCompressorParams TLeafCompressorParams;



		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			TLeafCompressorParams *pInnerComp = new TLeafCompressorParams(nPage);
			pInnerComp->read(pTran);
			return pInnerComp;
		}

		BPStringLeafNodeSimpleCompressor(CommonLib::alloc_t *pAlloc = 0, 
			TInnerCompressorParams *pParams = 0) : m_nSize(0), m_pAlloc(pAlloc), m_pLeafCompParams(pParams),
			m_nStringDataSize(0), m_pValueMemset(0)
		{

			assert(m_pLeafCompParams);
		}
		virtual ~BPStringLeafNodeSimpleCompressor(){}
		virtual bool Load(TLeafKeyMemSet& keySet, TLeafValueMemSet& valueSet, CommonLib::FxMemoryReadStream& stream)
		{

			assert(m_pValueMemset == 0);
			m_pValueMemset = &valueSet;

			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValueStream;


			eStringCoding sCode = pInnerComp->GetStringCoding();

			m_nStringDataSize = 0;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

		
			keySet.reserve(m_nSize);
			valueSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(TKey);
 
			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, stream.size() -  stream.pos() -  nKeySize);

			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStream.read(nlink);
				CommonLib::CString sString;

				if(sCode == scASCII)
				{
					nStrSize = sString.loadFromASCII((const char*)ValueStream.buffer());
				}
				else if(sCode == scUTF8)
				{
					nStrSize = sString.loadtFromUTF8((const char*)ValueStream.buffer());
				}

				m_nStringDataSize += nStrSize;

				ValueStream.seek(nStrSize, CommonLib::soFromCurrent)

				valueSet.push_back(sString);
				keySet.push_back(nlink);
			}
			assert(LinkStreams.pos() < stream.size());
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

			eStringCoding sCode = pInnerComp->GetStringCoding();
			uint32 nKeySize =  m_nSize * sizeof(TKey);
	

			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValueStream.attach(stream.buffer() + stream.pos() + nKeySize, m_nStringDataSize);
			stream.seek(stream.pos() + nKeySize + m_nStringDataSize, CommonLib::soFromBegin);		

			CommonLib::CBlob bufForUff8;
		 	for (size_t i = 0, sz = keySet.size(); i < sz; ++i )
			{
				KeyStream.write(keySet[i]);

				if(sCode == scASCII)
				{
					ValueStream.write(valueSet[i].cstr());
				}
				else if(sCode == scUTF8)
				{
					int utf8Len = valueSet[i].calcUTF8Length();
					bufForUff8.resize(utf8Len);

					valueSet[i].exportToUTF8(bufForUff8.buffer(), utf8Len);
					ValueStream.write((const byte*)bufForUff8.buffer(), utf8Len);
				}

			}
			return true;
		}

		virtual bool insert(int nIndex, TKey key, const CommonLib::CString& sStr)
		{
			m_nSize++;
			uint32 nStrSize = GetStingSize(sStr);
			m_nStringDataSize += nStrSize;

			assert(link!= 0);
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLeafValueMemSet& valuekSet)
		{
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valuekSet[i]);
			}

			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLeafValueMemSet& valuekSet)
		{
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], valuekSet[i]);
			}
		}
		virtual bool remove(int nIndex, TKey key, const CommonLib::CString& sStr)
		{
			m_nSize--;
			uint32 nStrSize = GetStingSize(sStr);
			m_nStringDataSize -= sStr;
			return true;
		}
		virtual bool update(int nIndex, TKey key, const CommonLib::CString& sStr)
		{
			assert(m_pValueMemset);
			int oldSize = GetStingSize((*m_pValueMemset)[nIndex]);
			int newSize = GetStingSize(sStr);
			m_nStringDataSize += (newSize - oldSize);
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) ) *  m_nSize + sizeof(uint32) + m_nStringDataSize;
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
			return (sizeof(TLink)) *  m_nSize + m_nStringDataSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (m_pLeafCompParams->GetStringLen() + sizeof(TLink));
		}
	private:
		int GetStingSize(const CString& sStr) const 
		{
			switch(m_pLeafCompParams->GetStringCoding())
			{
			case scASCII:
				return sStr.length();
				break;
			case  scUTF8:
				return sStr.calcUTF8Length();
				break;
			default:
				assert(false);
				return 0;
				break;
			}

		}
	private:
		size_t m_nStringDataSize;
		size_t m_nSize;
		CommonLib::alloc_t* m_pAlloc;
		TLeafCompressorParams *m_pLeafCompParams;
		TLeafValueMemSet *m_pValueMemset;
	};
}

#endif