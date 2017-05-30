#ifndef _EMBEDDED_DATABASE_BP_TRAN_LOGGER_H_
#define _EMBEDDED_DATABASE_BP_TRAN_LOGGER_H_

namespace embDB
{
	class ITranLogger
	{
		public:
			ITranLogger(){}
			virtual ~ITranLogger(){}

			virtual uint32 GetLogLevel() const = 0;
			virtual void SetLogLevel(uint32 nLogLevel) = 0;

			virtual void error(const wchar_t *pszFormat, ...) = 0;
			virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...) = 0;
			virtual void log_msg(uint32 nLevel, const wchar_t *pszMsg) = 0;
	};
}

#endif