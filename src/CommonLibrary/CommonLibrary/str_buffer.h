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

class str_buffer
{
  public:
    static str_buffer* make(alloc_t* customAlloc);
    static str_buffer* make(alloc_t* customAlloc, int count);
    static str_buffer* make(alloc_t* customAlloc, const char* _str, int count = -1);
    static str_buffer* make(alloc_t* customAlloc, const wchar_t* _str, int count = -1);
    static str_buffer* make(alloc_t* customAlloc, const str_buffer& _str);

  private:
    // Constructors
    str_buffer(alloc_t* customAlloc);//, str_vtab* _vtab);
    str_buffer(alloc_t* customAlloc, int count);//, str_vtab* _vtab);
    str_buffer(alloc_t* customAlloc, const char* _str, int count);//, str_vtab* _vtab);
    str_buffer(alloc_t* customAlloc, const wchar_t* _str, int count);//, str_vtab* _vtab);
    str_buffer(alloc_t* customAlloc, const str_buffer& _str);//, str_vtab* _vtab);

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
    ~str_buffer();
//    friend void deleteStrBuffer(alloc_t* customAlloc, str_buffer* _buffer);
//    friend void deleteCustomStrBuffer(alloc_t* customAlloc, str_buffer* _buffer);
    wchar_t* NewWCharBuffer(size_t _size);
    void     DeleteWCharBuffer(wchar_t* _buffer);
    char*    NewCharBuffer(size_t _size);
    void     DeleteCharBuffer(char* _buffer);
    void destroy();
  
  private:
    wchar_t* buffer_;
    size_t   capacity_;
    char*    charBuffer_;
#ifdef _WIN32
    BSTR     bstr_;
#endif
    bool     isCharBufferActual_;
#ifdef _WIN32
    bool     isBSTRActual_;
#endif
    bool     isShareable_;

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

str_buffer* str_buffer::make(alloc_t* customAlloc)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(str_buffer))) str_buffer(customAlloc);//&customStrVTab);
  return new str_buffer(customAlloc);//&defaultStrVTab);
}

str_buffer* str_buffer::make(alloc_t* customAlloc, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(str_buffer))) str_buffer(customAlloc, count);//, &customStrVTab);
  return new str_buffer(customAlloc, count);//, &defaultStrVTab);
}

str_buffer* str_buffer::make(alloc_t* customAlloc, const char* _str, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(str_buffer))) str_buffer(customAlloc, _str, count);//, &customStrVTab);
  return new str_buffer(customAlloc, _str, count);//, &defaultStrVTab);
}

str_buffer* str_buffer::make(alloc_t* customAlloc, const wchar_t* _str, int count)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(str_buffer))) str_buffer(customAlloc, _str, count);//, &customStrVTab);
  return new str_buffer(customAlloc, _str, count);//, &defaultStrVTab);
}

str_buffer* str_buffer::make(alloc_t* customAlloc, const str_buffer& _str)
{
  if(customAlloc)
    return new(customAlloc->alloc(sizeof(str_buffer))) str_buffer(customAlloc, _str);//, &customStrVTab);
  return new str_buffer(customAlloc, _str);//, &defaultStrVTab);
}

inline str_buffer::str_buffer(alloc_t* customAlloc)//, str_vtab* _vtab)
  : buffer_(NULL),
    capacity_(0),
    charBuffer_(NULL),
    isCharBufferActual_(false),
    refCount_(1),
    isShareable_(true),
#ifdef _WIN32
    bstr_(NULL),
    isBSTRActual_(false),
#endif
    //vtab_(_vtab),
    customAlloc_(customAlloc)
{
}

inline str_buffer::str_buffer(alloc_t* customAlloc, int count)//, str_vtab* _vtab)
  : buffer_(NULL),
    capacity_(0),
    charBuffer_(NULL),
    isCharBufferActual_(false),
    refCount_(1),
    isShareable_(true),
#ifdef _WIN32
    bstr_(NULL),
    isBSTRActual_(false),
#endif
    //vtab_(_vtab),
    customAlloc_(customAlloc)
{
  if(count > 0)
  {
    reserve(count);
    buffer_[0] = 0;
  }
}

inline str_buffer::str_buffer(alloc_t* customAlloc, const char* _str, int count)//, str_vtab* _vtab)
  : buffer_(NULL),
    capacity_(0),
    charBuffer_(NULL),
    isCharBufferActual_(false),
    refCount_(1),
    isShareable_(true),
