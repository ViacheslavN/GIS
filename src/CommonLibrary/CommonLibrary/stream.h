#ifndef _LIB_COMMON_STREAM_H
#define _LIB_COMMON_STREAM_H
#include "String.h"
#include "alloc_t.h"
namespace CommonLib
{

enum enSeekOffset
{
    soFromBegin,
    soFromCurrent,
    soFromEnd,
};

class IStream
{
public:
	virtual int32 size() const = 0;
	virtual int64 size64() const = 0;
	virtual bool seek(uint32 position, enSeekOffset offset ) = 0;
	virtual bool seek64(uint64 position, enSeekOffset offset ) = 0;
	virtual int32 pos() const = 0;
	virtual int64 pos64() const = 0;
	virtual void reset() = 0;
	virtual void close() = 0;
	virtual bool create(uint32 nSize) = 0;
	virtual bool attach(byte* pBuffer, uint32 nSize, bool bCopy = true)  = 0;
	virtual byte* deattach()  = 0;
	virtual byte* buffer()  = 0;
	virtual const byte* buffer() const = 0;

	static bool isBigEndian()
	{
		static const uint16 word = 0xFF00;
		return *((byte*) & word) != 0;

	}
};



class IWriteStream : public IStream
{
public:

	virtual void write(const byte* pBuffer, uint32 bufLen ) = 0;
	virtual void write(bool value) = 0;
	virtual void write(char value) = 0;
	virtual void write(byte value) = 0;
	virtual void write(int16 value) = 0;
	virtual void write(uint16 value) = 0;
	virtual void write(uint32 value) = 0;
	virtual void write(int32 value) = 0;
	virtual void write(int64 value) = 0;
	virtual void write(uint64 value) = 0;
	virtual void write(float value) = 0;
	virtual void write(double value) = 0;
	virtual void write(const CommonLib::CString& str) = 0;
	virtual void write(const char* pszStr) = 0;
	virtual void write(const wchar_t* pszStr) = 0;
	
	
	virtual ~IWriteStream()  {}



};






class IReadStream : public IStream
{
public:

	virtual ~IReadStream()  {}
	IReadStream(){}
	virtual void read( byte* pBuffer, uint32 bufLen ) = 0;
	virtual void read(bool& value) = 0;
	virtual void read(char& value) = 0;
	virtual void read(byte& value) = 0;
	virtual void read(int16& value) = 0;
	virtual void read(uint16& value) = 0;
	virtual void read(uint32& value) = 0;
	virtual void read(int32& value) = 0;
	virtual void read(int64& value) = 0;
	virtual void read(uint64& value) = 0;
	virtual void read(float& value) = 0;
	virtual void read(double& value) = 0;
	virtual void read(CommonLib::CString& str) = 0;
 

	virtual bool         readBool() = 0;
	virtual byte         readByte() = 0;
	virtual char         readChar() = 0;
	virtual int16        readint16() = 0;
	virtual uint16       readintu16() = 0;
	virtual uint32       readDword() = 0;
	virtual int32        readInt32() = 0;
	virtual uint32       readIntu32()= 0;
	virtual int64        readInt64() = 0;
	virtual uint64       readIntu64() = 0;
	virtual float        readFloat() = 0;
	virtual double       readDouble() = 0;


	virtual bool save_read( byte* pBuffer, uint32 bufLen ) = 0;
	virtual bool save_read(bool& value) = 0;
	virtual bool save_read(char& value) = 0;
	virtual bool save_read(byte& value) = 0;
	virtual bool save_read(int16& value) = 0;
	virtual bool save_read(uint16& value) = 0;
	virtual bool save_read(uint32& value) = 0;
	virtual bool save_read(int32& value) = 0;
	virtual bool save_read(int64& value) = 0;
	virtual bool save_read(uint64& value) = 0;
	virtual bool save_read(float& value) = 0;
	virtual bool save_read(double& value) = 0;
	virtual bool save_read(CommonLib::CString& str) = 0;



