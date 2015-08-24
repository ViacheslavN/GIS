#ifndef _LIB_GIS_ENGINE_COMMON_XML_UTILS_H_
#define _LIB_GIS_ENGINE_COMMON_XML_UTILS_H_

#include "GisEngineCommon.h"

namespace GisEngine
{
	namespace GisCommon
	{
		void utf8_to_utf16(const char* str8, CommonLib::str_t &);
		std::vector<char> utf16_to_utf8(const CommonLib::str_t& str16);
		std::vector<char> utf16_to_utf8(const wchar_t* str16);

		CommonLib::str_t blob_to_string(const CommonLib::CBlob& blob);
		CommonLib::CBlob string_to_blob(const CommonLib::str_t& str);
	}
}

#endif