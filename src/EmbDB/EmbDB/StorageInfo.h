#ifndef _EMBEDDED_DATABASE_STORAGE_INFO_H_
#define _EMBEDDED_DATABASE_STORAGE_INFO_H_
#include "CommonLibrary/str_t.h"
namespace embDB
{
	class CStorage;

	class CStorageInfo
	{
	public:
		CStorageInfo(CStorage * pStorage);
		~CStorageInfo();

		void BeginWriteTransaction(const CommonLib::str_t& sTranFileName);
		void EndWriteTransaction(const CommonLib::str_t& sTranFileName);
		bool load();
		bool isDitry() const;
		const CommonLib::str_t& getWriteTransaction() const;
		int64 getBeginSize() const;
		bool save();
	private:
		bool m_bDirty;
		CStorage *m_pStorage;
		CommonLib::str_t m_sTranName;
		int64 m_nBeginSize;

	};
}

#endif