	//virtual bool checkRead(uint32 nSize) const = 0;
	//virtual bool IsEndOfStream() const = 0;
	//virtual bool AttachStream(IStream *pStream, uint32 nSize, bool bSeek = true) = 0;
	
};






class IReadStreamBase : public IReadStream
{
public:


	IReadStreamBase()  {}
	virtual ~IReadStreamBase()  {}

	template <typename T>
	void readT(T& val)
	{
		if(IStream::isBigEndian())
			read_inverse((byte*)&val, sizeof(T));
		else
			read_bytes((byte*)&val, sizeof(T));
	}


	template <typename T>
	T readTR()
	{
		T ret;
		if(IStream::isBigEndian())
			read_inverse((byte*)&ret, sizeof(T));
		else
			read_bytes((byte*)&ret, sizeof(T));
		return ret;
	}


	template <typename T>
	bool save_readT(T& val)
	{
		if(getFree() < sizeof(T))
			return false;

		if(IStream::isBigEndian())
			read_inverse((byte*)&val, sizeof(T));
		else
			read_bytes((byte*)&val, sizeof(T));

		return true;
	}

	virtual void read_bytes(byte* dst, uint32 size) = 0;
	virtual void read_inverse(byte* buffer, uint32 size) = 0;

	virtual void read( byte* pBuffer, uint32 bufLen );
	virtual void read(bool& value); 
	virtual void read(char& value); 
	virtual void read(byte& value); 
	virtual void read(int16& value);
	virtual void read(uint16& value);
	virtual void read(uint32& value);
	virtual void read(int32& value) ;
	virtual void read(int64& value) ;
	virtual void read(uint64& value);
	virtual void read(float& value); 
	virtual void read(double& value);
	virtual void read(CommonLib::CString& str);




	virtual bool save_read( byte* pBuffer, uint32 bufLen );
	virtual bool save_read(bool& value);
	virtual bool save_read(char& value);
	virtual bool save_read(byte& value);
	virtual bool save_read(int16& value);
	virtual bool save_read(uint16& value);
	virtual bool save_read(uint32& value);
	virtual bool save_read(int32& value);
	virtual bool save_read(int64& value);
	virtual bool save_read(uint64& value);
	virtual bool save_read(float& value);
	virtual bool save_read(double& value);
	virtual bool save_read(CommonLib::CString& str);





	virtual bool         readBool();
	virtual byte         readByte();
	virtual char         readChar();
	virtual int16        readint16() ;
	virtual uint16       readintu16();
	virtual uint32       readDword() ;
	virtual int32        readInt32() ;
	virtual uint32       readIntu32();
	virtual int64        readInt64();
	virtual uint64       readIntu64();
	virtual float        readFloat() ;
	virtual double       readDouble();


	uint32 getFree() const {return size() - pos(); }

};


class IWriteStreamBase : public IWriteStream
{
public:
	IWriteStreamBase(){}
	virtual ~IWriteStreamBase(){}



	template <typename T>
	void writeT(T value)
	{
		if(IStream::isBigEndian())
			write_inverse((byte*)&value, sizeof(T));
		else
			write_bytes((byte*)&value, sizeof(T));
	}


	virtual void write_bytes(const byte* buffer, uint32 size) = 0;
	virtual void write_inverse(const byte* buffer, uint32 size) = 0;

