#ifndef _LIB_COMMON_LIBRARY_STR_T_H_
#define _LIB_COMMON_LIBRARY_STR_T_H_

#include "general.h"


#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <wtypes.h>
#endif //_WIN32
#include "alloc_t.h"

#if _MSC_VER>=1300
#include <xhash>
#else
#define _HASH_SEED	(size_t)0xdeadbeef
#endif

#ifdef ANDROID
#include <wchar.h>
#endif

#define SM_BYCHARS 1
#define SM_REMOVEEMPTY 2
#define SM_AUTOTRIM 4

namespace CommonLib
{

class string_buffer;

class CString
{




  public:

//#if _MSC_VER>=1300 || defined(__IPHONE_3_1)
    static void setDefaultAllocator(alloc_t* alloc);
    void changeAllocator(alloc_t* alloc);
//#endif

  public:
    CString(alloc_t* customAlloc = 0);
    CString(const char *_str, int count = -1, alloc_t* customAlloc = 0);
    CString(const wchar_t *_str, int count = -1, alloc_t* customAlloc = 0);
#ifdef _WIN32
    CString(const BSTR _str, alloc_t* customAlloc = 0);
#endif
    CString(const CString& _str);

    ~CString();

    CString& operator =(const char *_str);
    CString& operator =(const wchar_t *_str);
    CString& operator =(const CString& _str);

    bool   isEmpty() const;
    size_t length() const;
	const char* cstr() const;
    const twchar* cwstr() const;
    size_t capacity() const;
    
    int importFromUTF8 (const char *utf);
    int exportToUTF8 (char *utf, int maxbuf) const;
    int calcUTF8Length (void) const;



#ifdef _UNICODE
    #define ctstr cwstr
#else
    #define ctstr cstr
#endif

#ifdef _WIN32
    BSTR bstr() const;
#endif
    wchar_t* wstr();
#ifdef _WIN32
    BSTR copyBSTR() const;
#endif
    void finishExternalChanges();

    const wchar_t& operator [](size_t _index) const;
    wchar_t& operator [](size_t _index);

    wchar_t   wcharAt(size_t _index) const;
    char     charAt(size_t _index) const;

#ifdef _UNICODE
    #define tcharAt wcharAt
#else
    #define tcharAt charAt
#endif

    bool operator ==(const char *_str) const;
    bool operator ==(const wchar_t *_str) const;
    bool operator ==(const CString& _str) const;

    bool operator !=(const char *_str) const;
    bool operator !=(const wchar_t *_str) const;
    bool operator !=(const CString& _str) const;

    bool operator <(const char *_str) const;
    bool operator <(const wchar_t *_str) const;
    bool operator <(const CString& _str) const;

    bool operator >(const char *_str) const;
    bool operator >(const wchar_t *_str) const;
    bool operator >(const CString& _str) const;

    bool operator <=(const char *_str) const;
    bool operator <=(const wchar_t *_str) const;
    bool operator <=(const CString& _str) const;

    bool operator >=(const char *_str) const;
    bool operator >=(const wchar_t *_str) const;
    bool operator >=(const CString& _str) const;

    bool equals(const char *_str, bool _caseSensitive = true) const;
    bool equals(const wchar_t *_str, bool _caseSensitive = true) const;
    bool equals(const CString& _str, bool _caseSensitive = true) const;

    int  compare(const char *_str, bool _caseSensitive = true) const;
    int  compare(const wchar_t *_str, bool _caseSensitive = true) const;
    int  compare(const CString& _str, bool _caseSensitive = true) const;

    int  find(const char *_str, size_t _nStart = 0) const;
    int  find(const wchar_t *_str, size_t _nStart = 0) const;
    int  find(const CString& _str, size_t _nStart = 0) const;

