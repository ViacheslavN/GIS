#ifndef _LIB_COMMON_LIBRARY_PORTABLE_STR_BUFFER_H_
#define _LIB_COMMON_LIBRARY_PORTABLE_STR_BUFFER_H_


#ifdef _WIN32
#include "windows.h"
#include "oleauto.h"
#endif

#include "multibyte.h"
#include "GeneralTypes.h"
#include "alloc_t.h"

#ifdef __IPHONE_3_1
#include <libkern/OSAtomic.h>
#endif
#ifdef ANDROID
  #include <assert.h>
#endif

namespace CommonLib
{

//typedef wchar_t* (*newWCharBufferFunction)(alloc_t* customAlloc, size_t _size);
//typedef void (*deleteWCharBufferFunction)(alloc_t* customAlloc, wchar_t* _buffer);
//typedef char* (*newCharBufferFunction)(alloc_t* customAlloc, size_t _size);
//typedef void (*deleteCharBufferFunction)(alloc_t* customAlloc, char* _buffer);
//typedef void (*deleteStrBufferFunction)(alloc_t* customAlloc, str_buffer* _buffer);
//
//wchar_t* newWCharBuffer(alloc_t* , size_t _size);
//void deleteWCharBuffer(alloc_t* , wchar_t* _buffer);
//char* newCharBuffer(alloc_t* , size_t _size);
//void deleteCharBuffer(alloc_t* , char* _buffer);
//void deleteStrBuffer(alloc_t* , str_buffer* _buffer);
//
//#if _MSC_VER >= 1300
//
//wchar_t* newCustomWCharBuffer(alloc_t* customAlloc, size_t _size);
//void deleteCustomWCharBuffer(alloc_t* customAlloc, wchar_t* _buffer);
//char* newCustomCharBuffer(alloc_t* customAlloc, size_t _size);
//void deleteCustomCharBuffer(alloc_t* customAlloc, char* _buffer);
//void deleteCustomStrBuffer(alloc_t* customAlloc, str_buffer* _buffer);
//
//#endif
//
//typedef struct
//{
//  newWCharBufferFunction pNewWCharBuffer_;
//  deleteWCharBufferFunction pDeleteWCharBuffer_;
//  newCharBufferFunction pNewCharBuffer_;
//  deleteCharBufferFunction pDeleteCharBuffer_;
//  deleteStrBufferFunction pDeleteStrBuffer_;
//}str_vtab;

#pragma init_seg(lib)
//str_vtab defaultStrVTab = {newWCharBuffer, deleteWCharBuffer, newCharBuffer, deleteCharBuffer, deleteStrBuffer};
//#if _MSC_VER >= 1300
//str_vtab customStrVTab = {newCustomWCharBuffer, deleteCustomWCharBuffer, newCustomCharBuffer, deleteCustomCharBuffer, deleteCustomStrBuffer};
alloc_t* g_customAlloc = NULL;
//#endif

class string_buffer
{
  public:
    static string_buffer* make(alloc_t* customAlloc);
    static string_buffer* make(alloc_t* customAlloc, int count);
    static string_buffer* make(alloc_t* customAlloc, const char* _str, int count = -1);
    static string_buffer* make(alloc_t* customAlloc, const wchar_t* _str, int count = -1);
    static string_buffer* make(alloc_t* customAlloc, const string_buffer& _str);

  private:
    // Constructors
    string_buffer(alloc_t* customAlloc);//, str_vtab* _vtab);
    string_buffer(alloc_t* customAlloc, int count);//, str_vtab* _vtab);
    string_buffer(alloc_t* customAlloc, const char* _str, int count);//, str_vtab* _vtab);
    string_buffer(alloc_t* customAlloc, const wchar_t* _str, int count);//, str_vtab* _vtab);
    string_buffer(alloc_t* customAlloc, const string_buffer& _str);//, str_vtab* _vtab);

  public:
    void safeAddRef();
    void safeRelease();

