#ifndef _EMBEDDED_DATABASE_BLOB_VALUE_H_
#define _EMBEDDED_DATABASE_BLOB_VALUE_H_
#include "embDBInternal.h"
namespace embDB
{





	struct sBlobVal  
	{
		uint32 m_nSize;
		uint32 m_nBeginPos;
		uint32 m_nOldSize;
		int64 m_nPage;
		byte* m_pBuf;
		bool m_bChange;
		sBlobVal() : m_nSize(0), m_nBeginPos(0), m_nPage(-1), m_pBuf(0), m_bChange(false), m_nOldSize(0)
		{}


	};

}

#endif