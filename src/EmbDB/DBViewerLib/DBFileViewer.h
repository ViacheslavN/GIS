#ifndef _DB_FILE_VIEWER_H_
#define _DB_FILE_VIEWER_H_
#include "stdafx.h"

#include "PageInfo.h"
namespace embDB
{
	class CStorage;
}
namespace DbFileViewerLib
{
	class CDBFileViewer
	{
		enum eFileType
		{
			eNotDef,
			eDB,
			eTranLog
		};
	public:
		CDBFileViewer();
		~CDBFileViewer();
		bool open(const CommonLib::str_t& sDbName,const CommonLib::str_t& sWorkingPath = "", const CommonLib::str_t& sPassword = "");
		bool open(embDB::CStorage* pStorage);
		bool close();
		bool isOpen() const;
		IPageInfo* GetPageInfo(int64 nAddr);
	private:
		bool ReadHeader();
	private:
		embDB::CStorage* m_pStorage;
		bool m_bInnerStorage;
		bool m_bOpen;
		CommonLib::simple_alloc_t m_alloc;
		eFileType m_FileType;
	};

}


#endif