    bool     isEmpty() const;
    size_t   length() const;
    size_t   capacity() const;
    bool     isExclusive() const;
    bool     isShareable() const;

    const char* charBegin();
    wchar_t*    begin();
#ifdef _WIN32
    BSTR        bstr();
#endif
    void        clear();
    void        flush();
    void        reserve(size_t _len);
    void        enableSharing(bool _share);

    bool     isCharBufferActual();
#ifdef _WIN32
    bool     isBSTRActual();
#endif
  private:
    // Destructors
    ~string_buffer();
//    friend void deleteStrBuffer(alloc_t* customAlloc, str_buffer* _buffer);
//    friend void deleteCustomStrBuffer(alloc_t* customAlloc, str_buffer* _buffer);
    wchar_t* NewWCharBuffer(size_t _size);
    void     DeleteWCharBuffer(wchar_t* _buffer);
    char*    NewCharBuffer(size_t _size);
    void     DeleteCharBuffer(char* _buffer);
    void destroy();
  
  private:
    wchar_t* m_pBuffer;
    size_t   m_nCapacity;
    char*    m_pCharBuffer;
#ifdef _WIN32
    BSTR     m_bstr;
#endif
    bool     m_bIsCharBufferActual;
#ifdef _WIN32
    bool     m_bIsBSTRActual;
#endif
    bool     m_bIsShareable;

    //mutable str_vtab* vtab_;
#ifdef _WIN32_WCE
    mutable long refCount_;
#elif defined(__IPHONE_3_1)
		mutable volatile int32_t refCount_;
#else
    mutable long volatile refCount_;
#endif
    alloc_t* customAlloc_;
};

string_buffer* string_buffer::make(alloc_t* customAlloc)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(string_buffer))) string_buffer(customAlloc);//&customStrVTab);
  return new string_buffer(customAlloc);//&defaultStrVTab);
}

string_buffer* string_buffer::make(alloc_t* customAlloc, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(string_buffer))) string_buffer(customAlloc, count);//, &customStrVTab);
  return new string_buffer(customAlloc, count);//, &defaultStrVTab);
}

string_buffer* string_buffer::make(alloc_t* customAlloc, const char* _str, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(string_buffer))) string_buffer(customAlloc, _str, count);//, &customStrVTab);
  return new string_buffer(customAlloc, _str, count);//, &defaultStrVTab);
}

string_buffer* string_buffer::make(alloc_t* customAlloc, const wchar_t* _str, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(string_buffer))) string_buffer(customAlloc, _str, count);//, &customStrVTab);
  return new string_buffer(customAlloc, _str, count);//, &defaultStrVTab);
}

string_buffer* string_buffer::make(alloc_t* customAlloc, const string_buffer& _str)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(string_buffer))) string_buffer(customAlloc, _str);//, &customStrVTab);
  return new string_buffer(customAlloc, _str);//, &defaultStrVTab);
}

