#ifndef _LIB_COMMON_STREAM_H
#define _LIB_COMMON_STREAM_H
#include "String.h"

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
	virtual void create(size_t nSize) = 0;
	virtual void attach(byte* pBuffer, size_t nSize)  = 0;
	virtual byte* deattach()  = 0;
	virtual byte* buffer()  = 0;
	virtual size_t size() const = 0;
	virtual bool seek(size_t position, enSeekOffset offset ) = 0;
	virtual size_t pos() const = 0;
	virtual void reset() = 0;
	virtual void close() = 0;
	static bool isBigEndian()
	{
		static const uint16 word = 0xFF00;
		return *((byte*) & word) != 0;

	}


	/*virtual void write(const byte* pBuffer, size_t bufLen ) = 0;
	virtual void write(bool value) = 0;
	virtual void write(byte value) = 0;
	virtual void write(uint16 value) = 0;
	virtual void write(uint32 value) = 0;
	virtual void write(int32 value) = 0;
	virtual void write(int64 value) = 0;
	virtual void write(float value) = 0;
	virtual void write(double value) = 0;
	

	virtual void read( byte* pBuffer, size_t bufLen ) = 0;
	virtual void read(bool& value) = 0;
	virtual void read(byte& value) = 0;
	virtual void read(uint16& value) = 0;
	virtual void read(uint32& value) = 0;
	virtual void read(int32& value) = 0;
	virtual void read(int64& value) = 0;
	virtual void read(float& value) = 0;
	virtual void read(double& value) = 0;

	virtual bool         readBool() = 0;
	virtual byte         readByte() = 0;
	virtual uint16       readWord() = 0;
	virtual uint32       readDword() = 0;
	virtual int32        readInt32() = 0;
	virtual int64        readInt64() = 0;
	virtual float        readFloat() = 0;
	virtual double       readDouble() = 0;*/

	virtual ~IStream()  {}
};



class IWriteStream
{
public:

	virtual void write(const byte* pBuffer, size_t bufLen ) = 0;
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






class IReadStream
{
public:

	virtual ~IReadStream()  {}
	IReadStream(){}
	virtual void read( byte* pBuffer, size_t bufLen ) = 0;
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

	virtual bool checkRead(uint32 nSize) const = 0;
	virtual bool IsEndOfStream() const = 0;

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

	virtual void read_bytes(byte* dst, size_t size) = 0;
	virtual void read_inverse(byte* buffer, size_t size) = 0;

	virtual void read( byte* pBuffer, size_t bufLen );
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


	virtual void write_bytes(const byte* buffer, size_t size) = 0;
	virtual void write_inverse(const byte* buffer, size_t size) = 0;

	virtual void write(const byte* pBuffer, size_t bufLen );
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


}


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


#endif