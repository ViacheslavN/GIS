#include "stdafx.h"
#include "File.h"

namespace CommonLib
{
	namespace FileSystem
	{
		str_t  FindFileName(const str_t& sFullFileName)
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
		str_t  FindOnlyFileName(const str_t& sFullFileName)
		{
			str_t sFileName = FindFileName(sFullFileName);
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
		str_t  FindFileExtension(const str_t& sFullFileName)
		{
			str_t sFileName = FindFileName(sFullFileName);
			int i =  sFileName.reverseFind(L'.');
			if(i < 0)
				return str_t();
			return sFileName.right(sFileName.length() - (i + 1));
		}
		str_t  FindFilePath(const str_t& sFullFileName)
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