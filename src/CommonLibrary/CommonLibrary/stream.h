#ifndef _LIB_COMMON_STREAM_H
#define _LIB_COMMON_STREAM_H
#include "str_t.h"

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
		const uint16 word = 0xFF00;
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
	virtual void write(byte value) = 0;
	virtual void write(uint16 value) = 0;
	virtual void write(uint32 value) = 0;
	virtual void write(int32 value) = 0;
	virtual void write(int64 value) = 0;
	virtual void write(uint64 value) = 0;
	virtual void write(float value) = 0;
	virtual void write(const CommonLib::str_t& str) = 0;

	virtual ~IWriteStream()  {}
};

class IReadStream
{
public:

	virtual void read( byte* pBuffer, size_t bufLen ) = 0;
	virtual void read(bool& value) = 0;
	virtual void read(byte& value) = 0;
	virtual void read(uint16& value) = 0;
	virtual void read(uint32& value) = 0;
	virtual void read(int32& value) = 0;
	virtual void read(int64& value) = 0;
	virtual void read(uint64& value) = 0;
	virtual void read(float& value) = 0;
	virtual void read(double& value) = 0;
	virtual void read(CommonLib::str_t& str) = 0;

	virtual bool         readBool() = 0;
	virtual byte         readByte() = 0;
	virtual uint16       readWord() = 0;
	virtual uint32       readDword() = 0;
	virtual int32        readInt32() = 0;
	virtual uint32       readIntu32()= 0;
	virtual int64        readInt64() = 0;
	virtual uint64       readIntu64() = 0;
	virtual float        readFloat() = 0;
	virtual double       readDouble() = 0;

	virtual ~IReadStream()  {}
};



}

#endif