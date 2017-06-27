#ifndef _EMBEDDED_DATABASE_BLOB_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BLOB_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/blob.h"
#include "CompressorParams.h"
#include "BlobCompressorParams.h"
#include "BlobVal.h"
#include "utils/streams/ReadStreamPage.h"
#include "utils/streams/WriteStreamPage.h"

#include "../../BTreePlus/BaseNodeCompressor.h"
namespace embDB
{
	template<class _TCompParams = BlobFieldCompressorParams>
	class BlobCompressor  
	{
	public:
		typedef STLAllocator<sBlobVal> TValueAlloc;
		typedef std::vector<sBlobVal, TValueAlloc> TValueMemSet;
		typedef _TCompParams TCompParams;
		

		BlobCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc, TCompParams *pParams = nullptr) : m_pAlloc(pAlloc), m_pLeafCompParams(pParams),
			m_nPageSize(nPageSize), m_nBlobDataSize(0), m_nCount(0), m_nMaxBlobSize(0), m_pTransaction(NULL)
		{}


		template<typename _Transactions  >
		bool  init(TCompParams *pParams, _Transactions *pTran)
		{
			m_pLeafCompParams = pParams;
			assert(m_pLeafCompParams);
			m_pTransaction = pTran;
			m_nMaxBlobSize = m_pLeafCompParams->GetMaxPageBlobSize();
			return true;
		}
		int GetBlobSize(const sBlobVal& blob) const 
		{

			uint32 nSize = sizeof(uint32);
			if(blob.m_nSize < m_nMaxBlobSize)
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

		void AddSymbol(uint32 nSize,  int nIndex, const sBlobVal& blob, const TValueMemSet& vecValues)
		{
			m_nCount++;
			m_nBlobDataSize += GetBlobSize(blob);
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const sBlobVal& blob, const TValueMemSet& vecValues)
		{
			m_nCount--;
			m_nBlobDataSize -= GetBlobSize(blob);
		}
		void UpdateSymbol(uint32 nIndex, sBlobVal& newBlob, const sBlobVal& OldBlob, const TValueMemSet& vecValues)
		{
			RemoveSymbol(vecValues.size(), nIndex, OldBlob, vecValues);
			AddSymbol(vecValues.size(), nIndex, newBlob, vecValues);
		}

		virtual ~BlobCompressor()
		{
		}

		uint32 GetCompressSize() const
		{
			return m_nBlobDataSize;
		}
		bool BeginEncoding(const TValueMemSet& vecValues)
		{
			return true;
		}
		bool encode(const TValueMemSet& vecValues, CommonLib::IWriteStream* pStream)
		{	
			assert(m_nCount == vecValues.size());

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i )
			{
		
				const sBlobVal& blob = vecValues[i];

				int64 nPage = blob.m_nPage;
				uint32 nBeginPos = blob.m_nBeginPos;

				pStream->write(blob.m_nSize);
				if(blob.m_nSize < m_nMaxBlobSize)
				{					
					pStream->write(blob.m_blob.buffer(), blob.m_nSize);
				}
				else
				{				
					if(blob.m_bChange || nPage == -1)
					{
						WriteStreamPagePtr pWriteStream;
						if(blob.m_nOldSize >= blob.m_nSize && nPage != -1)
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction, nPage,  nBeginPos);
						else
						{
							pWriteStream = m_pLeafCompParams->GetWriteStream(m_pTransaction);
							nPage = -1;
						}

						if(nPage == -1)
						{
							nPage = pWriteStream->GetPage();
							nBeginPos = pWriteStream->GetPos();
						}

						pWriteStream->write(blob.m_blob.buffer(), blob.m_blob.size());
					}

					pStream->write(nPage);
					pStream->write((uint32)nBeginPos);
				}

			}
			return true;
		}
		bool decode(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{
			m_nCount = nSize;

			vecValues.resize(nSize);
			for (uint32 nIndex = 0; nIndex < nSize; ++nIndex)
			{
			 

				sBlobVal& blob = vecValues[i];
				blob.m_bChange = false;

				blob.m_nSize = pStream->readIntu32();
				m_nBlobDataSize += sizeof(uint32);
				if(blob.m_nSize  < m_nMaxBlobSize)
				{
			 
					blob.m_blob.resize(blob.m_nSize);
					pStream->read(blob.m_blob.buffer(), blob.m_nSize);
					m_nBlobDataSize += blob.m_nSize;
				}
				else
				{
					blob.m_nPage = pStream->readIntu64();
					blob.m_nBeginPos = pStream->readIntu32();
					m_nBlobDataSize += (sizeof(uint32) + sizeof(uint64));

					blob.m_blob.resize(blob.m_nSize);

					embDB::ReadStreamPagePtr pReadStream = m_pLeafCompParams->GetReadStream(m_pTransaction, blob.m_nPage, blob.m_nBeginPos);
					pReadStream->read(blob.m_blob.buffer(), blob.m_nSize);

				}
			}
			return true;
		}

		void clear()
		{
			m_nCount = 0;
			m_nBlobDataSize = 0;
		}

	private:

		uint32 m_nBlobDataSize;
		uint32 m_nCount;
		uint32 m_nMaxBlobSize;

		CommonLib::alloc_t* m_pAlloc;
		TCompParams *m_pLeafCompParams;
		IDBTransaction	*m_pTransaction;
		uint32 m_nPageSize;
	};

	template<class _TCompParams = BlobFieldCompressorParams>
	class BlobLeafNodeCompressor : public TBaseNodeCompressor<int64, sBlobVal, IDBTransaction, TSignedDiffEncoder64, BlobCompressor<_TCompParams>, _TCompParams> 
	{
	public:
		typedef TBaseNodeCompressor<int64, sBlobVal, IDBTransaction, TSignedDiffEncoder64, BlobCompressor<_TCompParams>, _TCompParams> TBase;
		BlobLeafNodeCompressor(uint32 nPageSize, CommonLib::alloc_t* pAlloc, _TCompParams *pParams = nullptr) : TBase(nPageSize, pAlloc, pParams)
		{}
	};

}

#endif