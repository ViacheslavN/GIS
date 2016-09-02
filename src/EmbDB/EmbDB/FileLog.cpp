#include "stdafx.h"
#include "FileLog.h"
#include "CommonLibrary/multibyte.h"
namespace embDB
{

	CFileLogger::CFileLogger(const wchar_t* pszFileName, bool bNew, bool bMultiThread) : 
		LogBase(bMultiThread)
	{
		
		m_File.openFile(pszFileName, bNew ? CommonLib::ofmCreateNew : CommonLib::ofmOpenAlways,  CommonLib::arWrite,
			 CommonLib::smNoMode);
		if(!bNew)
			m_File.setFilePos(0, CommonLib::soFromEnd);

 
	}
	CFileLogger::~CFileLogger()
	{

	}
 
	
	void CFileLogger::error(const wchar_t *pszFormat, ...)
	{
		va_list args;
		va_start(args, pszFormat);

		uint32 len =  _vscwprintf(pszFormat, args);

		if(len != 0)
		{
			wchar_t* buffer= (wchar_t*)_alloca ((len + 1)* sizeof (wchar_t)); 
			vswprintf(buffer, pszFormat, args);
			writeLog(buffer, len);
		}

		va_end(args);
	}
	void CFileLogger::log(uint32 nLevel, const wchar_t *pszFormat, ...)
	{
		if(m_nLogLevel < nLevel)
			return;

		va_list args;
		va_start(args, pszFormat);

		uint32 len =  _vscwprintf(pszFormat, args);

		if(len != 0)
		{
			wchar_t* buffer= (wchar_t*)_alloca ((len + 1)* sizeof (wchar_t)); 
			vswprintf(buffer, pszFormat, args);
			 writeLog(buffer, len);
		}

		va_end(args);
	}
	void CFileLogger::log_msg(uint32 nLevel, const wchar_t *pszMsg)
	{
		if(m_nLogLevel < nLevel)
			return;

		 writeLog(pszMsg, wcslen(pszMsg));
	}

	void CFileLogger::writeLog( const wchar_t *pszMsg, uint32 nSize)
	{

		const uint32 nMaxRow = n_msg_buf_size/4   - 4;
		uint32 nOffset = 0;
		while(nSize > 0)
		{
			uint32 nWriteSize =  nSize < nMaxRow ?  nSize : nSize - nMaxRow;
			nSize -= nWriteSize;

			 uint32 utf8Len = (uint32)CommonLib::dest_wcsmblen (pszMsg, nWriteSize);
			 assert(!(utf8Len > nMaxRow));
			  const wchar_t* pszMsg_o = pszMsg + nOffset;
			  wchar_t **ptr = (wchar_t **)&pszMsg_o;
			 uint32 ret = (uint32)CommonLib::dest_wcsnrtombs ((unsigned char*)m_sMsgBuffer, ptr, nWriteSize, nWriteSize);
			 if(nSize == 0)
			 {
				 m_sMsgBuffer[nWriteSize] = '\n';
				 nWriteSize += 1;
			 }


			 m_File.writeFile(m_sMsgBuffer, nWriteSize);
		}
	}
}