    int  findEOL(size_t _nStart = 0) const;
#ifdef _WIN32
    int  find(char _chr, size_t _nStart = 0) const;
#endif
    int  find(wchar_t _chr, size_t _nStart = 0) const;
#ifdef _WIN32
    int  reverseFind(char _chr) const;
#endif
    int  reverseFind(wchar_t _chr) const;

    bool isFound(size_t* _pnPos, const char *_str, size_t _nStart = 0) const;
    bool isFound(size_t* _pnPos, const wchar_t *_str, size_t _nStart = 0) const;
    bool isFound(size_t* _pnPos, const CString& _str, size_t _nStart = 0) const;
    bool isEOLFound(size_t* _pnPos, size_t _nStart = 0) const;

    bool isFound(size_t* _pnPos, char _chr, size_t _nStart = 0) const;
    bool isFound(size_t* _pnPos, wchar_t _chr, size_t _nStart = 0) const;

    bool isReverseFound(size_t* _pnPos, char _chr) const;
    bool isReverseFound(size_t* _pnPos, wchar_t _chr) const;

    void clear();
    void reserve(size_t _len);
    void exclusive();

    CString mid(size_t _nFirst, size_t _nCount = ((size_t)-1)) const;
    CString left(size_t _nCount) const;
    CString right(size_t _nCount) const;

    CString spanInclude(const CString& _str) const;
    CString spanExclude(const CString& _str) const;

    // Split by chars or string into array of pieces
    // Following flags can be used:
    // SM_BYCHARS - split by chars like spanExclude (default: by string)
    // SM_REMOVEEMPTY - remove empty entries (default: don't remove)
    // SM_AUTOTRIM - trim all entries (default: don't remove)
    std::vector<CString> split(const CString& _spliter, int flags = 0) const;

    CString& upper();
    CString& lower();
#ifdef _WIN32
    CString& reverse();
#endif
    CString& trimLeft();
    CString& trimRight();
    CString& trimAll();

#ifdef _WIN32
    size_t replace(char _chFind, char _chReplace);
#endif
    size_t replace(wchar_t _chFind, wchar_t _chReplace);
    size_t replace(const CString& _strFind, const CString& _strReplace);

    CString& operator += (wchar_t _chr);
    CString& operator += (const char* _str);
    CString& operator += (const wchar_t* _str);
    CString& operator += (const CString& _str);

    CString operator +(wchar_t _chr) const;
    CString operator +(const char* _str) const;
    CString operator +(const wchar_t* _str) const;
    CString operator +(const CString& _str) const;

    friend CString operator +(wchar_t _chr, const CString& _str);
    friend CString operator +(const char* _str, const CString& _str2);
    friend CString operator +(const wchar_t* _str, const CString& _str2);

    CString& format(const char* _format, ...);
    CString& format(const wchar_t* _format, ...);
    CString& format_c(const wchar_t* _format, ...);

#ifdef _WIN32
    CString& loadString(unsigned int _id, HINSTANCE _instance = NULL);
#endif

    bool split(char _separator, CString& _left, CString& _right) const;
    bool split(wchar_t _separator, CString& _left, CString& _right) const;

    bool split(const char* _separator, CString& _left, CString& _right) const;
    bool split(const wchar_t* _separator, CString& _left, CString& _right) const;

  private:
    mutable string_buffer* m_pBuffer;
    alloc_t* m_pCustomAlloc;
};

}

namespace stdext
{
  inline size_t hash_value(const CommonLib::CString& _Str)
  {
    typedef size_t _Strsize;

    size_t _Val = _HASH_SEED;
    _Strsize _Size = ::wcslen(_Str.cwstr());
    if (0 < _Size)
      {	// add one or more elements
      _Strsize _Stride = (_Size / 16) + 1;
      _Size -= _Stride;	// protect against _Size near _Str.max_size()
      for(_Strsize _Idx = 0; _Idx <= _Size; _Idx += _Stride)
        _Val += (size_t)_Str.cwstr()[_Idx];
      }
    return (_Val);
  }
}



#endif