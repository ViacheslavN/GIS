#ifndef _EMBEDDED_DATABASE_STORAGE_INFO_H_
#define _EMBEDDED_DATABASE_STORAGE_INFO_H_
#include "CommonLibrary/String.h"
namespace embDB
{
	class CStorage;

	class CStorageInfo
	{
	public:
		CStorageInfo(CStorage * pStorage);
		~CStorageInfo();

		void BeginWriteTransaction(const CommonLib::CString& sTranFileName);
		void EndWriteTransaction(const CommonLib::CString& sTranFileName);
		bool load();
		bool isDitry() const;
		const CommonLib::CString& getWriteTransaction() const;
		int64 getBeginSize() const;
		bool save();
	private:
		bool m_bDirty;
		CStorage *m_pStorage;
		CommonLib::CString m_sTranName;
		int64 m_nBeginSize;

	};
}

#endif