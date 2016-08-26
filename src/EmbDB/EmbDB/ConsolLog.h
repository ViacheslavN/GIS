#ifndef _EMBEDDED_DATABASE_BP_CONSOL_LOGGER_H_
#define _EMBEDDED_DATABASE_BP_CONSOL_LOGGER_H_
#include "CommonLibrary/general.h"
#include  "embDBInternal.h"

namespace embDB
{

	class CConsolLogger : public ILogger
	{
		public:
			CConsolLogger();
			~CConsolLogger();

			virtual eLogMode GetLogMode() const {return lmConsole;}

			virtual uint32 GetLogLevel() const;
			virtual void SetLogLevel(uint32 nLogLevel);

			virtual void error(const wchar_t *pszFormat, ...);
			virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...);
			virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg);

		private:
			uint32 m_nLogLevel;
	};

}

#endif