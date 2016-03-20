#include "stdafx.h"
#include "stream.h"

namespace CommonLib
{


void IReadStreamBase::read( byte* pBuffer, uint32 bufLen)
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
void IReadStreamBase::read(CommonLib::CString& str)
{
	uint32 nUtf8Len = readIntu32();
	if(nUtf8Len)
	{
		std::vector<char> buf(nUtf8Len + 1);
		read((byte*)&buf[0], (uint32)buf.size());
		str.loadFromUTF8(&buf[0]);
	}

}
void IReadStreamBase::read(IStream *pStream, bool bAttach)
{
	 readStream(pStream, bAttach);
}
bool IReadStreamBase::save_read(IStream *pStream, bool bAttach)
{
	return SaveReadStream(pStream, bAttach);
}



bool IReadStreamBase::save_read( byte* pBuffer, uint32 bufLen )
{

	if(getFree() < bufLen)
		return false;
	if(IStream::isBigEndian())
		read_inverse(pBuffer, bufLen);
	else
		read_bytes(pBuffer, bufLen);

	return true;
}
bool IReadStreamBase::save_read(bool& value)
{
	return save_readT<bool>(value);
}
bool IReadStreamBase::save_read(char& value)
{
	return save_readT<char>(value);
}
bool IReadStreamBase::save_read(byte& value)
{
	return save_readT<byte>(value);
}
bool IReadStreamBase::save_read(int16& value)
{
	return save_readT<int16>(value);
}
bool IReadStreamBase::save_read(uint16& value)
{
	return save_readT<uint16>(value);
}
bool IReadStreamBase::save_read(uint32& value)
{
	return save_readT<uint32>(value);
}
bool IReadStreamBase::save_read(int32& value)
{
	return save_readT<int32>(value);
}
bool IReadStreamBase::save_read(int64& value)
{
	return save_readT<int64>(value);
}
bool IReadStreamBase::save_read(uint64& value)
{
	return save_readT<uint64>(value);
}
bool IReadStreamBase::save_read(float& value)
{
	return save_readT<float>(value);
}
bool IReadStreamBase::save_read(double& value)
{
	return save_readT<double>(value);
}
bool IReadStreamBase::save_read(CommonLib::CString& str)
{
	uint32 nUtf8Len = 0;
	if(!save_readT<uint32>(nUtf8Len))
		return false;
	
	if(getFree() < nUtf8Len)
		return false;

	if(nUtf8Len)
	{
		std::vector<char> buf(nUtf8Len + 1);
		read((byte*)&buf[0], (uint32)buf.size());
		str.loadFromUTF8(&buf[0]);
	}

	return true;
		
}
 

void  IWriteStreamBase::write(const byte* pBuffer, uint32 bufLen )
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
void IWriteStreamBase::write(const CommonLib::CString& str)
{

	uint32 nUtf8Len =  str.calcUTF8Length();
	writeT<uint32>(nUtf8Len);
	if(nUtf8Len)
	{
		std::vector<char> buf(nUtf8Len + 1);
		str.exportToUTF8(&buf[0], (int)buf.size());
		write((byte*)&buf[0], (uint32)buf.size());
	
	}
}

void IWriteStreamBase::write(const char* pszStr)
{
	write((byte*)pszStr, (uint32)strlen(pszStr));
}
void IWriteStreamBase::write(const wchar_t* pszStr)
{
	write((byte*)pszStr, 2* (uint32)wcslen(pszStr));
}

void IWriteStreamBase::write(IStream *pStream, int32 nPos, int32 nSize)
{
	writeStream(pStream, nPos, nSize);
}
}