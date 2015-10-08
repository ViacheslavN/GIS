#ifndef _EMBEDDED_DATABASE_SHAPE_VALUE_H_
#define _EMBEDDED_DATABASE_SHAPE_VALUE_H_
#include "CommonLibrary/String.h"
#include "IDBTransactions.h"
namespace embDB
{




	class ShapeIterator
	{
		public:
			ShapeIterator(embDB::IDBTransactions* m_pTran, int64 nPageHeader, uint32 nSize, uint32 nBeginPos) :
				m_pTran(m_pTran), m_nPageHeader(nPageHeader), m_nNextPage(-1)
			{

			}
			~ShapeIterator()
			{

			}

		public:
			embDB::IDBTransactions* m_pTran;
			uint32 m_nSize;
			uint32 m_nBeginPos;
			int64 m_nPageHeader;
			int64 m_nNextPage;
	};

	struct sShapeVal  
	{
		uint32 m_nSize;
		uint32 m_nBeginPos;
		int64 m_nPage;

		sShapeVal() 
		{}


	};

	struct CompStringValue  
	{	


		int StrValcmp(const char *pszLeft, const char *pszRight) const
		{
			while ( *pszLeft && *pszRight && *pszLeft == *pszRight )
				++pszLeft, ++pszRight;
			return *pszLeft - *pszRight;
		}

		bool LE(const sStringVal& _Left, const sStringVal& _Right) const
		{
			return StrValcmp((const char *)_Left.m_pBuf, (const char *)_Right.m_pBuf) < 0;
		}
		bool EQ(const sStringVal& _Left, const sStringVal& _Right) const
		{
			if(_Left.m_nLen != _Right.m_nLen)
				return false;

			return StrValcmp((const char *)_Left.m_pBuf, (const char *)_Right.m_pBuf) == 0;
		}

	};


}

#endif