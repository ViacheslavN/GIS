#ifndef _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#define _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{
	class WriteStreamPage : public CommonLib::TMemoryStreamBase<CommonLib::IWriteStreamBase>, public CommonLib::AutoRefCounter
	{
	public:
		WriteStreamPage(IDBTransaction* pTran, uint32 nPageSize, uint16 nObjectPage = 0, uint16 nSubObjectPage = 0) :
		  m_pTran(pTran), m_nPageHeader(-1), m_nBeginPos(0), m_nPageSize(nPageSize), 
			  m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage)
		  {
			

		  }
		  ~WriteStreamPage()
		  {

		  }

		  bool open(FilePagePtr pPage,  uint32 nBeginPos = 0)
		  {
			    m_nBeginPos = nBeginPos;
				m_pPage = pPage;
				if(!m_pPage.get())
					return false;
			  assert(m_nBeginPos < m_pPage->getPageSize());

			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());
			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_nObjectPage, m_nSubObjectPage, pPage->getPageSize());
				  if(m_nBeginPos != 0)
					  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  }
			  else
				  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  return true;
		  }


		  bool open(int64 nPageHeader, uint32 nBeginPos, bool bReopen = false)
		  {
			  if(nPageHeader == -1)
			  {
				  return m_nPageHeader != -1;
			  }
 
			  if(!m_pPage.get() || m_pPage->getAddr() != nPageHeader || bReopen)
			  {
				  m_nPageHeader = nPageHeader;
				  m_nBeginPos = nBeginPos;
				  m_pPage = m_pTran->getFilePage(m_nPageHeader,  m_nPageSize, false);
				  if(!m_pPage.get())
					  return false; //TO DO Log;
			  }

			

			
			  assert(m_nBeginPos < m_pPage->getPageSize());
			 
			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());
			  if(m_nObjectPage !=0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_nObjectPage, m_nSubObjectPage, m_pPage->getPageSize());
				  if(m_nBeginPos != 0)
					  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  }
			  else
				   m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  return true;
			 
		  }

		  int64 GetPage() const {return m_pPage.get() ? m_pPage->getAddr() : -1;}
		  int32 GetPos() const {return m_stream.pos();}
		  virtual void write_bytes(const byte* buffer, uint32 size)
		  {

			  uint32 nPos = 0;
			  while(size)
			  {
				  int32 nFreeSize = m_stream.size() - m_stream.pos();
				  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
				  {
					  nFreeSize -= sFilePageHeader::size();  
				  }

				  if((int32)size <= nFreeSize)
				  {
					  m_stream.write_bytes(buffer + nPos, size);
					  size = 0;
				  }
				  else
				  {
					  if(nFreeSize < sizeof(int64))
					  { 
						  NextPage();
						  continue;
					  }
					  uint32 nWriteSize = nFreeSize - sizeof(int64);
			   

					  m_stream.write_bytes(buffer + nPos, nWriteSize);
					  FilePagePtr pPage = m_pTran->getNewPage(m_nPageSize);
					  if(!pPage.get())
					  {
						 return; //TO DO Log
					  }
					  m_stream.write(pPage->getAddr());
					  m_pTran->saveFilePage(m_pPage);

					  m_pPage = pPage;
					  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());
					  size -= nWriteSize;
					  nPos += nWriteSize;
				  }
				
			  }
			
		  }
		  virtual void write_inverse(const byte* buffer, uint32 size)
		  {

			  uint32 nPos = 0;
			  while(size)
			  {
				  int32 nFreeSize = m_stream.size() - m_stream.pos();

				  if((int32)size < nFreeSize)
				  {
					  m_stream.write_inverse(buffer + nPos, size);
					  size = 0;
				  }
				  else
				  {
					  if(nFreeSize < sizeof(int64))
					  { 
						  NextPage();
						  continue;
					  }
					  uint32 nWriteSize = size - nFreeSize - sizeof(int64);


					  m_stream.write_inverse(buffer, nWriteSize);
					  FilePagePtr pPage = m_pTran->getNewPage(m_nPageSize);
					  if(!pPage.get())
					  {
						  return; //TO DO Log
					  }
					  m_stream.write(pPage->getAddr());
					  m_pTran->saveFilePage(m_pPage);

					  m_pPage = pPage;
					  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());
					  size -= nWriteSize;
					  nPos += nWriteSize;
				  }

			  }

		  }


		  void NextPage()
		  {
			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_nObjectPage, m_nSubObjectPage, m_nPageSize);
				  header.writeCRC32(m_stream);
			
			  }

			  m_pTran->saveFilePage(m_pPage);
			  FilePagePtr pPage = m_pTran->getNewPage(m_nPageSize);
			  if(!pPage.get())
			  {
				  return; //TO DO Log
			  }
			  m_pPage = pPage;
			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());
			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_nObjectPage, m_nSubObjectPage, m_pPage->getPageSize());
			  }
		  }
 
		  void Save()
		  {
			  if(m_pPage.get())
			  {
				  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
				  {
					  sFilePageHeader header(m_nObjectPage, m_nSubObjectPage, m_pPage->getPageSize());
					  header.writeCRC32(m_stream);

				  }
				  m_pTran->saveFilePage(m_pPage);
			  }
		  }
		  virtual void writeStream(CommonLib::IStream *pStream, int32 nPos = -1, int32 nSize = -1)
		  {
			  CommonLib::IMemoryStream *pMemStream = dynamic_cast<CommonLib::IMemoryStream *>(pStream);
			  if(pMemStream)
			  {
				  uint32 _nPos = (nPos != -1 ? nPos : 0);
				  uint32 _nSize= (nSize != -1 ? nSize : pStream->size());

				  write(_nSize);
				  write(pMemStream->buffer() + _nPos, _nSize);
			  }
		  }
	public:
		FilePagePtr m_pPage;
		embDB::IDBTransaction* m_pTran;
		uint32 m_nBeginPos;
		int64 m_nPageHeader;
		uint32 m_nPageSize;
    	CommonLib::FxMemoryWriteStream m_stream;
		uint16 m_nObjectPage;
		uint16 m_nSubObjectPage; 
	
 
	};


	COMMON_LIB_REFPTR_TYPEDEF(WriteStreamPage);
}

#endif