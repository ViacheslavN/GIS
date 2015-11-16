#ifndef _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#define _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	class WriteStreamPage : public CommonLib::IWriteStreamBase, public CommonLib::AutoRefCounter
	{
	public:
		WriteStreamPage(embDB::IDBTransaction* pTran, uint32 nPageSize, uint16 nObjectPage = -1, uint16 nSubObjectPage = -1) :
		  m_pTran(pTran), m_nPageHeader(-1), m_nBeginPos(0), m_nPageSize(nPageSize), 
			  m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage)
		  {
			

		  }
		  ~WriteStreamPage()
		  {

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
			 
			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_stream, m_nObjectPage, m_nSubObjectPage);
			  }
			   m_stream.seek(m_nBeginPos, CommonLib::soFromCurrent);
			  return true;
			 
		  }

		  int64 GetPage() const {return m_pPage.get() ? m_pPage->getAddr() : -1;}
		  int32 GetPos() const {return m_stream.pos();}
		  virtual void write_bytes(const byte* buffer, size_t size)
		  {

			  uint32 nPos = 0;
			  while(size)
			  {
				  int32 nFreeSize = m_stream.size() - m_stream.pos();
				  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
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
					  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
					  size -= nWriteSize;
					  nPos += nWriteSize;
				  }
				
			  }
			
		  }
		  virtual void write_inverse(const byte* buffer, size_t size)
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
					  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
					  size -= nWriteSize;
					  nPos += nWriteSize;
				  }

			  }

		  }


		  void NextPage()
		  {
			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_nObjectPage, m_nSubObjectPage);
				  header.writeCRC32(m_stream);
			
			  }

			  m_pTran->saveFilePage(m_pPage);
			  FilePagePtr pPage = m_pTran->getNewPage(m_nPageSize);
			  if(!pPage.get())
			  {
				  return; //TO DO Log
			  }
			  m_pPage = pPage;
			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
			  if(m_nObjectPage != -1 && m_nSubObjectPage != -1)
			  {
				  sFilePageHeader header(m_stream, m_nObjectPage, m_nSubObjectPage);
			  }
		  }
 
		  void Save()
		  {
			  if(m_pPage.get())
				  m_pTran->saveFilePage(m_pPage);
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