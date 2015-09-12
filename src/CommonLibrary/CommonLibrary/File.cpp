#include "stdafx.h"
#include "File.h"

namespace CommonLib
{
	namespace FileSystem
	{
		CString  FindFileName(const CString& sFullFileName)
		{
			if(!sFullFileName.isEmpty())
			{
				const int nLen = sFullFileName.length();
				for(int i = nLen - 1; i >= 0; --i)
				{
					if(sFullFileName[i] == L'\\' || sFullFileName[i] == L'/')
					  return	sFullFileName.right(nLen - (i + 1));
				}
			}	

			return sFullFileName;
		}
		CString  FindOnlyFileName(const CString& sFullFileName)
		{
			CString sFileName = FindFileName(sFullFileName);
			if(!sFileName.isEmpty())
			{
				const int nLen = sFileName.length();
				for(int i = nLen - 1; i >= 0; --i)
				{
					if(sFileName[i] == L'.')
						return	sFileName.left(i);
				}
			}	
			return sFileName;

		}
		CString  FindFileExtension(const CString& sFullFileName)
		{
			CString sFileName = FindFileName(sFullFileName);
			int i =  sFileName.reverseFind(L'.');
			if(i < 0)
				return CString();
			return sFileName.right(sFileName.length() - (i + 1));
		}
		CString  FindFilePath(const CString& sFullFileName)
		{
			if(!sFullFileName.isEmpty())
			{
				const int nLen = sFullFileName.length();
				for(int i = nLen - 1; i >= 0; --i)
				{
					if(sFullFileName[i] == L'\\' || sFullFileName[i] == L'/')
						return	sFullFileName.left(i + 1);
				}
			}
			return sFullFileName;
		}
	}
}