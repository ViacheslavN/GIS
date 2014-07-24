#ifndef _LIB_COMMON_LIBRARY_PORTABLE_SNPRINTF_H_
#define _LIB_COMMON_LIBRARY_PORTABLE_SNPRINTF_H_

#include <sstream>

namespace CommonLib
{

struct out_str_stream
{
  virtual out_str_stream& operator << (wchar_t ch) = 0;
  virtual out_str_stream& operator << (const wchar_t* str) = 0;
};

struct out_str_stream_count : public out_str_stream
{
  size_t charCount;
  out_str_stream_count() : charCount(0) {}
  virtual out_str_stream& operator << (wchar_t /*ch*/)
  {
    ++charCount;
    return *this;
  }
  virtual out_str_stream& operator << (const wchar_t* str)
  {
    charCount += wcslen(str);
    return *this;
  }
};

struct out_str_stream_buf : public out_str_stream
{
  size_t charCount;
  wchar_t* buffer;
  size_t bufSize;
  out_str_stream_buf(wchar_t* buf, size_t size)
    : charCount(0)
    , buffer(buf)
    , bufSize(size)
  {
    buffer[bufSize - 1] = 0;
  }
  virtual out_str_stream& operator << (wchar_t ch)
  {
    if(charCount < (bufSize-1))
      buffer[charCount++] = ch;
    return *this;
  }
  virtual out_str_stream& operator << (const wchar_t* str)
  {
    if(!str)
      return *this;
    for(const wchar_t* p = str; *p != 0; ++p)
      *this << *p;
    return *this;
  }
};

int __sprintf(wchar_t *buf, int nbuf, const wchar_t *fmt, ...);
int __vsnprintf(wchar_t *buf, int nbuf, const wchar_t *fmt, va_list args);
int __vosnprintf(out_str_stream/*std::wostringstream*/ &f, const wchar_t *fmt, va_list args);


double __atof (const char *str);
double __wtof (const wchar_t *str);
#ifdef atof
#undef atof
#endif
#ifdef _atof
#undef _atof
#endif
#ifdef wtof
#undef wtof
#endif
#ifdef _wtof
#undef _wtof
#endif
#define atof decore::__atof
#define _atof decore::__atof
#define wtof decore::__wtof
#define _wtof decore::__wtof

}

#endif
