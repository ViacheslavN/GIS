#ifndef _EMBEDDED_DATABASE_BLOB_VALUE_H_
#define _EMBEDDED_DATABASE_BLOB_VALUE_H_
#include "embDBInternal.h"
namespace embDB
{





	struct sBlobVal  
	{
		uint32 m_nSize;
		uint32 m_nOldSize;
		uint32 m_nBeginPos;
		int64 m_nPage;
		CommonLib::CBlob m_blob;
		bool m_bChange;
		sBlobVal() : m_nSize(0), m_nBeginPos(0), m_nPage(-1), m_bChange(false), m_nOldSize(0)
		{}


	};

}

#endif