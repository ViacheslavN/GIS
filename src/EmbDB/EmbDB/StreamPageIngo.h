#ifndef _EMBEDDED_DATABASE_STREAM_PAGE_INFO_H_
#define _EMBEDDED_DATABASE_STREAM_PAGE_INFO_H_
#include "CommonLibrary/String.h"
#include "IDBTransactions.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "WriteStreamPage.h"
#include "ReadStreamPage.h"
namespace embDB
{


	class CStreamPageInfo
	{
	public:

			CStreamPageInfo(uint32 nSizePage = 1024*1024) : m_nSizePage(nSizePage), m_nRootPage(-1), m_nBeginStream(-1), m_nEndStream(-1), 
				m_nPos(0)
			{

			}
			~CStreamPageInfo()
			{

			}
 

			void SetPageSize(uint32 nSizePage) {m_nSizePage = nSizePage;}
			void SetRootPage(int64 nRootPage) {m_nRootPage = nRootPage;}
			void SetBeginPage(int64 nBeginStream) {m_nBeginStream = nBeginStream;}
			void SetEndPage(int64 nEndStream) {m_nEndStream = nEndStream;}
			void SetPos(uint32 nPos) {m_nPos = nPos;}


			int64 GetRootPage() const {return m_nRootPage;}
			int64 GetBeginPage() const {return m_nBeginStream;}
			int64 GetEndPage() const  {return m_nEndStream;}
			uint32 GetPageSize() {return m_nSizePage;}
			uint32 GetPos() const  {return m_nPos;}


			bool Save(embDB::IDBTransactions* pTran)
			{

				FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
				if(!pPage.get())
					return false; //TO DO Error

				if(m_pWriteStream.get())
				{
					m_nEndStream = m_pWriteStream->GetPage();
					m_nPos		= m_pWriteStream->GetPos();

					m_pWriteStream->Save();
				}


				CommonLib::FxMemoryWriteStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, BTREE_PAGE, BTREE_STREAM_PAGE_INFO);
				stream.write(m_nBeginStream);
				stream.write(m_nEndStream);
				stream.write(m_nPos);
				stream.write(m_nSizePage);
				header.writeCRC32(stream);
				pTran->saveFilePage(pPage);
				return !pTran->isError();
			}

			bool Load(IDBTransactions* pTran)
			{
				FilePagePtr pPage = pTran->getFilePage(m_nRootPage);
				if(!pPage.get())
					return false; //TO DO Error
				CommonLib::FxMemoryReadStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream);
				if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_STREAM_PAGE_INFO)
				{
					pTran->error(_T("BTREE: Page %I64d is not stream info page"), m_nRootPage);
					return false;
				}
				m_nBeginStream = stream.readInt64();
				m_nEndStream = stream.readInt64();
				m_nPos = stream.readIntu32();
				m_nSizePage = stream.readIntu32();
				return true;
			}

			WriteStreamPagePtr GetWriteStream(IDBTransactions* pTran, int64 nPage = -1, int32 nPos = -1)
			{
				if(!m_pWriteStream.get())
				{
					m_pWriteStream = new WriteStreamPage(pTran, m_nSizePage);
					if(m_nEndStream == -1)
					{ 
						Init(pTran);
						m_pWriteStream->open(m_nEndStream, m_nPos);
						return m_pWriteStream;
					}
				}
				m_pWriteStream->open(nPage, nPos);
				return m_pWriteStream;
			}
			ReadStreamPagePtr GetReadStream(IDBTransactions* pTran, int64 nPage = -1, int32 nPos = -1)
			{
				if(!m_pReadStream.get())
				{
					m_pReadStream = new ReadStreamPage(pTran, m_nSizePage);
				}
				m_pReadStream->SetEnd(m_nEndStream, nPos);
				m_pReadStream->open(nPage != 1 ? nPage : m_nEndStream, nPos  != -1 ? nPos : m_nPos);
				return m_pReadStream;
			}

			void Init(IDBTransactions* pTran)
			{
				if(m_nBeginStream == -1)
				{
					FilePagePtr pPage = pTran->getNewPage(m_nSizePage);
					if(!pPage.get())
						return; //TO DO Error
					m_nBeginStream = pPage->getAddr();
					m_nEndStream  = pPage->getAddr();
					m_nPos = 0;
					Save(pTran);

					m_pWriteStream = new WriteStreamPage(pTran, m_nSizePage);
					m_pWriteStream->open(m_nEndStream, m_nPos);
				 
				}
			}

	private:
		uint32 m_nSizePage;
		int64 m_nRootPage;
		int64 m_nBeginStream;
		int64 m_nEndStream;
		uint32 m_nPos;

		WriteStreamPagePtr m_pWriteStream;
		ReadStreamPagePtr m_pReadStream;
	};
}

#endif