#ifdef _WIN32
    bstr_(NULL),
    isBSTRActual_(false),
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
		  buffer_[i] = _str[i];
    buffer_[len] = 0;
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

    int n = ::MultiByteToWideChar(cp, 0, _str, (int)len, buffer_, 0);
    reserve(n);
    //buffer_ = (*vtab_->pNewWCharBuffer_)(n + 1);
  
    n = ::MultiByteToWideChar(cp, 0, _str, (int)len, buffer_, n);
    
    buffer_[n] = 0;
    //capacity_ = len;
  }
#endif //_WIN32
}

inline str_buffer::str_buffer(alloc_t* customAlloc, const wchar_t* _str, int count)//, str_vtab* _vtab)
  : buffer_(NULL),
    capacity_(0),
    charBuffer_(NULL),
    isCharBufferActual_(false),
    refCount_(1),
    isShareable_(true),
#ifdef _WIN32
    bstr_(NULL),
    isBSTRActual_(false),
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
    memcpy(buffer_, _str, len * sizeof(wchar_t));
    buffer_[len] = 0;
    //capacity_ = len;
  }
}

inline str_buffer::str_buffer(alloc_t* customAlloc, const str_buffer& _str)//, str_vtab* _vtab)
  : buffer_(NULL),
    capacity_(0),
    charBuffer_(NULL),
    isCharBufferActual_(false),
    refCount_(1),
    isShareable_(true),
#ifdef _WIN32
    bstr_(NULL),
    isBSTRActual_(false),
#endif
//    vtab_(_vtab),
    customAlloc_(customAlloc)
{
  if(_str.buffer_ != NULL)
  {
    size_t len = wcslen(_str.buffer_);
    //buffer_ = (*vtab_->pNewWCharBuffer_)(len + 1);
    reserve(len);
    //wcscpy(buffer_, _str.buffer_);
    memcpy(buffer_, _str.buffer_, len * sizeof(wchar_t));
    buffer_[len] = 0;
    //capacity_ = len;
  }
}

inline void str_buffer::safeAddRef()
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

inline void str_buffer::safeRelease()
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

inline bool str_buffer::isEmpty() const
{
  return length() == 0;
}

inline size_t str_buffer::length() const
{
  if(buffer_ == NULL)
    return 0;

  if(buffer_[0] == 0)
    return 0;

  return wcslen(buffer_);
}

inline size_t str_buffer::capacity() const
{
  return capacity_;
}

inline bool str_buffer::isExclusive() const
{
  return refCount_ == 1;
}

inline bool str_buffer::isShareable() const
{
  return isShareable_;
}

