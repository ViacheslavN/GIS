#ifndef _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#define _EMBEDDED_DATABASE_WRITE_STREAM_PAGE_H_
#include "CommonLibrary/String.h"
#include "IDBTransactions.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	class WriteStreamPage : public CommonLib::IWriteStreamBase, public CommonLib::AutoRefCounter
	{
	public:
		WriteStreamPage(embDB::IDBTransactions* pTran) :
		  m_pTran(pTran), m_nPageHeader(-1), m_nBeginPos(0)
		  {
			

		  }
		  ~WriteStreamPage()
		  {

		  }

		  bool open(int64 nPageHeader, uint32 nBeginPos, bool bReopen = false)
		  {
 
			  if(!m_pPage.get() || m_nPageHeader != nPageHeader || bReopen)
			  {
				  m_nPageHeader = nPageHeader;
				  m_nBeginPos = nBeginPos;
				  m_pPage = m_pTran->getFilePage(m_nPageHeader);
				  if(!m_pPage.get())
					  return false; //TO DO Log;
			  }

			

			
			  assert(m_nBeginPos < m_pPage->getPageSize());
			  uint32 nSize = m_pPage->getPageSize() - m_nBeginPos;
			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
			  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			 
		  }

		  virtual void write_bytes(const byte* buffer, size_t size)
		  {

			  uint32 nFreeSize = m_stream.size() - m_stream.pos();
			  if(nFreeSize > size)
			  {
				  m_stream.write_bytes(buffer, size);
			  }
			  else
			  {
				  uint32 nWriteSize = size - nFreeSize - sizeof(int64);
				  if(nWriteSize)
				  {
					  m_stream.write_bytes(buffer, nWriteSize);
					 FilePagePtr pPage = m_pTran->getNewPage();
					 if(!pPage.get())
					 {
						 return; //TO DO Log
					 }
					 m_stream.write(pPage->getAddr());
					 m_pTran->saveFilePage(m_pPage);
					
					 m_pPage = pPage;
					 m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
					 m_stream.write_bytes(buffer + nWriteSize, size - nWriteSize);
				  }
			  }
		  }
		  virtual void write_inverse(const byte* buffer, size_t size)
		  {

			  uint32 nFreeSize = m_stream.size() - m_stream.pos();
			  if(nFreeSize > size)
			  {
				  m_stream.write_inverse(buffer, size);
			  }
			  else
			  {
				  uint32 nWriteSize = size - nFreeSize - sizeof(int64);
				  if(nWriteSize)
				  {
					  m_stream.write_inverse(buffer, nWriteSize);
					  FilePagePtr pPage = m_pTran->getNewPage();
					  if(!pPage.get())
					  {
						  return; //TO DO Log
					  }
					  m_stream.write(pPage->getAddr());
					  m_pTran->saveFilePage(m_pPage);

					  m_pPage = pPage;
					  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
					  m_stream.write_inverse(buffer + nWriteSize, size - nWriteSize);
				  }
			  }
		  }

 

	public:
		FilePagePtr m_pPage;
		embDB::IDBTransactions* m_pTran;
		uint32 m_nBeginPos;
		int64 m_nPageHeader;
    	CommonLib::FxMemoryWriteStream m_stream;
	
 
	};


	COMMON_LIB_REFPTR_TYPEDEF(WriteStreamPage);
}

#endif