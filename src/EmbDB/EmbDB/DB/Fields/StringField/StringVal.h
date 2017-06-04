#ifndef _EMBEDDED_DATABASE_STRING_PROXY_H_
#define _EMBEDDED_DATABASE_STRING_PROXY_H_
#include "CommonLibrary/String.h"
#include "CommonLibrary/Variant.h"
namespace embDB
{

 

	struct sFixedStringVal  
	{
		byte *m_pBuf;
		uint32 m_nLen;
		wchar_t *m_pConvertBuf;

		sFixedStringVal() : m_pBuf(0), m_nLen(0), m_pConvertBuf(0)
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
		wchar_t *m_pConvertBuf;

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



	template<class _TBTree, class TString>
	class TStringVarConvertor
	{
	public:
		TStringVarConvertor() : m_pTree(nullptr)
		{}

		void convert(CommonLib::CVariant *pVar, const TString& value)
		{
			CommonLib::CString sVal(this->m_pAlloc);
			m_pTree->convert(value, sVal);

			pVar->setVal(sVal);
		}
		void Init(_TBTree *pTree, CommonLib::alloc_t *pAlloc)
		{
			m_pTree = pTree;
			m_pAlloc = pAlloc;
		}
	private:
		_TBTree *m_pTree;
		CommonLib::alloc_t *m_pAlloc;
	};
	

}

#endif