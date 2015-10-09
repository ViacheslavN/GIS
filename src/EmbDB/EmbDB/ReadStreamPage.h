#ifndef _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#define _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#include "CommonLibrary/String.h"
#include "IDBTransactions.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{

	class StreamReadPage : public CommonLib::IReadStreamBase
	{
	public:
		StreamReadPage(embDB::IDBTransactions* pTran, int64 nPageHeader, uint32 nBeginPos) :
		  m_pTran(pTran), m_nPageHeader(nPageHeader)
		  {
			

		  }
		  ~StreamReadPage()
		  {

		  }

		  bool open()
		  {
		 
			  m_pPage = m_pTran->getFilePage(m_nPageHeader);
			  if(!m_pPage.get())
				  return false; //TO DO Log;

			
			  assert(m_nBeginPos < m_pPage->getPageSize());
			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
			  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			 

		  }

		  virtual void read_bytes(byte* buffer, size_t size)
		  {

			   uint32 nFreeSize = m_stream.size() - m_stream.pos();
			  if(nFreeSize > size)
			  {
				  m_stream.read_bytes(buffer, size);
			  }
			  else
			  {
				  uint32 nReadSize = size - nFreeSize - sizeof(int64);
				  if(nReadSize)
				  {
					  m_stream.read_bytes(buffer, nReadSize);
					 int64 nNextPage = m_stream.readInt64();
					  if(!NextPage(nNextPage))
						  return; //TO DO Log

					   m_stream.read_bytes(buffer + nReadSize, size - nReadSize);
				  }
			  }

			 

		  }
		  virtual void read_inverse(byte* buffer, size_t size)
		  {
			  uint32 nFreeSize = m_stream.size() - m_stream.pos();
			  if(nFreeSize > size)
			  {
				  m_stream.read_inverse(buffer, size);
			  }
			  else
			  {
				  uint32 nReadSize = size - nFreeSize - sizeof(int64);;
				  if(nReadSize)
				  {
					  m_stream.read_inverse(buffer, nReadSize);
					  int64 nNextPage = m_stream.readInt64();
					  if(!NextPage(nNextPage))
						  return;

					  m_stream.read_inverse(buffer + nReadSize, size - nReadSize);
				  }
			  }
		  }

		  bool NextPage(int64 nNextPage)
		  {
			  if(nNextPage == -1)
				  return false; // TO DO Log

			 m_pPage = m_pTran->getFilePage(nNextPage);
			  if(!m_pPage.get())
				  return false;  // TO DO Log

			  m_stream.attach(m_pPage->getRowData(), m_pPage->getPageSize());
 
		  }

	public:
		FilePagePtr m_pPage;
		embDB::IDBTransactions* m_pTran;
		uint32 m_nBeginPos;
		int64 m_nPageHeader;
    	 CommonLib::FxMemoryReadStream m_stream;
	
 
	};

}