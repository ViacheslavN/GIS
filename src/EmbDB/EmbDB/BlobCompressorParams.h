#ifndef _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_

#include "embDBInternal.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "StreamPageIngo.h"
namespace embDB
{


	class BlobFieldCompressorParams
	{
	public:
		BlobFieldCompressorParams(int64 nRootPage = -1) : m_nRootPage(nRootPage), m_nMaxPageBlobSize(0), m_nStreamPageInfo(-1)
		{}
		virtual ~BlobFieldCompressorParams(){}


		virtual int64 getRootPage() const 
		{
			return m_nRootPage;
		}
		virtual void setRootPage(int64 nPageID)
		{
			m_nRootPage = nPageID;
		}
		virtual bool read(IDBTransaction *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage, MIN_PAGE_SIZE);
			if(!pPage.get())
				return false; //TO DO Error
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_BLOB_PARAMS_COMPRESS_PAGE)
			{
				pTran->error(_T("BTREE: Page %I64d is not blob params compress page"), m_nRootPage);
				return false;
			}

			m_nMaxPageBlobSize = stream.readIntu32();
			m_nStreamPageInfo = stream.readInt64();

			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}
			return true;
		}
		virtual bool save(IDBTransaction *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage, MIN_PAGE_SIZE);
			if(!pPage.get())
				return false; //TO DO Error


			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_BLOB_PARAMS_COMPRESS_PAGE);
			stream.write((uint32)m_nMaxPageBlobSize);
			stream.write(m_nStreamPageInfo);

			header.writeCRC32(stream);
			pTran->saveFilePage(pPage);
			if(m_nStreamPageInfo != -1)
			{			 
				return m_StreamPageInfo.Save(pTran);
			}
			return !pTran->isError();
		}

		uint32 GetMaxPageBlobSize() const {return m_nMaxPageBlobSize;}
		void SetMaxPageBlobSize (uint32 nSize) {m_nMaxPageBlobSize = nSize;}


		CStreamPageInfo* GetStreamInfo(IDBTransaction *pTran)
		{
			if(m_StreamPageInfo.GetRootPage() == -1)
			{
				FilePagePtr pPage = pTran->getNewPage(MIN_PAGE_SIZE);
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Init(pTran);
				m_nStreamPageInfo = pPage->getAddr();
				save(pTran);
			}
			return &m_StreamPageInfo;
		}
		ReadStreamPagePtr GetReadStream(IDBTransaction *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetReadStream(pTran, nPage, nPos);

		}
		WriteStreamPagePtr GetWriteStream(IDBTransaction *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetWriteStream(pTran, nPage, nPos);

		}


		template<class _Transaction>
		void SaveState(_Transaction * pTransaction)
		{
			if(m_StreamPageInfo.GetRootPage() != -1)
				m_StreamPageInfo.Save(pTransaction);
		}
	private:
		int64 m_nRootPage;
		uint32 m_nMaxPageBlobSize;
		int64 m_nStreamPageInfo;
			CStreamPageInfo m_StreamPageInfo;

	};
}




#endif