#include "stdafx.h"
#include "FixedMemoryStream.h"
namespace CommonLib
{
	FxStreamBase::FxStreamBase(alloc_t *pAlloc) :
		m_pAlloc(pAlloc)
		,m_pBuffer(0)
		,m_nPos(0)
		,m_nSize(0)
		,m_bAttach(false)
	{
		m_bIsBigEndian = IStream::isBigEndian();
	}
	FxStreamBase::~FxStreamBase()
	{
		if(!m_bAttach && m_pAlloc && m_pBuffer)
		{
			m_pAlloc->free(m_pBuffer);
			m_pBuffer = 0;
		}
	}


	void FxStreamBase::create(size_t nSize)
	{
		assert(m_pAlloc);
		if(m_pBuffer && !m_bAttach)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = (byte*)m_pAlloc->alloc(sizeof(byte) * nSize);
		m_nPos = 0;
		m_nSize = nSize;
		m_bAttach = false;
	}
	void FxStreamBase::attach(byte* pBuffer, size_t nSize)
	{
		m_pBuffer = pBuffer;
		m_nPos = 0;
		m_nSize = nSize;
		m_bAttach = true;
	}
	byte* FxStreamBase::deattach()
	{
		byte* tmp = m_pBuffer;
		m_nPos = 0;
		m_nSize = 0;
		m_pBuffer = 0;
		m_bAttach = false;
		return tmp;
	}
	byte* FxStreamBase::buffer()
	{
		return m_pBuffer;
	}
	size_t FxStreamBase::size() const
	{
		return m_nSize;
	}
	bool FxStreamBase::seek(size_t pos, enSeekOffset offset )
	{
		if(!m_pBuffer)
			return false;

		size_t newpos = 0;
		switch(offset)
		{
		case soFromBegin:
			newpos = pos;
			break;
		case soFromCurrent:
			newpos = m_nPos + pos;
			break;
		case soFromEnd:
			newpos = m_nSize - pos;
			break;
		}
		if(newpos > m_nSize)
		{
			assert(false);
			return false;
		}
		m_nPos = newpos;
		return true;
	}
	size_t FxStreamBase::pos() const
	{
		return m_nPos;
	}
	void FxStreamBase::reset()
	{
			m_nPos = 0;
	}
	void FxStreamBase::close()
	{
		m_nPos = 0;
		m_nSize = 0;
		if(m_pBuffer && !m_bAttach && m_pAlloc)
		{
			m_pAlloc->free(m_pBuffer);
		}
		m_pBuffer = 0;
	}
	void FxStreamBase::read_bytes(byte* dst, size_t size)
	{
		::memcpy(dst, m_pBuffer + m_nPos, size);
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}

	void FxStreamBase::read_inverse(byte* buffer, size_t size)
	{
		for(int64 i = 0; i < size; m_nPos++, i++)
			buffer[i] = m_pBuffer[m_nPos + size - i - 1];
		m_nPos += size;
		assert(m_nPos <= m_nSize);
	}
	void FxStreamBase::write_bytes(const byte* buffer, size_t size)
	{
		if(size > 0)
		{
			::memcpy(m_pBuffer + m_nPos, buffer, size);
			m_nPos += size;
		}
		assert(m_nPos <= m_nSize);
	}
	void FxStreamBase::write_inverse(const byte* buffer, size_t size)
	{
		for(size_t i = 0; i < size; m_nPos++, i++)
			m_pBuffer[m_nPos + size - i - 1] = buffer[i];
		assert(m_nPos <= m_nSize);
	}
	


	FxMemoryReadStream::FxMemoryReadStream(alloc_t *pAlloc) : FxStreamBase(pAlloc)
	{

	}
	FxMemoryReadStream::~FxMemoryReadStream()
	{

	}

	bool  FxMemoryReadStream::readBool()
	{
		return (readTR<byte>() == 1) ? true : false;
	}
	byte  FxMemoryReadStream::readByte()
	{
		return readTR<byte>();
	}
	uint16 FxMemoryReadStream::readWord()
	{
		return readTR<uint16>();
	}
	uint32 FxMemoryReadStream::readDword()
	{
		return readTR<uint32>();
	}
	int32 FxMemoryReadStream::readInt32()
	{
		return readTR<int32>();
	}
	uint32 FxMemoryReadStream::readIntu32()
	{
		return readTR<uint32>();
	}
	int64 FxMemoryReadStream::readInt64()
	{
		return readTR<int64>();
	}
	uint64 FxMemoryReadStream::readIntu64()
	{
		return readTR<uint64>();
	}
	float FxMemoryReadStream::readFloat()
	{
		return readTR<float>();
	}
	double FxMemoryReadStream::readDouble()
	{
		return readTR<double>();
	}
	void FxMemoryReadStream::read( byte* pBuffer, size_t bufLen)
	{
		if(m_bIsBigEndian)
			read_inverse(pBuffer, bufLen);
		else
			read_bytes(pBuffer, bufLen);
	}
	void FxMemoryReadStream::read(bool& value)
	{
		//readT<bool>(value);
		byte ret;
		readT<byte>(ret);
		value = ( ret == 1) ? true : false;
	}
	void FxMemoryReadStream::read(byte& value)
	{
		readT<byte>(value);
	}
	void FxMemoryReadStream::read(int16& value)
	{
		readT<int16>(value);
	}
	void FxMemoryReadStream::read(uint16& value)
	{
		readT<uint16>(value);
	}
	void FxMemoryReadStream::read(uint32& value)
	{
		readT<uint32>(value);
	}
	void FxMemoryReadStream::read(int32& value)
	{
		readT<int32>(value);
	}
	void FxMemoryReadStream::read(int64& value)
	{
		readT<int64>(value);
	}
	void FxMemoryReadStream::read(uint64& value)
	{
		readT<uint64>(value);
	}
	void FxMemoryReadStream::read(float& value)
	{
		readT<float>(value);
	}
	void FxMemoryReadStream::read(double& value)
	{
		readT<double>(value);
	}



	FxMemoryWriteStream::FxMemoryWriteStream(alloc_t *pAlloc) : FxStreamBase(pAlloc)
	{

	}
	FxMemoryWriteStream::~FxMemoryWriteStream()
	{

	}

	void  FxMemoryWriteStream::write(const byte* pBuffer, size_t bufLen )
	{
		if(m_bIsBigEndian)
			write_inverse(pBuffer, bufLen);
		else
			write_bytes(pBuffer, bufLen);
	}
	void FxMemoryWriteStream::write(bool value)
	{
		writeT<byte>(value ? (byte)1 : (byte)0);
	}

	void FxMemoryWriteStream::write(byte value)
	{
		writeT<byte>(value);
	}

	void FxMemoryWriteStream::write(uint16 value)
	{
		writeT<uint16>(value);
	}

	void FxMemoryWriteStream::write(uint32 value)
	{
		writeT<uint32>(value);
	}

	void FxMemoryWriteStream::write(int32 value)
	{
		writeT<int32>(value);
	}

	void FxMemoryWriteStream::write(int64 value)
	{
		writeT<int64>(value);
	}
	void FxMemoryWriteStream::write(uint64 value)
	{
		writeT<uint64>(value);
	}

	void FxMemoryWriteStream::write(float value)
	{
		writeT<float>(value);
	}

	void FxMemoryWriteStream::write(double value)
	{
		writeT<double>(value);
	}
}