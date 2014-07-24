#ifndef _EMBEDDED_DATABASE_I_CURSOR_H_
#define _EMBEDDED_DATABASE_I_CURSOR_H_

#include "IField.h"

namespace embDB
{
	
	class ICursor
	{
	public:
		ICursor(){}
		virtual ~ICursor(){}
		virtual bool begin() = 0;
		virtual bool next() = 0;
		virtual IFieldVariant* getField(size_t nIdx) = 0;
		virtual size_t getFieldCnt() const = 0;
	};
}
#endif