inline string_buffer::string_buffer(alloc_t* customAlloc)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    refCount_(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
    //vtab_(_vtab),
    customAlloc_(customAlloc)
{
}

inline string_buffer::string_buffer(alloc_t* customAlloc, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    refCount_(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
    //vtab_(_vtab),
    customAlloc_(customAlloc)
{
  if(count > 0)
  {
    reserve(count);
    m_pBuffer[0] = 0;
  }
}

inline string_buffer::string_buffer(alloc_t* customAlloc, const char* _str, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    refCount_(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
//    vtab_(_vtab),
    customAlloc_(customAlloc)
{
#if defined(__IPHONE_3_1) || defined(ANDROID)
  if(_str != NULL && count != 0)
  {
    size_t len = strlen(_str);
		reserve(len + 1);
    if(count > 0 && count < (int)len)
      len = (size_t)count;
		// Attention! Use UTF8 methods instead!
		// This code works correctly with english letters
		for (int i = 0; i < len; i++)
		  m_pBuffer[i] = _str[i];
    m_pBuffer[len] = 0;
	}
#endif //__IPHONE_3_1
#ifdef _WIN32
  if(_str != NULL && count != 0)
  {
    size_t len = strlen(_str);
    if(count > 0 && count < (int)len)
      len = (size_t)count;
#ifdef _WIN32_WCE
    UINT cp = CP_ACP;
#else
    UINT cp = CP_THREAD_ACP;
#endif //_WIN32_WCE

    int n = ::MultiByteToWideChar(cp, 0, _str, (int)len, m_pBuffer, 0);
    reserve(n);
    //buffer_ = (*vtab_->pNewWCharBuffer_)(n + 1);
  
    n = ::MultiByteToWideChar(cp, 0, _str, (int)len, m_pBuffer, n);
    
    m_pBuffer[n] = 0;
    //capacity_ = len;
  }
#endif //_WIN32
}

inline string_buffer::string_buffer(alloc_t* customAlloc, const wchar_t* _str, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    refCount_(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
//    vtab_(_vtab),
    customAlloc_(customAlloc)
{
  if(_str != NULL && count != 0)
  {
    size_t len = wcslen(_str);
    if(count > 0 && count < (int)len)
      len = (size_t)count;
    //buffer_ = (*vtab_->pNewWCharBuffer_)(len + 1);
    reserve(len);
    memcpy(m_pBuffer, _str, len * sizeof(wchar_t));
    m_pBuffer[len] = 0;
    //capacity_ = len;
  }
}

inline string_buffer::string_buffer(alloc_t* customAlloc, const string_buffer& _str)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    refCount_(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
//    vtab_(_vtab),
    customAlloc_(customAlloc)
{
  if(_str.m_pBuffer != NULL)
  {
    size_t len = wcslen(_str.m_pBuffer);
    //buffer_ = (*vtab_->pNewWCharBuffer_)(len + 1);
    reserve(len);
    //wcscpy(buffer_, _str.buffer_);
    memcpy(m_pBuffer, _str.m_pBuffer, len * sizeof(wchar_t));
    m_pBuffer[len] = 0;
    //capacity_ = len;
  }
}

inline void string_buffer::safeAddRef()
{
  if(this == NULL)
    return;

#ifdef _WIN32
  ::InterlockedIncrement(&refCount_);
#elif defined(__IPHONE_3_1)
	OSAtomicIncrement32(&refCount_);
#else
	/*assert(!"Implement it!"); */
  refCount_++;
#endif
}

inline void string_buffer::safeRelease()
{
  if(this == NULL)
    return;

#ifdef _WIN32
  if(::InterlockedDecrement(&refCount_) == 0)
#elif defined(__IPHONE_3_1)
	OSAtomicDecrement32(&refCount_); // return incorrect value
	if (0 == refCount_) // this is more safety
#else
 /* assert(!"Implement it!");*/ if (0 == (--refCount_))
#endif
	  destroy();//(*vtab_->pDeleteStrBuffer_)(this);
}

inline bool string_buffer::isEmpty() const
{
  return length() == 0;
}

inline size_t string_buffer::length() const
{
  if(m_pBuffer == NULL)
    return 0;

  if(m_pBuffer[0] == 0)
    return 0;

  return wcslen(m_pBuffer);
}

inline size_t string_buffer::capacity() const
{
  return m_nCapacity;
}

inline bool string_buffer::isExclusive() const
{
  return refCount_ == 1;
}

inline bool string_buffer::isShareable() const
{
  return m_bIsShareable;
}

inline const char* string_buffer::charBegin()
{
  if(m_bIsCharBufferActual)
    return m_pCharBuffer;

  begin();
  size_t len = wcslen(m_pBuffer);
  
  if(m_pCharBuffer != NULL)
    DeleteCharBuffer(m_pCharBuffer);//(*vtab_->pDeleteCharBuffer_)(charBuffer_);
		
#ifdef _WIN32
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  int n = ::WideCharToMultiByte(cp, 0, m_pBuffer, (int)len, m_pCharBuffer, 0, NULL, NULL);
  m_pCharBuffer = NewCharBuffer(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
  
  n = ::WideCharToMultiByte(cp, 0, m_pBuffer, (int)len, m_pCharBuffer, n, NULL, NULL);
	
#else // not _WIN32
  int n = dest_wcsmblen(m_pBuffer);
	//wcstombs (NULL, buffer_, 0xffff);
	if (n < 0)
	  return NULL;
  m_pCharBuffer = NewCharBuffer(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
	wchar_t *tmp = m_pBuffer;
	wchar_t **ptr = (wchar_t **)&tmp;
	dest_wcsnrtombs((unsigned char*) m_pCharBuffer, ptr, len, n + 1);
#endif

  m_pCharBuffer[n] = 0;

  m_bIsCharBufferActual = true;

  return m_pCharBuffer;
}

inline wchar_t* string_buffer::begin()
{
  if(m_pBuffer == NULL)
  {
    //buffer_ = (*vtab_->pNewWCharBuffer_)(1);
    reserve(1);
    m_pBuffer[0] = 0;
    //capacity_ = 1;

    //if(charBuffer_ != NULL)
    //  charBuffer_[0] = 0;

    m_bIsCharBufferActual = false;
#ifdef _WIN32
    m_bIsBSTRActual = false;
#endif
    //isShareable_ = true;
  }

  return m_pBuffer;
}

#ifdef _WIN32
inline BSTR string_buffer::bstr()
{
  if(m_bIsBSTRActual)
    return m_bstr;

  if(m_bstr != NULL)
    ::SysFreeString(m_bstr);

  m_bstr = ::SysAllocString(begin());

  return m_bstr;
}
#endif

inline void string_buffer::clear()
{
  if(m_pBuffer == NULL)
    return;

  m_pBuffer[0] = 0;

  m_bIsCharBufferActual = false;
#ifdef _WIN32
  m_bIsBSTRActual = false;
#endif
}

inline void string_buffer::flush()
{
  if(m_pCharBuffer != NULL)
    DeleteCharBuffer(m_pCharBuffer);//(*vtab_->pDeleteCharBuffer_)(charBuffer_);
  m_pCharBuffer = NULL;
#ifdef WIN32
  if(m_bstr != NULL)
    ::SysFreeString(m_bstr);
  m_bstr = NULL;
#endif  
  m_bIsCharBufferActual = false;
#ifdef WIN32
  m_bIsBSTRActual = false;
#endif
}

inline void string_buffer::reserve(size_t _len)
{
  if(m_nCapacity >= _len && (m_nCapacity != 0 || m_pBuffer != NULL))
    return;

  wchar_t* buffer = NewWCharBuffer(_len + 1);//(*vtab_->pNewWCharBuffer_)(_len + 1);
//  wchar_t* buffer = (*vtab_->pNewWCharBuffer_)((2 * _len) + 1);
  if(m_pBuffer != NULL)
    wcscpy(buffer, m_pBuffer);

  buffer[length()] = 0;

  DeleteWCharBuffer(m_pBuffer);//(*vtab_->pDeleteWCharBuffer_)(buffer_);

  m_pBuffer = buffer;
  m_nCapacity = _len;
//  capacity_ = 2 * _len;
  m_bIsShareable = true;

  m_bIsCharBufferActual = false;
#ifdef _WIN32
  m_bIsBSTRActual = false;
#endif
}

inline void string_buffer::enableSharing(bool _share)
{
  m_bIsShareable = _share;
}

inline bool string_buffer::isCharBufferActual()
{
  return m_bIsCharBufferActual;
}

#ifdef _WIN32
inline bool string_buffer::isBSTRActual()
{
  return m_bIsBSTRActual;
}
#endif

inline string_buffer::~string_buffer()
{
  if(m_pBuffer != NULL)
    DeleteWCharBuffer(m_pBuffer);//(*vtab_->pDeleteWCharBuffer_)(customAlloc_, buffer_);
  if(m_pCharBuffer != NULL)
    DeleteCharBuffer(m_pCharBuffer);//(*vtab_->pDeleteCharBuffer_)(customAlloc_, charBuffer_);
#ifdef _WIN32
  if(m_bstr != NULL)
    ::SysFreeString(m_bstr);
#endif
}

///////////////////////////////////////////////////////////////////////////////

//wchar_t* newWCharBuffer(alloc_t* customAlloc, size_t _size)
//{
//  return new wchar_t[_size];
//}
//
//void deleteWCharBuffer(alloc_t* customAlloc, wchar_t* _buffer)
//{
//  if(_buffer == NULL)
//    return;
//  delete[] _buffer;
//}
//
//char* newCharBuffer(alloc_t* customAlloc, size_t _size)
//{
//  return new char[_size];
//}
//
//void deleteCharBuffer(alloc_t* customAlloc, char* _buffer)
//{
//  if(_buffer == NULL)
//    return;
//  delete[] _buffer;
//}
//
//void deleteStrBuffer(alloc_t* customAlloc, str_buffer* _buffer)
//{
//  if(_buffer == NULL)
//    return;
//  delete _buffer;
////  _buffer->~str_buffer();
//}
//
//#if _MSC_VER >= 1300
///////////////////
//// Custom VTBL //
///////////////////
//wchar_t* newCustomWCharBuffer(alloc_t* customAlloc, size_t _size)
//{
//  return reinterpret_cast<wchar_t*>(newCustomCharBuffer(customAlloc, _size * sizeof(wchar_t)));
//}
//
//void deleteCustomWCharBuffer(alloc_t* customAlloc, wchar_t* _buffer)
//{
//  if(_buffer == NULL)
//    return;
//  deleteCustomCharBuffer(customAlloc, reinterpret_cast<char*>(_buffer));
//}
//
//char* newCustomCharBuffer(alloc_t* customAlloc, size_t _size)
//{
////  return new char[_size];
//  return reinterpret_cast<char*>(customAlloc->alloc(_size));
//}
//
//void deleteCustomCharBuffer(alloc_t* customAlloc, char* _buffer)
//{
//  if(_buffer == NULL)
//    return;
////  delete[] _buffer;
//  customAlloc->free(reinterpret_cast<void*>(_buffer));
//}
//
//void deleteCustomStrBuffer(alloc_t* customAlloc, str_buffer* _buffer)
//{
//  if(_buffer == NULL)
//    return;
////  delete _buffer;
//  _buffer->~str_buffer();
//  customAlloc->free(customAlloc, reinterpret_cast<void*>(_buffer));
//}
//#endif

wchar_t* string_buffer::NewWCharBuffer(size_t _size)
{
  return reinterpret_cast<wchar_t*>(NewCharBuffer(_size * sizeof(wchar_t)));
}

void string_buffer::DeleteWCharBuffer(wchar_t* _buffer)
{
  DeleteCharBuffer(reinterpret_cast<char*>(_buffer));
}

char* string_buffer::NewCharBuffer(size_t _size)
{
  if(customAlloc_)
    return reinterpret_cast<char*>(customAlloc_->alloc(_size));
  else
    return new char[_size];
}

void string_buffer::DeleteCharBuffer(char* _buffer)
{
  if(_buffer == NULL)
    return;

  if(customAlloc_)
    customAlloc_->free(reinterpret_cast<void*>(_buffer));
  else
    delete[] _buffer;
}

void string_buffer::destroy()
{
  string_buffer* buffer = this;
  alloc_t* customAlloc = customAlloc_;

  if(buffer == NULL)
    return;

  if(customAlloc)
  {
    buffer->~string_buffer();
    customAlloc->free(reinterpret_cast<void*>(buffer));
  }
  else
    delete buffer;
}

}

#endif //_GIS_COMMON_LIBRARY_PORTABLE_STR_BUFFER_H_