#include "stdafx.h"
#include "str_t.h"
#include "str_buffer.h"
#include <stdio.h>
#ifdef _WIN32
#include <oleauto.h>
#include <malloc.h>
#endif
#include <assert.h>
#include <string>
#include <sstream>
#include "sprintf.h"
#include "caseutils.h"
#include "multibyte.h"
#ifdef ANDROID
  #include <android/log.h>
#endif
#define LOADSTRING_BUFFER_MAX_SIZE 1024

namespace CommonLib
{

//#if _MSC_VER>=1300
void str_t::setDefaultAllocator(alloc_t* alloc)
{
  g_customAlloc = alloc;
}

void str_t::changeAllocator(alloc_t* alloc)
{
  customAlloc_ = alloc;

  if(!buffer_)
    return;

  str_buffer* buffer = buffer_;
  buffer_ = str_buffer::make(alloc, *buffer);
  buffer->safeRelease();
}
//#endif

str_t::str_t(alloc_t* customAlloc)
  : buffer_(NULL)
  , customAlloc_(customAlloc ? customAlloc : g_customAlloc)
{
}

str_t::str_t(const char *_str, int count, alloc_t* customAlloc)
  : buffer_(NULL)
  , customAlloc_(customAlloc ? customAlloc : g_customAlloc)
{
  if(_str == NULL || count == 0)
    return;

  if(*_str == 0)
    return;

  buffer_ = str_buffer::make(customAlloc_, _str, count);
}

str_t::str_t(const wchar_t *_str, int count, alloc_t* customAlloc)
  : buffer_(NULL)
  , customAlloc_(customAlloc ? customAlloc : g_customAlloc)
{
  if(_str == NULL || count == 0)
    return;

  if(*_str == 0)
    return;

  buffer_ = str_buffer::make(customAlloc_, _str, count);
}

#ifdef _WIN32
str_t::str_t(const BSTR _str, alloc_t* customAlloc)
  : buffer_(NULL)
  , customAlloc_(customAlloc ? customAlloc : g_customAlloc)
{
  if(_str != NULL && wcslen(static_cast<const wchar_t*>(_str)) != 0)
    buffer_ = str_buffer::make(customAlloc_, static_cast<const wchar_t*>(_str));
}
#endif

str_t::str_t(const str_t& _str)
  : buffer_(NULL)
  , customAlloc_(_str.customAlloc_)
{
  if(_str.buffer_ == NULL)
    return;

  if(_str.buffer_->isShareable())
  {
    buffer_ = _str.buffer_;
    buffer_->safeAddRef();
  }
  else
    buffer_ = str_buffer::make(customAlloc_, *_str.buffer_);
}

str_t::~str_t()
{
  buffer_->safeRelease();
}

str_t& str_t::operator =(const char *_str)
{
  if(buffer_ == NULL && _str == NULL)
    return *this;

  //if(buffer_ != NULL && buffer_->isCharBufferActual())
  //{
  //  if(buffer_->charBegin() == _str)
  //    return *this;
  //}

  buffer_->safeRelease();
  buffer_ = str_buffer::make(customAlloc_, _str);

  return *this;
}

str_t& str_t::operator =(const wchar_t *_str)
{
  if((buffer_ == NULL && _str == NULL))// || cwstr() == _str)
    return *this;

  buffer_->safeRelease();
  buffer_ = str_buffer::make(customAlloc_, _str);

  return *this;
}

str_t& str_t::operator =(const str_t& _str)
{


  if(this == &_str)
    return *this;

  if(buffer_ == _str.buffer_)
    return *this;

  if(buffer_ == NULL && _str.isEmpty())
    return *this;

  buffer_->safeRelease();
  buffer_ = NULL;

  if(_str.buffer_ == NULL)
    return *this;
  customAlloc_ = _str.customAlloc_;
  if(_str.buffer_->isShareable())
  {
    buffer_ = _str.buffer_;
    buffer_->safeAddRef();
  }
  else
  {
    buffer_ = str_buffer::make(customAlloc_, _str.cwstr());
  }
  return *this;
}

bool str_t::isEmpty() const
{
  if(buffer_ == NULL)
    return true;

  return buffer_->isEmpty();
}

size_t str_t::length() const
{
  if(buffer_ == NULL)
    return 0;
  return buffer_->length();
}
const twchar* str_t::cwstr() const
{
  if(buffer_ == NULL)
    buffer_ = str_buffer::make(customAlloc_);

  return buffer_->begin();
}
size_t str_t::capacity() const
{
  if(buffer_ == NULL)
    return 0;
  return buffer_->capacity();
}

const char* str_t::cstr() const
{
  if(buffer_ == NULL)
    buffer_ = str_buffer::make(customAlloc_);

  return buffer_->charBegin();
}



#if 0
const TCHAR* str_t::ctstr() const
{
#ifdef _UNICODE
  return cwstr();
#else
  return cstr();
#endif
}
#endif

#ifdef _WIN32
BSTR str_t::bstr() const
{
  //return static_cast<BSTR>(const_cast<wchar_t*>(cwstr()));
  if(buffer_ == NULL)
    buffer_ = str_buffer::make(customAlloc_);

  return buffer_->bstr();
}
#endif

wchar_t* str_t::wstr()
{
  exclusive();

  wchar_t* res = const_cast<wchar_t*>(cwstr());
  buffer_->enableSharing(false);

  return res;
}

#ifdef _WIN32
BSTR str_t::copyBSTR() const
{
  return ::SysAllocString(cwstr());
}
#endif

void str_t::finishExternalChanges()
{
  exclusive();
  buffer_->enableSharing(true);
}

const wchar_t& str_t::operator [](size_t _index) const
{
  assert (_index < capacity ());
  return *(cwstr() + _index);
}

wchar_t& str_t::operator [](size_t _index)
{
  assert (_index < capacity ());
  return *(wstr() + _index);
}

wchar_t str_t::wcharAt(size_t _index) const
{
  return (wchar_t)operator [](_index);
}

#ifdef _WIN32
char str_t::charAt(size_t _index) const
{
  assert (_index < capacity ());
  char chr;
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  ::WideCharToMultiByte(cp, 0, (cwstr() + _index), 1, &chr, 1, NULL, NULL);

  return chr;
}
#endif //_WIN32

#if 0
TCHAR str_t::tcharAt(int _index) const
{
#ifdef _UNICODE
  return wcharAt(_index);
#else
  return charAt(_index);
#endif
}
#endif

bool str_t::operator ==(const char *_str) const
{
  return compare(_str, true) == 0;
}

bool str_t::operator ==(const wchar_t *_str) const
{
  if(_str == 0 || _str[0] == 0 || isEmpty())
    return (_str == 0 || _str[0] == 0) == isEmpty();

  return wcscmp(_str, buffer_->begin()) == 0;
  //return compare(_str, true) == 0;
}

bool str_t::operator ==(const str_t& _str) const
{
  if(_str.isEmpty() || isEmpty())
    return _str.isEmpty() == isEmpty();

  return wcscmp(_str.buffer_->begin(), buffer_->begin()) == 0;
  //return compare(_str, true) == 0;
}

bool str_t::operator !=(const char *_str) const
{
  return !operator ==(_str);
  //return compare(_str, true) != 0;
}

bool str_t::operator !=(const wchar_t *_str) const
{
  return !operator ==(_str);
  //return compare(_str, true) != 0;
}

bool str_t::operator !=(const str_t& _str) const
{
  return !operator ==(_str);
  //return compare(_str, true) != 0;
}

bool str_t::operator <(const char *_str) const
{
  return compare(_str, true) < 0;
}

bool str_t::operator <(const wchar_t *_str) const
{
  return compare(_str, true) < 0;
}

bool str_t::operator <(const str_t& _str) const
{
  return compare(_str, true) < 0;
}

bool str_t::operator >(const char *_str) const
{
  return compare(_str, true) > 0;
}

bool str_t::operator >(const wchar_t *_str) const
{
  return compare(_str, true) > 0;
}

bool str_t::operator >(const str_t& _str) const
{
  return compare(_str, true) > 0;
}

bool str_t::operator <=(const char *_str) const
{
  return compare(_str, true) <= 0;
}

bool str_t::operator <=(const wchar_t *_str) const
{
  return compare(_str, true) <= 0;
}

bool str_t::operator <=(const str_t& _str) const
{
  return compare(_str, true) <= 0;
}

bool str_t::operator >=(const char *_str) const
{
  return compare(_str, true) >= 0;
}

bool str_t::operator >=(const wchar_t *_str) const
{
  return compare(_str, true) >= 0;
}

bool str_t::operator >=(const str_t& _str) const
{
  return compare(_str, true) >= 0;
}

bool str_t::equals(const char *_str, bool _caseSensitive) const
{
  return compare(_str, _caseSensitive) == 0;
}

bool str_t::equals(const wchar_t *_str, bool _caseSensitive) const
{
  return compare(_str, _caseSensitive) == 0;
}

bool str_t::equals(const str_t& _str, bool _caseSensitive) const
{
  return compare(_str, _caseSensitive) == 0;
}

int str_t::compare(const char *_str, bool _caseSensitive) const
{
  if(_str == NULL)
    return isEmpty() ? 0 : 1;

  str_t str(_str);
  
  return compare(str.cwstr(), _caseSensitive);
}

int str_t::compare(const wchar_t *_str, bool _caseSensitive) const
{
  if(_str == NULL)
    return isEmpty() ? 0 : 1;

  if(isEmpty())
    return *_str == 0 ? 0 : -1;

  if(_caseSensitive)
    return wcscmp(cwstr(), _str);
  else
  {
    const wchar_t * str = cwstr();
    for(int c1, c2; *str && *_str; ++str, ++_str)
    {
      c1 = (int)CommonLib::toupper(*str);
      c2 = (int)CommonLib::toupper(*_str);
      if(c1 != c2)
        return c1 - c2;
    }
    return ((int)*str) - ((int)*_str);
    //return wcsicmp(cwstr(), _str);
  }
}

int str_t::compare(const str_t& _str, bool _caseSensitive) const
{
  if(_str.isEmpty())
    return isEmpty() ? 0 : 1;

  if(isEmpty())
    return -1;

  return compare(_str.cwstr(), _caseSensitive);
}

int str_t::find(const char *_str, size_t nStart) const
{
  if(_str == 0 || _str[0] == 0 || isEmpty())
    return -1;

  str_t str(_str);

  return find(str.cwstr(), nStart);
}

int str_t::find(const wchar_t *_str, size_t nStart) const
{
  if(_str == NULL || _str[0] == 0 || isEmpty())
    return -1;
  
  const wchar_t* str = wcsstr(cwstr() + nStart, _str);
  if(str == NULL)
    return -1;
  
  return (int)(str - cwstr());
}

int str_t::find(const str_t& _str, size_t nStart) const
{
  if(_str.isEmpty() || isEmpty())
    return -1;

  return find(_str.cwstr(), nStart);
}

#ifdef _WIN32
int str_t::find(char _chr, size_t nStart) const
{
  wchar_t chr;
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  ::MultiByteToWideChar(cp, 0, &_chr, 1, &chr, (int)sizeof(wchar_t));

  return find(chr, nStart);
}
#endif

int str_t::find(wchar_t _chr, size_t nStart) const
{
  if(isEmpty())
    return -1;
  
  const wchar_t* str = wcschr(cwstr() + nStart, _chr);
  if(str == NULL)
    return -1;
  
  return (int)(str - cwstr());
}

int str_t::findEOL(size_t _nStart) const
{
  wchar_t eol[3] = {0x0D, 0x0A, 0x00};
  int res = find(eol, _nStart);
  if (res != -1)
    return res;

  return find(eol + 1, _nStart);
}

#ifdef _WIN32
int str_t::reverseFind(char _chr) const
{
  wchar_t chr;
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  ::MultiByteToWideChar(cp, 0, &_chr, 1, &chr, (int)sizeof(wchar_t));

  return reverseFind(chr);
}
#endif

int str_t::reverseFind(wchar_t _chr) const
{
  const wchar_t* p = wcsrchr(cwstr(), _chr);

  return (p != NULL) ? (int)(p - cwstr()) : -1;
}

bool str_t::isFound(size_t* _pnPos, const char *_str, size_t _nStart) const
{
  int n = find(_str, _nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isFound(size_t* _pnPos, const wchar_t *_str, size_t _nStart) const
{
  int n = find(_str, _nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isFound(size_t* _pnPos, const str_t& _str, size_t _nStart) const
{
  int n = find(_str, _nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isEOLFound(size_t* _pnPos, size_t _nStart) const
{
  int n = findEOL(_nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isFound(size_t* _pnPos, char _chr, size_t _nStart) const
{
  int n = find(_chr, _nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isFound(size_t* _pnPos, wchar_t _chr, size_t _nStart) const
{
  int n = find(_chr, _nStart);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isReverseFound(size_t* _pnPos, char _chr) const
{
  int n = reverseFind(_chr);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

bool str_t::isReverseFound(size_t* _pnPos, wchar_t _chr) const
{
  int n = reverseFind(_chr);
  if(n >= 0)
    *_pnPos = (size_t)n;

  return n >= 0;
}

void str_t::clear()
{
  if(buffer_ == NULL)
    return;

  if(buffer_->isExclusive())
    buffer_->clear();
  else
  {
    buffer_->safeRelease();
    buffer_ = NULL;
  }
}

void str_t::reserve(size_t _len)
{
  if(_len == 0 || capacity() >= _len)
    return;

  if(buffer_ == NULL)
    buffer_ = str_buffer::make(customAlloc_);
  else
    exclusive();

  buffer_->reserve(_len);
}

void str_t::exclusive()
{
  if(buffer_ == NULL)
    return;

  if(buffer_->isExclusive())
    return;

  str_buffer* buffer = str_buffer::make(customAlloc_, *buffer_);
  buffer_->safeRelease();
  buffer_ = buffer;
}

str_t str_t::mid(size_t _nFirst, size_t _nCount) const
{
  size_t len = length();

  if(_nCount == ((size_t)-1)) 
    _nCount = len - _nFirst;
  
  if(_nFirst == 0 && _nCount >= len)
    return *this;

  str_t str;

  if(_nFirst >= len)
    return str;

  if(_nFirst + _nCount > len)
    _nCount = len - _nFirst;

  str.reserve(_nCount + 1);
  wchar_t* p = str.wstr();
  
  wcsncpy(p, cwstr() + _nFirst, _nCount);
  p[_nCount] = 0;

  return str;
}

str_t str_t::left(size_t _nCount) const
{
  return mid(0, _nCount);
}

str_t str_t::right(size_t _nCount) const
{
  size_t len = length();
  
  if(len > (size_t)_nCount)
    return mid(len - _nCount, _nCount);
  
  return *this;
}

str_t str_t::spanInclude(const str_t& _str) const
{
  size_t len = wcsspn(cwstr(), _str.cwstr());

  return (len != 0) ? mid(0, len) : str_t();
}

str_t str_t::spanExclude(const str_t& _str) const
{
  size_t len = wcscspn(cwstr(), _str.cwstr());
  
  return len ? mid(0, len) : str_t();
}

std::vector<str_t> str_t::split(const str_t& _spliter, int flags) const
{
  std::vector<str_t> v;

  size_t spliter_length = (flags & SM_BYCHARS) ? 1 : _spliter.length();
  const wchar_t* p = cwstr();
  const wchar_t* pend = cwstr() + length();
    
  while(p < pend)
  {
    const wchar_t* s;
      
    if(flags & SM_BYCHARS)
    {
      size_t i = wcscspn(p, _spliter.cwstr());
      if((p + i) >= pend)
        s = 0;
      else
        s = p + i;
    }
    else
      s = wcsstr(p, _spliter.cwstr());

    if(!s)
    {
      v.push_back(str_t(p));
      if(flags & SM_AUTOTRIM)
        v.back().trimAll();
      if((flags & SM_REMOVEEMPTY) && v.back().isEmpty())
        v.pop_back();
      break;
    }

    if(s == p)
    {
      if(!(flags & SM_REMOVEEMPTY))
        v.push_back(str_t());
    }
    else
    {
      v.push_back(mid(p - cwstr(), s - p));
      if(flags & SM_AUTOTRIM)
        v.back().trimAll();
      if((flags & SM_REMOVEEMPTY) && v.back().isEmpty())
        v.pop_back();
    }

    p = s + spliter_length;
  }

  if(p >= pend && v.size() != 0 && !(flags & SM_REMOVEEMPTY))
    v.push_back(str_t());

  return v;
}

str_t& str_t::upper()
{
  bool share = buffer_ == NULL ? false : buffer_->isShareable();
  
  toupper (wstr());

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return *this;
}

str_t& str_t::lower()
{
  bool share = buffer_ == NULL ? false : buffer_->isShareable();

  tolower (wstr());

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return *this;
}

#ifdef _WIN32
str_t& str_t::reverse()
{
  bool share = buffer_ == NULL ? false : buffer_->isShareable();

  _wcsrev(wstr());

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return *this;
}
#endif

#ifdef _WIN32
size_t str_t::replace(char _chFind, char _chReplace)
{
  if(_chFind == _chReplace)
    return 0;

  wchar_t wchFind;
  wchar_t wchReplace;
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  ::MultiByteToWideChar(cp, 0, &_chFind, 1, &wchFind, (int)sizeof(wchar_t));
  ::MultiByteToWideChar(cp, 0, &_chReplace, 1, &wchReplace, (int)sizeof(wchar_t));

  return replace(wchFind, wchReplace);
}
#endif

size_t str_t::replace(wchar_t _chFind, wchar_t _chReplace)
{
  if(_chFind == _chReplace)
    return 0;

  bool share = buffer_ == NULL ? false : buffer_->isShareable();

  size_t nCount = 0;

  for(wchar_t* p = wcschr(wstr(), _chFind); p != NULL; p = wcschr(p, _chFind), ++nCount)
    *p = _chReplace;

  return nCount;
}

size_t str_t::replace(const str_t& _strFind, const str_t& _strReplace)
{
  if(_strFind == _strReplace)
    return 0;

  bool share = buffer_ == NULL ? false : buffer_->isShareable();

  size_t nCount = 0;
  size_t lenFind = _strFind.length();
  size_t lenReplace = _strReplace.length();

  if(lenFind == lenReplace)
  {
    for(wchar_t* p = wcsstr(wstr(), _strFind.cwstr()); p != NULL; p = wcsstr(p, _strFind.cwstr()), ++nCount)
    {
      wcsncpy(p, _strReplace.cwstr(), lenReplace);
      p += lenFind;
    }
  }
  else
  {
    str_t res;
    const wchar_t* p = wcsstr(cwstr(), _strFind.cwstr());
    const wchar_t* p2 = cwstr();

    for(; p != NULL; p = wcsstr(p, _strFind.cwstr()), ++nCount)
    {
      res += mid(p2 - cwstr(), p - p2);
      res += _strReplace;
      p += lenFind;
      p2 = p;
    }

    if(nCount == 0)
      return 0;

    if(p < cwstr() + length())
      res += right((int)(cwstr() - p2 + length()));

    *this = res;
  }

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return nCount;
}

str_t& str_t::operator +=(wchar_t _chr)
{
  size_t len = length();
  reserve(len + 1);

  bool share = buffer_ == NULL ? false : buffer_->isShareable();

  wchar_t* p = wstr();
  p[len] = (wchar_t)_chr;
  p[len + 1] = 0;

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return *this;
}

str_t& str_t::operator +=(const char* _str)
{
  return (*this) += str_t(_str);
}

str_t& str_t::operator +=(const wchar_t* _str)
{
  if(_str == 0)
    return *this;
  
  size_t len = length();
  size_t len2 = wcslen(_str);

  if(len + len2 == 0 || len2 == 0)
    return *this;

  reserve(len + len2);

  bool share = (buffer_ == NULL) ? false : buffer_->isShareable();

  wchar_t* p = wstr();
  //wcscat(p, _str);
  memcpy(p + len, _str, (len2 + 1) * sizeof(wchar_t)); // with zero

  if(buffer_ != NULL)
    buffer_->enableSharing(share);

  return *this;

//  return (*this) += str_t(_str);
}

str_t& str_t::operator +=(const str_t& _str)
{
  if(_str.length() == 0)
    return *this;

  return operator +=(_str.cwstr());
  //size_t len = length();
  //size_t len2 = _str.length();
  //
  //if(len + len2 == 0)
  //  return *this;

  //reserve(len + len2);

  //bool share = buffer_ == NULL ? false : buffer_->isShareable();

  //wchar_t* p = wstr();
  //wcscat(p, _str.cwstr());

  //if(buffer_ != NULL)
  //  buffer_->enableSharing(share);

  //return *this;
}

str_t str_t::operator +(wchar_t _chr) const
{
  return str_t(*this) += _chr;
}

str_t str_t::operator +(const char* _str) const
{
  return str_t(*this) += _str;  
}

str_t str_t::operator +(const wchar_t* _str) const
{
  str_t res;
  size_t len = 0;
  if(!_str)
    len = wcslen(_str);
  res.reserve(length() + len);
  res += *this;
  res += _str;
  return res;
  //return str_t(*this) += _str;  
}

str_t str_t::operator +(const str_t& _str) const
{
  if(_str.length() == 0)
    return str_t(*this);
  return operator +(_str.cwstr());
  //return str_t(*this) += _str;
}

str_t operator +(wchar_t _chr, const str_t& _str)
{
  return (str_t() += _chr) += _str;
}

str_t operator +(const char* _str, const str_t& _str2)
{
  return str_t(_str) += _str2;
}

str_t operator +(const wchar_t* _str, const str_t& _str2)
{
  return str_t(_str) += _str2;
}

str_t& str_t::format(const char* _format, ...)
{
  va_list args;
  char* buffer;
  int len;

  va_start(args, _format);
#if defined(_WIN32) && !defined(_WIN32_WCE)
  len = _vscprintf(_format, args);
#else
  len = 1000;
#endif
  buffer = (char*)_alloca (len + 1); //new char[len + 1];
  vsprintf(buffer, _format, args);
  *this = buffer;
  
  //delete[] buffer;
  return *this;
}

str_t& str_t::format(const wchar_t* _format, ...)
{
  va_list args;
  wchar_t* buffer;
  int len;

  va_start(args, _format);
#if defined(_WIN32) && !defined(_WIN32_WCE)
  len = _vscwprintf(_format, args);
#else
  len = 1000;
#endif
  buffer = (wchar_t*)_alloca ((len + 1)* sizeof (wchar_t)); //new wchar_t[len + 1];
  vswprintf(buffer,
#ifndef _WIN32
		1000,
#endif
		_format, args);
  *this = buffer;
  //delete[] buffer;
  return *this;
}

str_t& str_t::format_c(const wchar_t* _format, ...)
{
  va_list args;
  //wchar_t* buffer;
  int len;

  va_start(args, _format);
  //std::wostringstream f;
  out_str_stream_count f1;
  len = __vosnprintf (f1, _format, args);
  reserve(f1.charCount);
  out_str_stream_buf f2(wstr(), f1.charCount + 1);
  len = __vosnprintf (f2, _format, args);
  //*this = f.str ().c_str ();
  return *this;
}

#ifdef _WIN32
str_t& str_t::loadString(unsigned int _id, HINSTANCE _instance)
{
  if(_instance == NULL)
    _instance = (HINSTANCE)::GetModuleHandle(NULL);

  wchar_t buffer[LOADSTRING_BUFFER_MAX_SIZE + 1];
  int n = ::LoadStringW(_instance, _id, buffer, LOADSTRING_BUFFER_MAX_SIZE);

  if(n > 0)
    *this = buffer;
  else
    clear();

  return *this;
}
#endif

#if 0
str_t& str_t::trimLeft()
{
  exclusive();
  int idx = 0;
  size_t max = length();
  while (idx < ((int)max) && _istspace(tcharAt(idx)))
    idx++;

  if (idx != 0)
  {
    str_t res = mid(idx);
    *this = res;
  }
  return *this;
}

str_t& str_t::trimRight()
{
  exclusive();
  int idx = (int)length() - 1;
  while (idx >= 0 && _istspace(tcharAt(idx)))
    idx--;
  if (idx != ((int)length()) - 1)
  {
    str_t res = left(idx + 1);
    *this = res;
  }
  return *this;
}

#else

str_t& str_t::trimLeft()
{
  if(NULL == buffer_)
    return *this;

  exclusive();

  wchar_t *pstr = buffer_->begin();
  size_t len = wcslen (pstr);
  size_t nbefore = 0;
  size_t nafter = 0;
  wchar_t *ptmp = pstr;
  for (; iswspace (*ptmp++);)
    nbefore++;
  len -= nbefore;
  if (nbefore)
    memmove (pstr, pstr + nbefore, len * sizeof (wchar_t));
  pstr[len] = L'\0';

  buffer_->flush ();
  return *this;
}

str_t& str_t::trimRight()
{
  if(NULL == buffer_)
    return *this;

  exclusive();

  wchar_t *pstr = buffer_->begin();
  size_t len = wcslen (pstr);
  size_t nafter = 0;
  for (size_t ix = len - 1; ix >= 0; ix --)
    {
      if (iswspace(pstr[ix]))
        nafter++;
      else
        break;
    }
  pstr[len - nafter] = L'\0';

  buffer_->flush ();
  return *this;
}
#endif 
str_t& str_t::trimAll()
{
  if(NULL == buffer_)
    return *this;

  exclusive();

  wchar_t *pstr = buffer_->begin();
  size_t len = wcslen (pstr);
  size_t nbefore = 0;
  size_t nafter = 0;
  wchar_t *ptmp = pstr;
  for (; iswspace (*ptmp++);)
    nbefore++;
  for (size_t ix = len - 1; ix > nbefore; ix --)
    {
      if (iswspace(pstr[ix]))
        nafter++;
      else
        break;
    }
  len -= (nbefore + nafter);
  if (nbefore)
    memmove (pstr, pstr + nbefore, len * sizeof (wchar_t));
  pstr[len] = L'\0';

  buffer_->flush ();
  return *this;
}
 
bool str_t::split(char _separator, str_t& _left, str_t& _right) const
{
  char buf[2];
  buf[0] = _separator;
  buf[1] = 0;

  return split(buf, _left, _right);
}

bool str_t::split(wchar_t _separator, str_t& _left, str_t& _right) const
{
  wchar_t buf[2];
  buf[0] = (wchar_t)_separator;
  buf[1] = 0;

  return split(buf, _left, _right);
}

bool str_t::split(const char* _separator, str_t& _left, str_t& _right) const
{
  int n = find(_separator);
  if(n < 0)
    return false;

  _left = left(n);
  _right = mid(n + strlen(_separator));

  return true;
}

bool str_t::split(const wchar_t* _separator, str_t& _left, str_t& _right) const
{
  int n = find(_separator);
  if(n < 0)
    return false;

  _left = left(n);
  _right = mid(n + wcslen(_separator));

  return true;
}

int str_t::importFromUTF8 (const char *utf)
{
  if (NULL == utf)
    return false;
  size_t srcByteCount = strlen (utf);

  if(srcByteCount > 2 && utf[0] == 0xEF && utf[1] == 0xBB && utf[2] == 0xBF)
  {
    utf += 3;
    srcByteCount -= 3;
  }

  int dstCharCount = dest_mbslen ((unsigned char*)utf);
  
  if (buffer_)
    buffer_->safeRelease();
  buffer_ = str_buffer::make(customAlloc_, dstCharCount);
  
  int dstByteCount = dstCharCount * sizeof (wchar_t); 

  unsigned char *lsrc = (unsigned char *)utf;
  unsigned char **psrc = (unsigned char **)&lsrc;
  wchar_t *ldst = wstr ();
  int ret = dest_mbsnrtowcs (ldst, psrc, srcByteCount, dstByteCount);
  wstr ()[dstCharCount] = 0;
  return ret;
}

int str_t::exportToUTF8 (char *utf, int maxbuf) const 
{
  assert (maxbuf > 0);
  const wchar_t *tmp = cwstr ();
  int len = (int)dest_wcsmblen (tmp);
  if (len >= maxbuf)
    len = maxbuf - 1;
  wchar_t **ptr = (wchar_t **)&tmp;
  int ret = dest_wcsnrtombs ((unsigned char*)utf, ptr, wcslen (tmp), len + 1);
  utf[len] = 0;
  return ret;
}

int 
str_t::calcUTF8Length (void) const
{
  const wchar_t *tmp = cwstr ();
  return (int)dest_wcsmblen (tmp);
}
}
