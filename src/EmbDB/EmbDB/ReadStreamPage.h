#ifndef _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#define _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	class ReadStreamPage : public CommonLib::IReadStreamBase, public CommonLib::AutoRefCounter
	{
	public:
		ReadStreamPage(IFilePage* pTran, uint32 nPageSize, uint16 nObjectPage = -1, uint16 nSubObjectPage = -1) :
		  m_pTran(pTran), m_nPageHeader(-1), m_nEndPage(-1), m_nEndPos(0), m_nPageSize(nPageSize), 
			  m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage)
		  {									  
			

		  }
		  ~ReadStreamPage()
		  {

		  }

		  void SetEnd(int64 nEndPage, uint32 nEndPos)
		  {
			  m_nEndPage = nEndPage;
			  m_nEndPos = nEndPos;
		  }
		  bool open(FilePagePtr pPage,  uint32 nBeginPos = 0)
		  {
			  m_nBeginPos = nBeginPos;
			  m_pPage = pPage;
			  assert(m_nBeginPos < m_pPage->getPageSize());

			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_stream, !m_pPage->isCheck());
				  if(!m_pPage->isCheck() && !header.isValid())
				  {
					  //TO DO Logs
					  return false;
				  }
				  if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				  {
					  //TO DO Logs
					  return false;
				  }

				  m_pPage->setCheck(true);
			  }



			  m_stream.seek(m_nBeginPos, CommonLib::soFromCurrent);
			  return true;
		  }

		  bool open(int64 nPageHeader, uint32 nBeginPos, bool bReopen = false)
		  {
		 
			  if(!m_pPage.get() || m_pPage->getAddr() != nPageHeader || bReopen)
			  {
				  m_nPageHeader = nPageHeader;
				  m_nBeginPos = nBeginPos;
				  m_pPage = m_pTran->getFilePage(m_nPageHeader, m_nPageSize, true);
				  if(!m_pPage.get())
					  return false; //TO DO Log;
			  }
			  
			  assert(m_nBeginPos < m_pPage->getPageSize());
			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_stream, !m_pPage->isCheck());
				  if(!header.isValid())
				  {
					  //TO DO Logs
					  return false;
				  }
				  if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				  {
					  //TO DO Logs
					  return false;
				  }

				  m_pPage->setCheck(true);

				  if(m_nBeginPos != 0)
					 m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  }
			  else
				  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);

			


			 
				return true;

		  }

		  virtual void read_bytes(byte* buffer, size_t size)
		  {

			
			  uint32 nPos = 0;
			  while(size)
			  {
				    uint32 nFreeSize = m_stream.size() - m_stream.pos();
					if(nFreeSize >= size)
					{
						m_stream.read_bytes(buffer + nPos, size);
						size = 0;
					}
					else
					{
						uint32 nReadSize = nFreeSize - sizeof(int64);
						if(nReadSize)
						{
							m_stream.read_bytes(buffer + nPos, nReadSize);
							size -= nReadSize;
							nPos += nReadSize;

							int64 nNextPage = m_stream.readInt64();
							if(!NextPage(nNextPage))
								return; //TO DO Log
						}
					}

			  }
			
			 

		  }
		  virtual void read_inverse(byte* buffer, size_t size)
		  {
			  uint32 nPos = 0;
			  while(size)
			  {
				  uint32 nFreeSize = m_stream.size() - m_stream.pos();
				  if(nFreeSize >= size)
				  {
					  m_stream.read_inverse(buffer + nPos, size);
					  size = 0;
				  }
				  else
				  {
					  uint32 nReadSize = nFreeSize - sizeof(int64);
					  if(nReadSize)
					  {
						  m_stream.read_inverse(buffer + nPos, nReadSize);
						  size -= nReadSize;
						  nPos += nReadSize;

						  int64 nNextPage = m_stream.readInt64();
						  if(!NextPage(nNextPage))
							  return; //TO DO Log
					  }
				  }

			  }
		  }


		  virtual bool checkRead(uint32 nSize) const
		  {
			  return true;
		  }
		  virtual bool IsEndOfStream() const
		  {
			  return m_nPageHeader == m_nEndPage && m_stream.pos() == m_nEndPos;
		  }
		  virtual bool AttachStream(CommonLib::IStream *pStream, uint32 nSize, bool bSeek = true)
		  {
			  return false;
		  }

		  bool NextPage(int64 nNextPage)
		  {
			  if(nNextPage == -1)
				  return false; // TO DO Log

			 m_pPage = m_pTran->getFilePage(nNextPage, m_nPageSize, true);
			  if(!m_pPage.get())
				  return false;  // TO DO Log

			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_stream, !m_pPage->isCheck());
				  if(!header.isValid())
				  {
					  //TO DO Logs
					  return false;
				  }
				  if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				  {
					  //TO DO Logs
					  return false;
				  }

				  m_pPage->setCheck(true);
			  }



			return true;
		  }

	public:
		FilePagePtr m_pPage;
		IFilePage* m_pTran;
		uint32 m_nBeginPos;
		int64 m_nPageHeader;
		int64 m_nEndPage;
		uint32 m_nEndPos;
		uint32 m_nPageSize;
    	CommonLib::FxMemoryReadStream m_stream;
		uint16 m_nObjectPage;
		uint16 m_nSubObjectPage; 
 
	};
	COMMON_LIB_REFPTR_TYPEDEF(ReadStreamPage);
}

#endif