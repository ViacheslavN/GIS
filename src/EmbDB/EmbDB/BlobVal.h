#ifndef _EMBEDDED_DATABASE_BLOB_VALUE_H_
#define _EMBEDDED_DATABASE_BLOB_VALUE_H_
#include "IDBTransactions.h"
namespace embDB
{





	struct sBlobVal  
	{
		uint32 m_nSize;
		uint32 m_nBeginPos;
		int64 m_nPage;
		byte* m_pBuf;

		sBlobVal() : m_nSize(0), m_nBeginPos(0), m_nPage(-1), m_pBuf(0)
		{}


	};

}

#endif