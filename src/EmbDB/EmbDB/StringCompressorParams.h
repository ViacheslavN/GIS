#ifndef _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_STRING_COMPRESSOR_PARAMS_H_

#include "embDBInternal.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "StreamPageIngo.h"
namespace embDB
{


	class StringFieldCompressorParams : public CompressorParamsBaseImp
	{
	public:
		StringFieldCompressorParams(int64 nRootPage = -1) : m_StringCoding(scUTF8), m_nLen(0),
			m_nMaxPageStringSize(400), m_nStreamPageInfo(-1)
		{}
		virtual ~StringFieldCompressorParams(){}


	/*	virtual int64 getRootPage() const 
		{
			return m_nRootPage;
		}
		virtual void setRootPage(int64 nPageID)
		{
			m_nRootPage = nPageID;
		}*/
		virtual bool load(CommonLib::IReadStream* pStream,  IDBTransaction *pTran)
		{


			m_StringCoding = (eStringCoding)pStream->readIntu32();
			m_nLen = pStream->readIntu32();
			m_nMaxPageStringSize= pStream->readIntu32();
			m_nStreamPageInfo = pStream->readInt64();
			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}

			return true;
		}
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{

			if(m_nStreamPageInfo == -1)
			{			 
				FilePagePtr pPage = pTran->getNewPage(MIN_PAGE_SIZE);
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Save(pTran);
				m_nStreamPageInfo = pPage->getAddr();
			}


			pStream->write((uint32)m_StringCoding);
			pStream->write(m_nLen);
			pStream->write(m_nMaxPageStringSize);
			pStream->write(m_nStreamPageInfo);

			
	

			 return !pTran->isError();
		}

		uint32 GetStringLen() const {return m_nLen;}
		eStringCoding GetStringCoding() const {return m_StringCoding;}
		uint32 GetMaxPageStringSize() const {return m_nMaxPageStringSize;}

		void SetStringLen(uint32 nLen){m_nLen = nLen;}
		void SetMaxPageStringSize(uint32 nMaxPageStringSize){m_nMaxPageStringSize = nMaxPageStringSize;}
		void setStringCoding(eStringCoding sc){m_StringCoding = sc;}


		CStreamPageInfo* GetStreamInfo(IDBTransaction *pTran)
		{
			if(m_StreamPageInfo.GetRootPage() == -1)
			{
				FilePagePtr pPage = pTran->getNewPage(MIN_PAGE_SIZE);
				m_nStreamPageInfo = pPage->getAddr();
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Init(pTran);
				save(NULL, pTran);
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
		uint32 m_nMaxPageStringSize;
		uint32 m_nLen;
		eStringCoding m_StringCoding;
		int64 m_nStreamPageInfo;

		CStreamPageInfo m_StreamPageInfo;
		 

		
	};
}




#endif