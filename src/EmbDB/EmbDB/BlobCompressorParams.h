#ifndef _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{


	class BlobFieldCompressorParams
	{
	public:
		BlobFieldCompressorParams(int64 nRootPage = -1) : m_nRootPage(nRootPage), m_nMaxPageBlobSize(0)
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
		virtual bool read(IDBTransactions *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
			if(!pPage.get())
				return false; //TO DO Error
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_BLOB_PARAMS_COMPRESS_PAGE)
			{
				pTran->error(_T("BTREE: Page %I64d is not string params compress page"), m_nRootPage);
				return false;
			}

			m_nMaxPageBlobSize = (eStringCoding)stream.readIntu32();
		
			return true;
		}
		virtual bool save(IDBTransactions *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
			if(!pPage.get())
				return false; //TO DO Error


			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_BLOB_PARAMS_COMPRESS_PAGE);
			stream.write((uint32)m_nMaxPageBlobSize);

			header.writeCRC32(stream);
			pTran->saveFilePage(pPage);
			return !pTran->isError();
		}

		uint32 GetMaxPageBlobSize() const {return m_nMaxPageBlobSize;}
		void SetMaxPageBlobSize (uint32 nSize) {m_nMaxPageBlobSize = nSize;}
	private:
		int64 m_nRootPage;
		uint32 m_nMaxPageBlobSize;

	};
}




#endif