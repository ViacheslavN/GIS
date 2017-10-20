#ifndef _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#define _EMBEDDED_DATABASE_STREAM_READ_PAGE_H_
#include "CommonLibrary/String.h"
#include "../../embDBInternal.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/FixedMemoryStream.h"
namespace embDB
{
	template<class _TStorage>	
	class TReadStreamPage : public CommonLib::TMemoryStreamBase<CommonLib::IReadStreamBase>, public CommonLib::AutoRefCounter
	{
	public:
		TReadStreamPage(_TStorage* pTran, uint32 nPageSize, bool bCheckCRC, uint16 nObjectPage = 0, uint16 nSubObjectPage = 0) :
		  m_pTran(pTran), m_nPageHeader(-1), m_nEndPage(-1), m_nEndPos(0), m_nPageSize(nPageSize), 
			  m_nObjectPage(nObjectPage), m_nSubObjectPage(nSubObjectPage), m_bCheckCRC(bCheckCRC)
		  {									  
			

		  }
		  ~TReadStreamPage()
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

			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_pPage->getPageSize(), m_bCheckCRC);
				  if(/*!m_pPage->isCheck() &&*/ !header.isValid())
				  {
					  //TO DO Logs
					  return false;
				  }
				  if(header.m_nObjectPageType != m_nObjectPage || header.m_nSubObjectPageType != m_nSubObjectPage)
				  {
					  //TO DO Logs
					  return false;
				  }

				//  m_pPage->setCheck(true);
			  }



			  m_stream.seek(m_nBeginPos, CommonLib::soFromCurrent);
			  return true;
		  }

		  bool open(int64 nPageHeader, uint32 nBeginPos, bool bReopen = false)
		  {
			 m_nBeginPos = nBeginPos;
			  if(!m_pPage.get() || m_pPage->getAddr() != nPageHeader || bReopen)
			  {
				  m_nPageHeader = nPageHeader;
				  m_pPage = m_pTran->getFilePage(m_nPageHeader, m_nPageSize, true);
				  if(!m_pPage.get())
					  return false; //TO DO Log;
			  }
			  
			  assert(m_nBeginPos < m_pPage->getPageSize());
			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_pPage->getPageSize(), m_bCheckCRC);
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

				//  m_pPage->setCheck(true);

				  if(m_nBeginPos != 0)
					 m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);
			  }
			  else
				  m_stream.seek(m_nBeginPos, CommonLib::soFromBegin);

			


			 
				return true;

		  }

		  virtual void read_bytes(byte* buffer, uint32 size)
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
		  virtual void read_inverse(byte* buffer, uint32 size)
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


		  void readStream(IStream *pStream, bool bAttach)
		  {
			  if(bAttach)
				  return;

			  IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
			  if(pMemStream)
			  {
				  uint32 nStreamSize = readIntu32();
				
				  if(nStreamSize)
				  {
					   pMemStream->resize(nStreamSize);
					  read(pMemStream->buffer() + pStream->pos(), nStreamSize);
				  }
			  }
		  }
		  bool SaveReadStream(IStream *pStream, bool bAttach)
		  {
			   if(bAttach)
				   return false;

			  IMemoryStream *pMemStream = dynamic_cast<IMemoryStream *>(pStream);
			  if(!pMemStream)
				  return false;
			  uint32 nStreamSize = 0;
			  if(!save_read(nStreamSize))
				  return false;			 
					
			  pMemStream->resize(nStreamSize);
			 return save_read(pMemStream->buffer() + pStream->pos(), nStreamSize);
			 
		  }

		  bool NextPage(int64 nNextPage)
		  {
			  if(nNextPage == -1)
				  return false; // TO DO Log

			 m_pPage = m_pTran->getFilePage(nNextPage, m_nPageSize, true);
			  if(!m_pPage.get())
				  return false;  // TO DO Log

			  m_stream.attachBuffer(m_pPage->getRowData(), m_pPage->getPageSize());

			  if(m_nObjectPage != 0 && m_nSubObjectPage != 0)
			  {
				  sFilePageHeader header(m_stream, m_pPage->getPageSize(), m_bCheckCRC);
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

				//  m_pPage->setCheck(true);
			  }



			return true;
		  }

	public:
		FilePagePtr m_pPage;
		_TStorage* m_pTran;
		uint32 m_nBeginPos;
		int64 m_nPageHeader;
		int64 m_nEndPage;
		uint32 m_nEndPos;
		uint32 m_nPageSize;
    	CommonLib::FxMemoryReadStream m_stream;
		uint16 m_nObjectPage;
		uint16 m_nSubObjectPage; 
		bool m_bCheckCRC;
 
	};

	typedef TReadStreamPage<IFilePage> ReadStreamPage;
	COMMON_LIB_REFPTR_TYPEDEF(ReadStreamPage);
}

#endif