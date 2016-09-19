#ifndef _EMBEDDED_DATABASE_BASE_LOGGER_H_
#define _EMBEDDED_DATABASE_BASE_LOGGER_H_
#include "CommonLibrary/general.h"
#include  "embDBInternal.h"
#include "CommonLibrary/LockObject.h"
namespace embDB
{


	class CLogHolder : public ILogger
	{
		public:
			CLogHolder();
			~CLogHolder();



			virtual eLogMode GetLogMode() const;

			virtual uint32 GetLogLevel() const;
			virtual void SetLogLevel(uint32 nLogLevel);

			virtual void error(const wchar_t *pszFormat, ...);
			virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...);
			virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg) ;

		private:

			ILogger *m_pLog;
	};

}

#endif