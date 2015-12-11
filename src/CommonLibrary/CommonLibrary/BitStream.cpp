#include "stdafx.h"
#include "general.h"
#include "BitStream.h"

namespace CommonLib
{

	IReadBitStreamBase::IReadBitStreamBase()  
	{

	}
	IReadBitStreamBase::~IReadBitStreamBase(){

	}
	void IReadBitStreamBase::readBits(byte& nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_readBits<byte>(nBits, nCntBits); 
	}
	void IReadBitStreamBase::readBits(uint16&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_readBits<uint16>(nBits, nCntBits);
	}
	void IReadBitStreamBase::readBits(uint32&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_readBits<uint32>(nBits, nCntBits); 
	}
	void IReadBitStreamBase::readBits(uint64&  nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_readBits<uint64>(nBits, nCntBits); 
	}



	IWriteBitStreamBase::IWriteBitStreamBase()  {}
	IWriteBitStreamBase::~IWriteBitStreamBase()  {}

	void IWriteBitStreamBase::writeBit(byte nBit)
	{
				
		bool bitValue = (nBit & 0x01) ? true : false;
		writeBit(bitValue);

	}

	void IWriteBitStreamBase::writeBits(byte nBits, size_t nCntBits)
	{
		assert(nCntBits < 9);
		_writeBits<byte>(nBits, nCntBits); 
	}
	void IWriteBitStreamBase::writeBits(uint16 nBits, size_t nCntBits)
	{
		assert(nCntBits < 17);
		_writeBits<uint16>(nBits, nCntBits); 

	}
	void IWriteBitStreamBase::writeBits(uint32 nBits, size_t nCntBits)
	{
		assert(nCntBits < 33);
		_writeBits<uint32>(nBits, nCntBits); 
	}
	void IWriteBitStreamBase::writeBits(uint64 nBits, size_t nCntBits)
	{
		assert(nCntBits < 65);
		_writeBits<uint64>(nBits, nCntBits); 
	}

}