	virtual void write(const byte* pBuffer, uint32 bufLen );
	virtual void write(bool value);
	virtual void write(byte value);
	virtual void write(char value);
	virtual void write(int16 value);
	virtual void write(uint16 value);
	virtual void write(uint32 value);
	virtual void write(int32 value);
	virtual void write(int64 value);
	virtual void write(uint64 value);
	virtual void write(float value);
	virtual void write(double value);
	virtual void write(const CommonLib::CString& str);
	virtual void write(const char* pszStr);
	virtual void write(const wchar_t* pszStr);
};




/*
#define  SAFE_READ_EX(pStream, Val, size)  \
	if(pStream->checkRead(size))\
		pStream->read(Val);

#define  SAFE_READ(pStream, Val)  \
	if(pStream->checkRead(sizeof(Val)))\
		pStream->read(Val);

#define  SAFE_READ_RES_EX(pStream, Val, size)  \
	if(!pStream->checkRead(size))\
		return false;					\
	else							\
		pStream->read(Val); 
 
#define  SAFE_READ_RES(pStream, Val)  \
	if(!pStream->checkRead(sizeof(Val)))\
		return false;					\
	else							\
		pStream->read(Val); 

#define  SAFE_READ_BOOL_RES(pStream, bVal)  \
	if(!pStream->checkRead(sizeof(byte)))\
		return false;					\
	else							\
		bVal = pStream->readBool(); 
*/
 




template<class I>
class TMemoryStreamBase : public I
{


private:

	TMemoryStreamBase(const TMemoryStreamBase& stream){}
	TMemoryStreamBase& operator=(const TMemoryStreamBase& stream){}

public:
	TMemoryStreamBase(alloc_t *pAlloc = NULL) : m_pAlloc(pAlloc),m_pBuffer(0)
		,m_nPos(0)
		,m_nSize(0)
		,m_bAttach(false)
	{
		if(!m_pAlloc)
			m_pAlloc = &m_alloc;

		m_bIsBigEndian = IStream::isBigEndian();

	}
	~TMemoryStreamBase()
	{
		if(!m_bAttach && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}

	virtual bool create(uint32 nSize)
	{
		if(!m_bAttach && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
		m_nPos = 0;
		m_nSize = nSize;
		m_bAttach = false;

		return m_pBuffer != NULL;
	}
	virtual bool attach(byte* pBuffer, uint32 nSize, bool bCopy = true)
	{
		if(bCopy)
		{
			create(nSize);
			memcpy(m_pBuffer, pBuffer, nSize);
			m_bAttach = false;
		}
		else
		{
			m_pBuffer = pBuffer;
			m_bAttach = true;

		}
		m_nPos = 0;
		m_nSize = nSize;
		return true;
	}

	virtual byte* deattach()
	{
		byte* tmp = m_pBuffer;
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}
	virtual byte* buffer()
	{
		return m_pBuffer;
	}
	virtual const byte* buffer() const
	{
		return m_pBuffer;
	}
	virtual int32 size() const 
	{
		return m_nSize;
	}
	virtual int64 size64() const 
	{
		return (int64)m_nSize;
	}
	virtual bool seek(uint32 position, enSeekOffset offset )
	{
		if(!m_pBuffer)
			return false;

		uint32 newpos = 0;
		switch(offset)
		{
		case soFromBegin:
			newpos = position;
			break;
		case soFromCurrent:
			newpos = m_nPos + position;
			break;
		case soFromEnd:
			newpos = m_nSize + position;
			break;
		}
		if(newpos > m_nSize && m_bAttach)
			return false;
		if(newpos > m_nSize && !m_bAttach)
		{
			if(!resize(newpos - m_nSize))
				return false;
		}
		m_nPos = newpos;
		return true;
	}
	virtual bool seek64(uint64 position, enSeekOffset offset )
	{
		return seek((uint32)position, offset);
	}
	virtual int32 pos() const
	{
		return m_nPos;
	}
	virtual int64 pos64() const
	{
		return (int64)m_nPos;
	}
	virtual void reset()
	{
		m_nPos = 0;
	}
	virtual void close()
	{
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		
		if(!m_bAttach && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}
	virtual bool resize(uint32 nSize) {return false;}
protected:

	byte* m_pBuffer;
	uint32  m_nPos;
	uint32  m_nSize;
	bool m_bIsBigEndian;
	alloc_t *m_pAlloc;
	bool m_bAttach;
	simple_alloc_t m_alloc;
};

}
#endif