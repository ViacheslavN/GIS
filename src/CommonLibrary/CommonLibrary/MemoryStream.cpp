#include "stdafx.h"
#include "MemoryStream.h"
namespace CommonLib
{
	MemoryStream::MemoryStream(alloc_t *pAlloc) :
	m_pAlloc(pAlloc)
	,m_pBuffer(0)
	,m_nPos(0)
	,m_nUsedSize(0)
	,m_nSize(0)
	,m_bAttach(false)
	{
		m_bIsBigEndian = IStream::isBigEndian();
	}
	MemoryStream::~MemoryStream()
	{
		if(!m_bAttach)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}

	//IStream
	void MemoryStream::create(size_t nSize)
	{
		if(m_pBuffer && !m_bAttach)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
		m_nPos = 0;
		m_nUsedSize = nSize;
		m_nSize = nSize;
		m_bAttach = false;
	}
	void MemoryStream::attach(byte* pBuffer, size_t nSize)
	{
		m_pBuffer = pBuffer;
		m_nPos = 0;
		m_nUsedSize = 0;
		m_nSize = nSize;
		m_bAttach = true;
	}
	byte* MemoryStream::deattach()
	{
		byte* tmp = m_pBuffer;
		m_nPos = 0;
		m_nUsedSize = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}
	byte* MemoryStream:: buffer()
	{
		return m_pBuffer;
	}
	size_t MemoryStream::size() const
	{
		return m_nUsedSize;
	}

	bool MemoryStream::seek(size_t pos, enSeekOffset offset  )
	{
		if(!m_pBuffer)
			return false;

		size_t newpos = 0;
		switch(offset)
		{
		case soFromBegin:
			newpos = m_nPos;
			break;
		case soFromCurrent:
			newpos = m_nPos + pos;
			break;
		case soFromEnd:
			newpos = m_nUsedSize + pos;
			break;
		}
		if(newpos > m_nUsedSize && m_bAttach)
			return false;
		if(newpos > m_nUsedSize && !m_bAttach)
			resize(newpos - m_nUsedSize);
		m_nPos = newpos;
		return true;

	}
	size_t MemoryStream::pos() const
	{
		return m_nPos;
	}
	void MemoryStream::reset()
	{
		m_nPos = 0;
	}
	void MemoryStream::close()
	{
		m_nPos = 0;
		m_nUsedSize = 0;
		m_nSize = 0;
		m_pBuffer = 0;
	}
	void  MemoryStream::resize(size_t nSize)
	{
		size_t newSize = m_nSize;

		while(m_nUsedSize + nSize > newSize)
			newSize = size_t(newSize * 1.5) + 1;
		if(newSize > m_nSize)
		{
			m_nSize = newSize;
			byte* buffer =  (byte*)m_pAlloc->alloc(sizeof(byte) * newSize);
			if(m_pBuffer)
				memcpy(buffer, m_pBuffer, m_nUsedSize);
			m_pBuffer = buffer;
		}
		m_nUsedSize += nSize;
	}


	void MemoryStream::read_bytes(byte* dst, size_t size)
	{
		::memcpy(dst, m_pBuffer + m_nPos, size);
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}

	void MemoryStream::read_inverse(byte* buffer, size_t size)
	{
		for(int64 i = 0; i < size; m_nPos++, i++)
			buffer[i] = m_pBuffer[m_nPos + size - i - 1];
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}
	void MemoryStream::write_bytes(const byte* buffer, size_t size)
	{
		if(size > 0)
		{
			resize(size);
			::memcpy(m_pBuffer + m_nPos, buffer, size);
			m_nPos += size;
		}
		assert(m_nPos <= m_nSize);
	}
	void MemoryStream::write_inverse(const byte* buffer, size_t size)
	{
		resize(size);
		for(size_t i = 0; i < size; m_nPos++, i++)
			m_pBuffer[m_nPos + size - i - 1] = buffer[i];
		assert(m_nPos <= m_nSize);
	}
	void MemoryStream::read( byte* pBuffer, size_t bufLen)
	{
		if(m_bIsBigEndian)
			read_inverse(pBuffer, bufLen);
		else
			read_bytes(pBuffer, bufLen);
	}



	bool  MemoryStream::readBool()
	{
		  return (readTR<byte>() == 1) ? true : false;
	}
	byte  MemoryStream::readByte()
	{
		return readTR<byte>();
	}
	int16  MemoryStream::readint16()
	{
		return readTR<int16>();
	}
	uint16 MemoryStream::readintu16()
	{
		 return readTR<uint16>();
	}
	uint32 MemoryStream::readDword()
	{
		return readTR<uint32>();
	}
	int32 MemoryStream::readInt32()
	{
		return readTR<int32>();
	}
	uint32 MemoryStream::readIntu32()
	{
		return readTR<uint32>();
	}
	int64 MemoryStream::readInt64()
	{
		return readTR<int64>();
	}
	uint64 MemoryStream::readIntu64()
	{
		return readTR<uint64>();
	}
	float MemoryStream::readFloat()
	{
		return readTR<float>();
	}
	double MemoryStream::readDouble()
	{
		return readTR<double>();
	}
	void MemoryStream::read(bool& value)
	{
		//readT<bool>(value);
		byte ret;
		readT<byte>(ret);
		value = ( ret == 1) ? true : false;
	}
	void MemoryStream::read(byte& value)
	{
		readT<byte>(value);
	}
	void MemoryStream::read(uint16& value)
	{
		readT<uint16>(value);
	}
	void MemoryStream::read(uint32& value)
	{
		readT<uint32>(value);
	}
	void MemoryStream::read(int32& value)
	{
		readT<int32>(value);
	}
	void MemoryStream::read(int64& value)
	{
		readT<int64>(value);
	}
	void MemoryStream::read(uint64& value)
	{
		readT<uint64>(value);
	}
	void MemoryStream::read(float& value)
	{
		readT<float>(value);
	}
	void MemoryStream::read(double& value)
	{
		readT<double>(value);
	}
	void MemoryStream::read(CommonLib::str_t& str)
	{
		uint32 nlen = readIntu32();
		if(nlen)
		{
			str.reserve(nlen);
			read((byte*)str.wstr(), 2 *nlen);
		}

	}

	void  MemoryStream::write(const byte* pBuffer, size_t bufLen )
	{
		if(m_bIsBigEndian)
			write_inverse(pBuffer, bufLen);
		else
			write_bytes(pBuffer, bufLen);
	}
	void MemoryStream::write(bool value)
	{
		writeT<byte>(value ? (byte)1 : (byte)0);
	}

	void MemoryStream::write(byte value)
	{
		writeT<byte>(value);
	}

	void MemoryStream::write(uint16 value)
	{
		writeT<uint16>(value);
	}

	void MemoryStream::write(uint32 value)
	{
		writeT<uint32>(value);
	}

	void MemoryStream::write(int32 value)
	{
		writeT<int32>(value);
	}

	void MemoryStream::write(int64 value)
	{
		writeT<int64>(value);
	}
	void MemoryStream::write(uint64 value)
	{
		writeT<uint64>(value);
	}

	void MemoryStream::write(float value)
	{
		writeT<float>(value);
	}

	void MemoryStream::write(double value)
	{
		writeT<double>(value);
	}
	void MemoryStream::write(const CommonLib::str_t& str)
	{
		writeT<uint32>(str.length());
		if(str.length())
			write((byte*)str.cwstr(), str.length() *2);
	}
}