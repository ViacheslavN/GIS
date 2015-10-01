#ifndef _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{


	class StringFieldCompressorParams
	{
	public:
		StringFieldCompressorParams() : m_nRootPage(-1), m_StringCoding(scUndefined), m_nLen(0)
		{}
		virtual ~StringFieldCompressorParams(){}


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
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_STRING_PARAMS_COMPRESS_PAGE)
			{
				pTran->error(_T("BTREE: Page %I64d is not string params compress page"), m_nRootPage);
				return false;
			}

			m_StringCoding = (eStringCoding)stream.readIntu32();
			m_nLen = stream.readIntu32();

			return true;
		}
		virtual bool save(IDBTransactions *pTran)
		{
			FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
			if(!pPage.get())
				return false; //TO DO Error


			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_STRING_PARAMS_COMPRESS_PAGE);
			stream.write((uint32)m_StringCoding);
			stream.write((uint32)m_nLen);
			header.writeCRC32(stream);
			 pTran->saveFilePage(pPage);
			 return !pTran->isError();
		}

		uint32 GetStringLen() const {return m_nLen;}
		eStringCoding GetStringCoding() const {return m_StringCoding;}
	private:
		uint32 m_nLen;
		int64 m_nRootPage;
		eStringCoding m_StringCoding;
	};
}




#endif