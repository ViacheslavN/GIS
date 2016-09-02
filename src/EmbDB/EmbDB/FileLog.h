#ifndef _EMBEDDED_DATABASE_BP_FILELOGGER_H_
#define _EMBEDDED_DATABASE_BP_FILELOGGER_H_
#include "CommonLibrary/general.h"
#include "CommonLibrary/File.h"
#include "LogBase.h"
namespace embDB
{

	class CFileLogger : public LogBase
	{
	public:
		CFileLogger(const wchar_t* pszFileName, bool bNew = false, bool bMultiThread = true);
		~CFileLogger();

		virtual eLogMode GetLogMode() const {return lmFile;}

		 

		virtual void error(const wchar_t *pszFormat, ...);
		virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...);
		virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg);
	private:
		void writeLog( const wchar_t *pszMsg, uint32 nSize);
	private:
		CommonLib::CFile m_File;
		static const uint32 n_msg_buf_size = 1024;

		byte m_sMsgBuffer[n_msg_buf_size];
	};

}

#endif