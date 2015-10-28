#ifndef _EMBEDDED_DATABASE_STRING_PROXY_H_
#define _EMBEDDED_DATABASE_STRING_PROXY_H_
#include "CommonLibrary/String.h"
namespace embDB
{

 

	struct sFixedStringVal  
	{
		byte *m_pBuf;
		uint32 m_nLen;

		sFixedStringVal() : m_pBuf(0), m_nLen(0)
		{}

		
	};


	struct sStringVal  
	{
		byte *m_pBuf;
		uint32 m_nLen;
		uint32 m_nOldLen;
		int32 m_nPos;
		int64 m_nPage;
		bool m_bChange;

		sStringVal() : m_pBuf(0), m_nLen(0), m_nPos(0), m_nPage(-1), m_bChange(true), m_nOldLen(0)
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

		bool LE(const sFixedStringVal& _Left, const sFixedStringVal& _Right) const
		{
			return StrValcmp((const char *)_Left.m_pBuf, (const char *)_Right.m_pBuf) < 0;
		}
		bool EQ(const sFixedStringVal& _Left, const sFixedStringVal& _Right) const
		{
			if(_Left.m_nLen != _Right.m_nLen)
				return false;

			 return StrValcmp((const char *)_Left.m_pBuf, (const char *)_Right.m_pBuf) == 0;
		}
		 
	};
	

}

#endif