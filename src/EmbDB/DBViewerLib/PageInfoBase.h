#ifndef _DB_FILE_VIEWER_PAGE_INFO_BASE_H_
#define _DB_FILE_VIEWER_PAGE_INFO_BASE_H_

#include "PageInfo.h"
namespace embDB
{
	class CFilePage;
}
namespace DbFileViewerLib
{
	class CPageInfoBase : public embDB::IPageInfo
	{
	public:
		virtual  embDB::eObjectPageType getObjectPageType() const;
		virtual int getPageSubType() const;
		virtual uint32 getRawSize() const;
		virtual int64 addr() const;

		CPageInfoBase();
		CPageInfoBase(CFilePage *pFilePage);
		CPageInfoBase(embDB::eObjectPageType, uint32 nSubObj, uint32 nSize, int64 nAddr);
	private:
	};
}


#endif