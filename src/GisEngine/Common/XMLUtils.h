#ifndef _LIB_GIS_ENGINE_COMMON_XML_UTILS_H_
#define _LIB_GIS_ENGINE_COMMON_XML_UTILS_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace GisCommon
	{
		void utf8_to_utf16(const char* str8, CommonLib::CString &);
		void utf16_to_utf8(const CommonLib::CString& str16, std::vector<char>& str8);
		void utf16_to_utf8(const wchar_t* str16, std::vector<char>& str8);

		void blob_to_string(const CommonLib::CBlob& blob, CommonLib::CString& str);
		void string_to_blob(const CommonLib::CString& str, CommonLib::CBlob& blob);
	}
}

#endif