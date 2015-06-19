// TestCommonLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CommonLibrary/str_t.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedBitStream.h"
void testAlloc ();
int _tmain(int argc, _TCHAR* argv[])
{
	testAlloc();
	return 0;

	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	CommonLib::FxBitWriteStream writeBitStream(alloc);
	CommonLib::FxBitReadStream  readBitStream(alloc);

	writeBitStream.create(1000);
	uint16 nBit16_14 = (1 << 13) + (1 << 12) + (1 << 10) + (1 << 5);
	uint16 nBit16	= 0xffff;

	uint32 nBit32_30 =  (1 << 29) + (1 << 15) + (1 << 7) + (1 << 3);
	uint32 nBit32 = 0xffffffff;


	uint64 nBit64_60 =  ((uint64)1 << 59) + ((uint64)1 << 30) + ((uint64)1 << 15) + ((uint64)1 << 3);
	uint64 nBit64 = 0xffffffffffffffff;


	uint16 nBit16_14_r;
	uint16 nBit16_r;

	uint32 nBit32_30_r;
	uint32 nBit32_r;


	uint64 nBit64_60_r;
	uint64 nBit64_r;


	//writeBitStream.writeBits(nBit16, 15)


	writeBitStream.writeBit((byte)1);
	writeBitStream.writeBit((byte)0);
	writeBitStream.writeBit((byte)1);
	writeBitStream.writeBit((byte)0);
	writeBitStream.writeBit((byte)1);
	writeBitStream.writeBit((byte)0);
	writeBitStream.writeBit((byte)1);
	writeBitStream.writeBit((byte)0);
	writeBitStream.writeBit((byte)1);
	writeBitStream.writeBits(nBit16_14, 14);
	writeBitStream.writeBits(nBit16, 16);
	writeBitStream.writeBits(nBit32_30, 30);
	writeBitStream.writeBits(nBit32, 32);
	writeBitStream.writeBits(nBit64_60, 60);
	writeBitStream.writeBits(nBit64, 64);



	readBitStream.attach(writeBitStream.buffer(), writeBitStream.size());

 

	byte bBit1, bBit2, bBit3, bBit4, bBit5, bBit6, bBit7, bBit8, bBit9;


	readBitStream.readBit(bBit1);
	readBitStream.readBit(bBit2);
	readBitStream.readBit(bBit3);
	readBitStream.readBit(bBit4);
	readBitStream.readBit(bBit5);
	readBitStream.readBit(bBit6);
	readBitStream.readBit(bBit7);
	readBitStream.readBit(bBit8);
	readBitStream.readBit(bBit9);
	readBitStream.readBits(nBit16_14_r, 14);
	readBitStream.readBits(nBit16_r, 16);

	readBitStream.readBits(nBit32_30_r, 30);
	readBitStream.readBits(nBit32_r, 32);

	readBitStream.readBits(nBit64_60_r, 60);
	readBitStream.readBits(nBit64_r, 64);

	/*CommonLib::str_t str;
 
	CommonLib::MemoryStream stream(alloc);
	str.format("dgf %s", "dd");
	str.wstr();

	int i1 = 666;
	float f1 = 237.4556;
	double d1 = 237.4556;
	bool b1 = true;
	stream.write(i1);
	stream.write(f1);
	stream.write(d1);
	stream.write(b1);

	stream.reset();

	int i2;
	float f2;
	double d2;
	bool b2;

	stream.read(i2);
	stream.read(f2);
	stream.read(d2);
	stream.read(b2);*/

	return 0;
}

