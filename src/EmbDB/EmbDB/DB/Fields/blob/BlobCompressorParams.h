#ifndef _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_
#define _EMBEDDED_DATABASE_BLOB_COMPRESSOR_PARAMS_H_

#include "embDBInternal.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "StreamPageIngo.h"
#include "DB/GlobalParams.h"
namespace embDB
{


	class BlobFieldCompressorParams :public CompressorParamsBaseImp
	{
	public:
		BlobFieldCompressorParams() :  m_nMaxPageBlobSize(0), m_nStreamPageInfo(-1), m_StreamPageInfo(CGlobalParams::Instance().GetCheckCRC())
		{}
		virtual ~BlobFieldCompressorParams(){}

		virtual bool load(CommonLib::IReadStream *pSteram,  IDBTransaction *pTran)
		{

			m_nMaxPageBlobSize = pSteram->readIntu32();
			m_nStreamPageInfo = pSteram->readInt64();

			if(m_nStreamPageInfo != -1)
			{
				m_StreamPageInfo.SetRootPage(m_nStreamPageInfo);
				return m_StreamPageInfo.Load(pTran);
			}
			return true;
		}
		virtual bool save(CommonLib::IWriteStream *pSteram,IDBTransaction *pTran)
		{
			 
			if(m_nStreamPageInfo == -1)
			{			 
				FilePagePtr pPage = pTran->getNewPage(MIN_PAGE_SIZE);
				m_StreamPageInfo.SetRootPage(pPage->getAddr());
				m_StreamPageInfo.Init(pTran);
				m_nStreamPageInfo = pPage->getAddr();
				return m_StreamPageInfo.Save(pTran);
			}

			pSteram->write((uint32)m_nMaxPageBlobSize);
			pSteram->write(m_nStreamPageInfo);

			 
			
			return !pTran->isError();
		}

		uint32 GetMaxPageBlobSize() const {return m_nMaxPageBlobSize;}
		void SetMaxPageBlobSize (uint32 nSize) {m_nMaxPageBlobSize = nSize;}


		CStreamPageInfo* GetStreamInfo(IDBTransaction *pTran)
		{
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
		uint32 m_nMaxPageBlobSize;
		int64 m_nStreamPageInfo;
		CStreamPageInfo m_StreamPageInfo;

	};
}




#endif