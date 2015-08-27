#include "stdafx.h"
#include "XMLUtils.h"

namespace GisEngine
{
	namespace GisCommon
	{
		void utf8_to_utf16(const char* str8, CommonLib::str_t & str16)
		{
#ifdef WIN32
			int len = (int)strlen(str8);
			int size = ::MultiByteToWideChar(CP_UTF8, 0, str8, len, 0, 0);
			str16.reserve(size + 1);
			::MultiByteToWideChar(CP_UTF8, 0, str8, len, str16.wstr(), size);
			str16[size] = 0;
#endif
		}
		void  utf16_to_utf8(const CommonLib::str_t& str16, std::vector<char> &str8 )
		{
			 utf16_to_utf8(str16.cwstr(), str8);
		}
		void utf16_to_utf8(const wchar_t* str16, std::vector<char> &str8)
		{
#ifdef WIN32
			int len = (int)wcslen(str16);
			int size = ::WideCharToMultiByte(CP_UTF8, 0, str16, len, 0, 0, 0, 0);
			if(size == 0)
				return;

			str8.resize(size + 1);
			::WideCharToMultiByte(CP_UTF8, 0, str16, len, &str8[0], size, 0, 0);
			str8[size] = 0;
	#endif
		}

		void  blob_to_string(const CommonLib::CBlob& blob, CommonLib::str_t& result)
		{
			const unsigned char* cbuffer = blob.buffer();
			if(!cbuffer || blob.size() == 0)
				return;

			result.reserve(blob.size() * 2 + 1);
			for(size_t i = 0; i < blob.size(); ++i)
			{
				unsigned char sym = cbuffer[i];
				wchar_t left = sym >> 4;
				wchar_t right = sym & 0xF;
				result[(int)i * 2 + 0] = left <= 9 ? (left + L'0') : ((left - 10) + L'A');
				result[(int)i * 2 + 1] = right <= 9 ? (right + L'0') : ((right - 10) + L'A');
			}
			result[(int)blob.size() * 2] = 0;
		}
		void string_to_blob(const CommonLib::str_t& str, CommonLib::CBlob& blob)
		{
			int len = (int)str.length() / 2;
			if(len == 0)
				return;
			blob.resize(len);

			for(int i = 0; i < len; ++i)
			{
				wchar_t left = str[i * 2 + 0];
				wchar_t right = str[i * 2 + 1];
				int leftVal;
				int rightVal;

				if(left >= L'0' && left <= L'9')
					leftVal = left - L'0';
				else if(left >= L'A' && left <= L'F')
					leftVal = left - L'A' + 10;
				else
					CommonLib::str_t();

				if(right >= L'0' && right <= L'9')
					rightVal = right - L'0';
				else if(right >= L'A' && right <= L'F')
					rightVal = right - L'A' + 10;
				else
					CommonLib::str_t();

				blob[i] = (unsigned char)((leftVal & 0xF) << 4 | (rightVal & 0xF));
			}
		}
	}
}