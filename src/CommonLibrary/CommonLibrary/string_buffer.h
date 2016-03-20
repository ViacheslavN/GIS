#ifndef _LIB_COMMON_LIBRARY_STR_BUFFER_H_
#define _LIB_COMMON_LIBRARY_STR_BUFFER_H_


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




class string_buffer
{
  public:
    static string_buffer* make(alloc_t* pAlloc);
    static string_buffer* make(alloc_t* pAlloc, int count);
    static string_buffer* make(alloc_t* pAlloc, const char* _str, int count = -1);
    static string_buffer* make(alloc_t* pAlloc, const wchar_t* _str, int count = -1);
    static string_buffer* make(alloc_t* pAlloc, const string_buffer& _str);

  private:
    // Constructors
    string_buffer(alloc_t* pAlloc);
    string_buffer(alloc_t* pAlloc, int count);
    string_buffer(alloc_t* pAlloc, const char* _str, int count);
    string_buffer(alloc_t* pAlloc, const wchar_t* _str, int count);
    string_buffer(alloc_t* pAlloc, const string_buffer& _str);

  public:
    void safeAddRef();
    void safeRelease();

    bool     isEmpty() const;
    uint32   length() const;
    uint32   capacity() const;
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
    ~string_buffer();

    wchar_t* NewWCharBuffer(size_t _size);
    void     DeleteWCharBuffer(wchar_t* _buffer);
    char*    NewCharBuffer(size_t _size);
    void     DeleteCharBuffer(char* _buffer);
    void destroy();
  
  private:
    wchar_t* m_pBuffer;
    uint32   m_nCapacity;
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
    mutable long m_nRefCount;
#elif defined(__IPHONE_3_1)
		mutable volatile int32_t m_nRefCount;
#else
    mutable long volatile m_nRefCount;
#endif
    alloc_t* m_pAlloc;
	simple_alloc_t m_alloc;
};

string_buffer* string_buffer::make(alloc_t* pAlloc)
{
	if(pAlloc)
	  return new(pAlloc->alloc(sizeof(string_buffer))) string_buffer(pAlloc);
	return new string_buffer(pAlloc);
}

string_buffer* string_buffer::make(alloc_t* pAlloc, int count)
{
	if(pAlloc)
		return new(pAlloc->alloc(sizeof(string_buffer))) string_buffer(pAlloc, count);
	return new string_buffer(pAlloc, count);
}

string_buffer* string_buffer::make(alloc_t* pAlloc, const char* _str, int count)
{
	if(pAlloc)
		return new(pAlloc->alloc(sizeof(string_buffer))) string_buffer(pAlloc, _str, count);
	return new string_buffer(pAlloc, _str, count);
}

string_buffer* string_buffer::make(alloc_t* pAlloc, const wchar_t* _str, int count)
{
	if(pAlloc)
		 return new(pAlloc->alloc(sizeof(string_buffer))) string_buffer(pAlloc, _str, count);
	return new string_buffer(pAlloc, _str, count);
}

string_buffer* string_buffer::make(alloc_t* pAlloc, const string_buffer& _str)
{
	if(pAlloc)
		return new(pAlloc->alloc(sizeof(string_buffer))) string_buffer(pAlloc, _str);
	return new string_buffer(pAlloc, _str);
}

