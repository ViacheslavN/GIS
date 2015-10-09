#ifndef _EMBEDDED_DATABASE_SHAPE_VALUE_H_
#define _EMBEDDED_DATABASE_SHAPE_VALUE_H_
#include "CommonLibrary/String.h"
#include "IDBTransactions.h"
namespace embDB
{





	struct sShapeVal  
	{
		//uint32 m_nSize;
		uint32 m_nBeginPos;
		int64 m_nPage;

		sShapeVal() 
		{}


	};
	 
}

#endif