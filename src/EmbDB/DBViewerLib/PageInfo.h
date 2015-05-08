#ifndef _DB_FILE_VIEWER_PAGE_INFO_H_
#define _DB_FILE_VIEWER_PAGE_INFO_H_
#include "../EmbDB/FilePageType.h"

namespace DbFileViewerLib
{
	class IPageInfo
	{
		IPageInfo(){}
		~IPageInfo(){}
		virtual  embDB::eObjectPageType getObjectPageType() const = 0;
		virtual int getPageSubType() const = 0;
		virtual uint32 getRawSize() const = 0;
		virtual int64 addr() const = 0;
	};
}

#endif