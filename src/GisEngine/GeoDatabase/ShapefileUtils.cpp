#include "stdafx.h"
#include "ShapefileUtils.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace ShapefileUtils
		{
			CommonLib::str_t  NormalizePath(const CommonLib::str_t& path)
			{
				if(path.isEmpty())
					return path;

				CommonLib::str_t normalizedPath = path;
#ifdef WIN32
				normalizedPath.lower();
#endif
				if(normalizedPath[-1 + (int)normalizedPath.length()] != L'\\' &&
					normalizedPath[-1 + (int)normalizedPath.length()] != L'/')
				{
#ifdef WIN32
					normalizedPath += L"\\";
#else
					normalizedPath += L"/";
#endif
				}

				return normalizedPath;
			}
		}
	}
}