inline const char* str_buffer::charBegin()
{
  if(isCharBufferActual_)
    return charBuffer_;

  begin();
  size_t len = wcslen(buffer_);
  
  if(charBuffer_ != NULL)
    DeleteCharBuffer(charBuffer_);//(*vtab_->pDeleteCharBuffer_)(charBuffer_);
		
#ifdef _WIN32
#ifdef _WIN32_WCE
  UINT cp = CP_ACP;
#else
  UINT cp = CP_THREAD_ACP;
#endif

  int n = ::WideCharToMultiByte(cp, 0, buffer_, (int)len, charBuffer_, 0, NULL, NULL);
  charBuffer_ = NewCharBuffer(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
  
  n = ::WideCharToMultiByte(cp, 0, buffer_, (int)len, charBuffer_, n, NULL, NULL);
	
#else // not _WIN32
  int n = dest_wcsmblen(buffer_);
	//wcstombs (NULL, buffer_, 0xffff);
	if (n < 0)
	  return NULL;
  charBuffer_ = NewCharBuffer(n + 1);//(*vtab_->pNewCharBuffer_)(n + 1);
	wchar_t *tmp = buffer_;
	wchar_t **ptr = (wchar_t **)&tmp;
	dest_wcsnrtombs((unsigned char*) charBuffer_, ptr, len, n + 1);
#endif

  charBuffer_[n] = 0;

  isCharBufferActual_ = true;

  return charBuffer_;
}

inline wchar_t* str_buffer::begin()
{
  if(buffer_ == NULL)
  {
    //buffer_ = (*vtab_->pNewWCharBuffer_)(1);
    reserve(1);
    buffer_[0] = 0;
    //capacity_ = 1;

    //if(charBuffer_ != NULL)
    //  charBuffer_[0] = 0;

    isCharBufferActual_ = false;
#ifdef _WIN32
    isBSTRActual_ = false;
#endif
    //isShareable_ = true;
  }

  return buffer_;
}

#ifdef _WIN32
inline BSTR str_buffer::bstr()
{
  if(isBSTRActual_)
    return bstr_;

  if(bstr_ != NULL)
    ::SysFreeString(bstr_);

  bstr_ = ::SysAllocString(begin());

  return bstr_;
}
#endif

inline void str_buffer::clear()
{
  if(buffer_ == NULL)
    return;

  buffer_[0] = 0;

  isCharBufferActual_ = false;
#ifdef _WIN32
  isBSTRActual_ = false;
#endif
}

inline void str_buffer::flush()
{
  if(charBuffer_ != NULL)
    DeleteCharBuffer(charBuffer_);//(*vtab_->pDeleteCharBuffer_)(charBuffer_);
  charBuffer_ = NULL;
#ifdef WIN32
  if(bstr_ != NULL)
    ::SysFreeString(bstr_);
  bstr_ = NULL;
#endif  
  isCharBufferActual_ = false;
#ifdef WIN32
  isBSTRActual_ = false;
#endif
}

inline void str_buffer::reserve(size_t _len)
{
  if(capacity_ >= _len && (capacity_ != 0 || buffer_ != NULL))
    return;

  wchar_t* buffer = NewWCharBuffer(_len + 1);//(*vtab_->pNewWCharBuffer_)(_len + 1);
//  wchar_t* buffer = (*vtab_->pNewWCharBuffer_)((2 * _len) + 1);
  if(buffer_ != NULL)
    wcscpy(buffer, buffer_);

  buffer[length()] = 0;

  DeleteWCharBuffer(buffer_);//(*vtab_->pDeleteWCharBuffer_)(buffer_);

  buffer_ = buffer;
  capacity_ = _len;
//  capacity_ = 2 * _len;
  isShareable_ = true;

  isCharBufferActual_ = false;
#ifdef _WIN32
  isBSTRActual_ = false;
#endif
}

inline void str_buffer::enableSharing(bool _share)
{
  isShareable_ = _share;
}

inline bool str_buffer::isCharBufferActual()
{
  return isCharBufferActual_;
}

#ifdef _WIN32
inline bool str_buffer::isBSTRActual()
{
  return isBSTRActual_;
}
#endif

inline str_buffer::~str_buffer()
{
  if(buffer_ != NULL)
    DeleteWCharBuffer(buffer_);//(*vtab_->pDeleteWCharBuffer_)(customAlloc_, buffer_);
  if(charBuffer_ != NULL)
    DeleteCharBuffer(charBuffer_);//(*vtab_->pDeleteCharBuffer_)(customAlloc_, charBuffer_);
#ifdef _WIN32
  if(bstr_ != NULL)
    ::SysFreeString(bstr_);
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

wchar_t* str_buffer::NewWCharBuffer(size_t _size)
{
  return reinterpret_cast<wchar_t*>(NewCharBuffer(_size * sizeof(wchar_t)));
}

void str_buffer::DeleteWCharBuffer(wchar_t* _buffer)
{
  DeleteCharBuffer(reinterpret_cast<char*>(_buffer));
}

char* str_buffer::NewCharBuffer(size_t _size)
{
  if(customAlloc_)
    return reinterpret_cast<char*>(customAlloc_->alloc(_size));
  else
    return new char[_size];
}

void str_buffer::DeleteCharBuffer(char* _buffer)
{
  if(_buffer == NULL)
    return;

  if(customAlloc_)
    customAlloc_->free(reinterpret_cast<void*>(_buffer));
  else
    delete[] _buffer;
}

void str_buffer::destroy()
{
  str_buffer* buffer = this;
  alloc_t* customAlloc = customAlloc_;

  if(buffer == NULL)
    return;

  if(customAlloc)
  {
    buffer->~str_buffer();
    customAlloc->free(reinterpret_cast<void*>(buffer));
  }
  else
    delete buffer;
}

}

#endif //_GIS_COMMON_LIBRARY_PORTABLE_STR_BUFFER_H_