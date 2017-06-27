#pragma once

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "StringCompressorParams.h"
#include "utils/streams/ReadStreamPage.h"
#include "utils/streams/WriteStreamPage.h"

#include "../../BTreePlus/BaseNodeCompressor.h"
#include "StringVal.h"

namespace embDB
{


	class BPStringLeafNodeCompressor  
	{
	public:

		typedef STLAllocator<sStringVal> TValueAlloc;
		typedef std::vector<sStringVal, TValueAlloc> TValueMemSet;





		BPStringLeafNodeCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc, StringFieldCompressorParams *pParams) : m_nCount(0),
			m_nStringDataSize(0),m_nPageSize(nPageSize), m_StringCoding(scUTF8), m_pLeafCompParams(nullptr)
		{}


		virtual ~BPStringLeafNodeCompressor()
		{}


		template<typename _Transactions  >
		bool  init(StringFieldCompressorParams *pParams, _Transactions *pTran)
		{
			m_pLeafCompParams = pParams;
			assert(m_pLeafCompParams);
			m_nMaxPageLen = m_pLeafCompParams->GetMaxPageStringSize();
			m_StringCoding = m_pLeafCompParams->GetStringCoding();

			m_pTransaction = pTran;
			return true;
		}

		void AddSymbol(uint32 nSize, int nIndex, const sStringVal& string, const TValueMemSet& vecValues)
		{
			m_nCount++;
			if (string.m_nLen < m_pLeafCompParams->GetMaxPageStringSize())
			{
				m_nStringDataSize += (sizeof(uint32) + string.m_nLen);
			}
			else
			{
				m_nStringDataSize += (sizeof(uint32) + sizeof(uint64));
			}
		}
		void RemoveSymbol(uint32 nSize, int nIndex, const sStringVal& string, const TValueMemSet& vecValues)
		{
			m_nCount--;
			if (string.m_nLen < m_pLeafCompParams->GetMaxPageStringSize())
			{
				m_nStringDataSize -= (sizeof(uint32) + string.m_nLen);
			}
			else
			{
				m_nStringDataSize -= (sizeof(uint32) + sizeof(uint64));
			}
		}
		void UpdateSymbol(int nIndex, sStringVal& sNewStr, const sStringVal& sOldStr, const TValueMemSet& vecValues)
		{

			int oldSize = sOldStr.m_nLen;
			sNewStr.m_nOldLen = oldSize;
			sNewStr.m_bChange = true;
			m_nStringDataSize += (sNewStr.m_nLen - oldSize);
			return;
		}

		uint32 GetCompressSize() const
		{
			return m_nCount * m_nStringDataSize;
		}

		uint32 GetLenString(const CommonLib::CString& string)
		{
	 		if (m_StringCoding == scASCII)
				return string.length();
			else
				return string.calcUTF8Length();
		}

		void WriteString(const sStringVal& string, CommonLib::CBlob bufForUff8, CommonLib::IWriteStream *pStream)
		{
			if (m_StringCoding == embDB::scASCII)
			{
				//strcpy((char*)&bufForUff8[0], string.cstr());
				//bufForUff8[string.length()] = 0;

				pStream->write(string.m_string.cstr());
			}
			else if (m_StringCoding == embDB::scUTF8)
			{
				if (bufForUff8.size() < string.m_nLen)
					bufForUff8.resize(string.m_nLen);

				string.m_string.exportToUTF8((char*)&bufForUff8[0], string.m_nLen);
				pStream->write(bufForUff8.buffer(), string.m_nLen);
			}

		}


		void ReadString(sStringVal& string, CommonLib::CBlob bufForUff8, CommonLib::IReadStream *pStream)
		{
			if (string.m_nLen == 0)
				return;

			if (bufForUff8.size() < string.m_nLen)
				bufForUff8.resize(string.m_nLen);
			pStream->read(bufForUff8.buffer(), string.m_nLen);

			if (m_StringCoding == embDB::scASCII)
				string.m_string.loadFromASCII((const char*)bufForUff8.buffer(), string.m_nLen);
			else if (m_StringCoding == embDB::scUTF8)
				string.m_string.loadFromUTF8((char*)&bufForUff8[0]);
		}
		bool BeginEncoding(const TValueMemSet& vecValues)
		{
			return true;
		}
		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_nCount == vecValues.size());
			uint32 nMaxPageLen = m_pLeafCompParams->GetMaxPageStringSize();


			CommonLib::CBlob bufForUff8;

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				const sStringVal& string = vecValues[i];
				int64 nPage = string.m_nPage;
				int32 nPos = string.m_nPos;

				pStream->write(string.m_nLen);
				if (string.m_nLen < nMaxPageLen)
				{
					WriteString(string, bufForUff8, pStream);
				}
				else
				{

					if (string.m_bChange || nPage == -1)
					{
						WriteStreamPagePtr pWriteStream;
						if (string.m_nOldLen >= string.m_nLen && nPage != -1)
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction.get(), nPage, nPos);
						else
						{
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction.get());
							nPage = -1;
						}

						if (nPage == -1)
						{
							nPage = pWriteStream->GetPage();
							nPos = pWriteStream->GetPos();
						}
						WriteString(string, bufForUff8, pWriteStream.get());
					}

					pStream->write(nPage);
					pStream->write(nPos);
				}



			}
			return true;
		}
		bool decode(uint32 nSize, TValueMemSet& vecValues, CommonLib::IReadStream *pStream)
		{
			CommonLib::CBlob bufForUff8;
			for (uint32 i = 0, sz = nSize; i < sz; ++i)
			{
				sStringVal string;
				string.m_bChange = false;
				string.m_nLen = pStream->readIntu32();

			
				if (string.m_nLen < m_pLeafCompParams->GetMaxPageStringSize())
				{
			 		m_nStringDataSize += (sizeof(uint32) + string.m_nLen);
					ReadString(string, bufForUff8, pStream);
				}
				else
				{
					string.m_nPage = pStream->readIntu64();
					string.m_nPos = pStream->readIntu32();
					m_nStringDataSize += (sizeof(uint32) + sizeof(uint64));


					embDB::ReadStreamPagePtr pReadStream = m_pLeafCompParams->GetReadStream(m_pTransaction.get(), string.m_nPage, string.m_nPos);
					ReadString(string, bufForUff8, pReadStream.get());

				}

				vecValues.push_back(string);
			}

			m_nCount = nSize;
			return true;
		}
		void clear()
		{
			m_nCount = 0;
			m_nStringDataSize = 0;
		}

		uint32 count() const
		{
			return m_nCount;
		}

		

	private:

		uint32 m_nStringDataSize;
		uint32 m_nCount;
		uint32 m_nMaxPageLen;
		eStringCoding m_StringCoding;
 		CommonLib::alloc_t* m_pAlloc;
		uint32 m_nPageSize;
		StringFieldCompressorParams *m_pLeafCompParams;
		IDBTransactionPtr m_pTransaction;
	};


	typedef TBaseNodeCompressor<int64, sStringVal, IDBTransaction, TSignedDiffEncoder64, BPStringLeafNodeCompressor, StringFieldCompressorParams> TBPStringLeafCompressor;

	
}

