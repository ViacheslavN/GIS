#include "stdafx.h"
#include "stream.h"

namespace CommonLib
{


void IReadStreamBase::read( byte* pBuffer, size_t bufLen)
{
	if(IStream::isBigEndian())
		read_inverse(pBuffer, bufLen);
	else
		read_bytes(pBuffer, bufLen);
}



bool  IReadStreamBase::readBool()
{
	return (readTR<byte>() == 1) ? true : false;
}
byte  IReadStreamBase::readByte()
{
	return readTR<byte>();
}
char IReadStreamBase::readChar()
{
	return readTR<char>();
}
int16  IReadStreamBase::readint16()
{
	return readTR<int16>();
}
uint16 IReadStreamBase::readintu16()
{
	return readTR<uint16>();
}
uint32 IReadStreamBase::readDword()
{
	return readTR<uint32>();
}
int32 IReadStreamBase::readInt32()
{
	return readTR<int32>();
}
uint32 IReadStreamBase::readIntu32()
{
	return readTR<uint32>();
}
int64 IReadStreamBase::readInt64()
{
	return readTR<int64>();
}
uint64 IReadStreamBase::readIntu64()
{
	return readTR<uint64>();
}
float IReadStreamBase::readFloat()
{
	return readTR<float>();
}
double IReadStreamBase::readDouble()
{
	return readTR<double>();
}
void IReadStreamBase::read(bool& value)
{
	//readT<bool>(value);
	byte ret;
	readT<byte>(ret);
	value = ( ret == 1) ? true : false;
}
void IReadStreamBase::read(byte& value)
{
	readT<byte>(value);
}
void IReadStreamBase::read(char& value)
{
	readT<char>(value);
}
void IReadStreamBase::read(int16& value)
{
	readT<int16>(value);
}
void IReadStreamBase::read(uint16& value)
{
	readT<uint16>(value);
}
void IReadStreamBase::read(uint32& value)
{
	readT<uint32>(value);
}
void IReadStreamBase::read(int32& value)
{
	readT<int32>(value);
}
void IReadStreamBase::read(int64& value)
{
	readT<int64>(value);
}
void IReadStreamBase::read(uint64& value)
{
	readT<uint64>(value);
}
void IReadStreamBase::read(float& value)
{
	readT<float>(value);
}
void IReadStreamBase::read(double& value)
{
	readT<double>(value);
}
void IReadStreamBase::read(CommonLib::str_t& str)
{
	uint32 nlen = readIntu32();
	if(nlen)
	{
		str.reserve(nlen);
		read((byte*)str.wstr(), 2 *nlen);
	}

}

 

void  IWriteStreamBase::write(const byte* pBuffer, size_t bufLen )
{
	if(IStream::isBigEndian())
		write_inverse(pBuffer, bufLen);
	else
		write_bytes(pBuffer, bufLen);
}
void IWriteStreamBase::write(bool value)
{
	writeT<byte>(value ? (byte)1 : (byte)0);
}
void IWriteStreamBase::write(byte value)
{
	writeT<byte>(value);
}
void IWriteStreamBase::write(char value)
{
	writeT<char>(value);
}
void IWriteStreamBase::write(int16 value)
{
	writeT<int16>(value);
}
void IWriteStreamBase::write(uint16 value)
{
	writeT<uint16>(value);
}

void IWriteStreamBase::write(uint32 value)
{
	writeT<uint32>(value);
}

void IWriteStreamBase::write(int32 value)
{
	writeT<int32>(value);
}

void IWriteStreamBase::write(int64 value)
{
	writeT<int64>(value);
}
void IWriteStreamBase::write(uint64 value)
{
	writeT<uint64>(value);
}

void IWriteStreamBase::write(float value)
{
	writeT<float>(value);
}

void IWriteStreamBase::write(double value)
{
	writeT<double>(value);
}
void IWriteStreamBase::write(const CommonLib::str_t& str)
{
	writeT<uint32>(str.length());
	if(str.length())
	{
		write((byte*)str.cwstr(), str.length() *2);
	
	}
}

void IWriteStreamBase::write(const char* pszStr)
{
	write((byte*)pszStr, strlen(pszStr));
}
void IWriteStreamBase::write(const wchar_t* pszStr)
{
	write((byte*)pszStr, 2* wcslen(pszStr));
}
}