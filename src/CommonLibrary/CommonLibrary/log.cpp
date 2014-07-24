#include "stdafx.h"
#include "log.h"
#include "File.h"
#include "CSSection.h"
#include <stdio.h>
namespace CommonLib
{

static int debug_level = 1;
static CFile logFile;
static CSSection logSection;
void setLogLevel(int level)
{
	debug_level = level;
}

void InitLogFile()
{
	logFile.openFile("log.txt", ofmOpenAlways, arWrite, smRead);
}
void setLogFileName(const char* pszFileName)
{
	CSSection::scoped_lock lock(logSection);
	if(!logFile.isValid())
	{
		logFile.closeFile();
	}
	logFile.openFile(pszFileName, ofmOpenAlways, arWrite, smRead);
}
void setLogFileNameW(const wchar_t* pszFileName)
{
	CSSection::scoped_lock lock(logSection);
	if(!logFile.isValid())
	{
		logFile.closeFile();
	}
	logFile.openFile(pszFileName, ofmOpenAlways, arWrite, smRead);
}
void devent  (int level, char * format, ...)
{
	CSSection::scoped_lock lock(logSection);
	if(debug_level < level)
		return;
	if(!logFile.isValid()){
		InitLogFile();
		if(!logFile.isValid())
			return;
	}
	logFile.setFilePos(0, soFromEnd);
	va_list args;
	char* buffer;
	int len;

	va_start(args, format);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	len = _vscprintf(format, args);
#else
	len = 1000;
#endif
	buffer = (char*)_alloca (len + 1); //new char[len + 1];
	vsprintf(buffer, format, args);
	logFile.writeFile(buffer, len);
}
void dwevent  (int level, wchar_t * _format, ...)
{
	CSSection::scoped_lock lock(logSection);
	if(debug_level < level)
		return;
	if(!logFile.isValid()){
		InitLogFile();
		if(!logFile.isValid())
			return;
	}
	logFile.setFilePos(0, soFromEnd);
	 va_list args;
	wchar_t* buffer;
	int len;

	  va_start(args, _format);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	  len = _vscwprintf(_format, args);
#else
	  len = 1000;
#endif
	  size_t nSize = (len + 1)* sizeof (wchar_t);
	  buffer = (wchar_t*)_alloca (nSize); //new wchar_t[len + 1];
	  vswprintf(buffer,
#ifndef _WIN32
			1000,
#endif
		_format, args);
#ifdef _WIN32
	UINT cp = CP_THREAD_ACP;
#else // not _WIN32
	int n = dest_wcsmblen(buffer_);
	//wcstombs (NULL, buffer_, 0xffff);
	if (n < 0)
	  return NULL;
	charBuffer = NewCharBuffer(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
	wchar_t *tmp = buffer_;
	wchar_t **ptr = (wchar_t **)&tmp;
	dest_wcsnrtombs((unsigned char*) charBuffe_, ptr, len, n + 1);
#endif
 char* charBuffer = NULL;
 int n = ::WideCharToMultiByte(cp, 0, buffer, (int)len, charBuffer, 0, NULL, NULL);
 charBuffer =(char*)_alloca(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
  n = ::WideCharToMultiByte(cp, 0, buffer, (int)len, charBuffer, n, NULL, NULL);
  logFile.writeFile(charBuffer, n);

}


}