inline string_buffer::string_buffer(alloc_t* pAlloc)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    m_nRefCount(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
    //vtab_(_vtab),
    m_pAlloc(pAlloc)
{
	 if(!m_pAlloc)
		 m_pAlloc = &m_alloc;
}

inline string_buffer::string_buffer(alloc_t* pAlloc, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    m_nRefCount(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
    //vtab_(_vtab),
    m_pAlloc(pAlloc)
{
	if(!m_pAlloc)
		m_pAlloc = &m_alloc;
  if(count > 0)
  {
    reserve(count);
    m_pBuffer[0] = 0;
  }
}

inline string_buffer::string_buffer(alloc_t* pAlloc, const char* _str, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    m_nRefCount(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
//    vtab_(_vtab),
    m_pAlloc(pAlloc)
{
	if(!m_pAlloc)
		m_pAlloc = &m_alloc;
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

inline string_buffer::string_buffer(alloc_t* pAlloc, const wchar_t* _str, int count)//, str_vtab* _vtab)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    m_nRefCount(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
//    vtab_(_vtab),
    m_pAlloc(pAlloc)
{
	if(!m_pAlloc)
		m_pAlloc = &m_alloc;
  if(_str != NULL && count != 0)
  {
    size_t len = wcslen(_str);
    if(count > 0 && count < (int)len)
      len = (size_t)count;
    reserve(len);
    memcpy(m_pBuffer, _str, len * sizeof(wchar_t));
    m_pBuffer[len] = 0;
  }
}

inline string_buffer::string_buffer(alloc_t* pAlloc, const string_buffer& _str)
  : m_pBuffer(NULL),
    m_nCapacity(0),
    m_pCharBuffer(NULL),
    m_bIsCharBufferActual(false),
    m_nRefCount(1),
    m_bIsShareable(true),
#ifdef _WIN32
    m_bstr(NULL),
    m_bIsBSTRActual(false),
#endif
    m_pAlloc(pAlloc)
{
	if(!m_pAlloc)
		m_pAlloc = &m_alloc;
  if(_str.m_pBuffer != NULL)
  {
    size_t len = wcslen(_str.m_pBuffer);
    reserve(len);
    memcpy(m_pBuffer, _str.m_pBuffer, len * sizeof(wchar_t));
    m_pBuffer[len] = 0;
  }
}

inline void string_buffer::safeAddRef()
{
  if(this == NULL)
    return;

#ifdef _WIN32
  ::InterlockedIncrement(&m_nRefCount);
#elif defined(__IPHONE_3_1)
	OSAtomicIncrement32(&m_nRefCount);
#else
	/*assert(!"Implement it!"); */
  m_nRefCount++;
#endif
}

inline void string_buffer::safeRelease()
{
  if(this == NULL)
    return;

#ifdef _WIN32
  if(::InterlockedDecrement(&m_nRefCount) == 0)
#elif defined(__IPHONE_3_1)
	OSAtomicDecrement32(&m_nRefCount); // return incorrect value
	if (0 == m_nRefCount) // this is more safety
#else
 /* assert(!"Implement it!");*/ if (0 == (--m_nRefCount))
#endif
	  destroy();
}

inline bool string_buffer::isEmpty() const
{
  return length() == 0;
}

inline uint32 string_buffer::length() const
{
  if(m_pBuffer == NULL)
    return 0;

  if(m_pBuffer[0] == 0)
    return 0;

  return (uint32)wcslen(m_pBuffer);
}

inline uint32 string_buffer::capacity() const
{
  return m_nCapacity;
}

inline bool string_buffer::isExclusive() const
{
  return m_nRefCount == 1;
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
  uint32 len = wcslen(m_pBuffer);
  
  if(m_pCharBuffer != NULL)
    DeleteCharBuffer(m_pCharBuffer);
		
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
 
    reserve(1);
    m_pBuffer[0] = 0;
     m_bIsCharBufferActual = false;
#ifdef _WIN32
    m_bIsBSTRActual = false;
#endif
   // m_bIsShareable = true;
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
    DeleteCharBuffer(m_pCharBuffer);
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

  wchar_t* buffer = NewWCharBuffer(_len + 1);
  if(m_pBuffer != NULL)
    wcscpy(buffer, m_pBuffer);

  buffer[length()] = 0;

  DeleteWCharBuffer(m_pBuffer);

  m_pBuffer = buffer;
  m_nCapacity = _len;
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
    DeleteWCharBuffer(m_pBuffer);
  if(m_pCharBuffer != NULL)
    DeleteCharBuffer(m_pCharBuffer);
#ifdef _WIN32
  if(m_bstr != NULL)
    ::SysFreeString(m_bstr);
#endif
}

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
 // if(m_pAlloc)
    return reinterpret_cast<char*>(m_pAlloc->alloc(_size));
 // else
  //  return new char[_size];
}

void string_buffer::DeleteCharBuffer(char* _buffer)
{
  if(_buffer == NULL)
    return;
   m_pAlloc->free(reinterpret_cast<void*>(_buffer));
 
}

void string_buffer::destroy()
{
  string_buffer* buffer = this;
  if(buffer == NULL)
    return;

  bool bExtAlloc = m_pAlloc != &m_alloc;

   buffer->~string_buffer();
   if(bExtAlloc)
	 m_pAlloc->free(reinterpret_cast<void*>(buffer));
   else
	   ::free(buffer);

}

}

#endif 