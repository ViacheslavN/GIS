#ifndef _EMBEDDED_DATABASE_BP_CONSOL_LOGGER_H_
#define _EMBEDDED_DATABASE_BP_CONSOL_LOGGER_H_
#include "CommonLibrary/general.h"
#include  "embDBInternal.h"
#include "CommonLibrary/LockObject.h"
#include "LogBase.h"
namespace embDB
{

	class CConsolLogger : public LogBase
	{
		public:
			CConsolLogger(bool bMultiThread = true);
			~CConsolLogger();

			virtual eLogMode GetLogMode() const {return lmConsole;}

		 

			virtual void error(const wchar_t *pszFormat, ...);
			virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...);
			virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg);
 
			
	};

}

#endif