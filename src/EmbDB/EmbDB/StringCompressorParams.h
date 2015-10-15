#ifndef _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "StreamPageIngo.h"
namespace embDB
{


	class StringFieldCompressorParams
	{
	public:
		StringFieldCompressorParams(int64 nRootPage = -1) : m_nRootPage(nRootPage), m_StringCoding(scUndefined), m_nLen(0),
			m_nMaxPageStringSize(400), m_nStreamPageInfo(-1)
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
			m_nMaxPageStringSize= stream.readIntu32();
			m_nStreamPageInfo = stream.readInt64();
			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}

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
			stream.write(m_nLen);
			stream.write(m_nMaxPageStringSize);
			stream.write(m_nStreamPageInfo);

			
			header.writeCRC32(stream);
			 pTran->saveFilePage(pPage);
			 if(m_nStreamPageInfo != -1)
			 {			 
				 return m_StreamPageInfo.Save(pTran);
			 }

			 return !pTran->isError();
		}

		uint32 GetStringLen() const {return m_nLen;}
		eStringCoding GetStringCoding() const {return m_StringCoding;}
		uint32 GetMaxPageStringSize() const {return m_nMaxPageStringSize;}

		void SetStringLen(uint32 nLen){m_nLen = nLen;}
		void SetMaxPageStringSize(uint32 nMaxPageStringSize){m_nMaxPageStringSize = nMaxPageStringSize;}
		void setStringCoding(eStringCoding sc){m_StringCoding = sc;}


		CStreamPageInfo* GetStreamInfo(IDBTransactions *pTran)
		{
			if(m_StreamPageInfo.GetRootPage() == -1)
			{
				FilePagePtr pPage = pTran->getNewPage();
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Init(pTran);
			}
			return &m_StreamPageInfo;
		}
		ReadStreamPagePtr GetReadStream(IDBTransactions *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetReadStream(pTran, nPage, nPos);

		}
		WriteStreamPagePtr GetWriteStream(IDBTransactions *pTran, int64 nPage = -1, int32 nPos = -1)
		{
			CStreamPageInfo* pStreamInfo = GetStreamInfo(pTran);
			return pStreamInfo->GetWriteStream(pTran, nPage, nPos);

		}
	private:
		uint32 m_nMaxPageStringSize;
		uint32 m_nLen;
		int64 m_nRootPage;
		eStringCoding m_StringCoding;
		int64 m_nStreamPageInfo;

		CStreamPageInfo m_StreamPageInfo;
		 

		
	};
}




#endif