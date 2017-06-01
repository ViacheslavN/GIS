// TestCommonLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CommonLibrary/String.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedBitStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/FileStream.h"
#include "CommonLibrary/blob.h"
#include "CommonLibrary/BitsVector.h"
void testAlloc ();

#include "../EmbDB/Utils/BitUtils.h"

class Int128
{
public:

	Int128(uint64 h = 0, uint64 l = 0) :high(h),low(l)
	{

	}
	Int128 operator+(const Int128 & rhs)
	{
		Int128 sum;
		sum.high = high + rhs.high;
		sum.low = low + rhs.low;
		// check for overflow of low 64 bits, add carry to high
		if (sum.low < low)
			++sum.high;
		return sum;
	}
	Int128 operator-(const Int128 & rhs)
	{
		Int128 difference;
		difference.high = high - rhs.high;
		difference.low = low - rhs.low;
		// check for underflow of low 64 bits, subtract carry to high
		if (difference.low > low)
			--difference.high;
		return difference;
	}

private:
	uint64 high;
	uint64 low;
};

int _tmain(int argc, _TCHAR* argv[])
{



	




	testAlloc();
	CommonLib::CBitsVector bitVectors;

	bool bBit = false;
	for (size_t i = 0; i < 10000; ++i)
	{

		bitVectors.push_back(bBit);
		bBit = !bBit;
	}
	

	for (size_t i = 0; i < 10000; ++i)
	{

		bool bBitt = bitVectors[bBit];
		bBit = !bBit;
	}
	 
	int nBitCount = 8;

	int nByte = nBitCount/8;
	int nBit = nBitCount%8;
	 

	Int128 a1(2, 0xFFFFFFFFFFFFFFFF);
	Int128 a2(1, 1);

	Int128 res = a1 + a2;

	uint64 dd = 0;
	uint64 dd1 = 0;
	uint64 dd2 =100;

	dd = (dd1 - 1);
	if(dd > (dd + 1))
	{
		int rr = 0;
		rr++;
	}


	int i = 0;
	int j = i++;
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	 
	CommonLib::CWriteMemoryStream memStream(alloc);
	memStream.resize(50);
	CommonLib::FxBitWriteStream bitWrite;
	CommonLib::FxBitReadStream bitRead;

	bitWrite.attach(&memStream, 0, 50);
	bitRead.attach(&memStream, 0, 50);

	uint32 nVal1111 = 32;
	int64 nValue = 0;
	int len = embDB::BitsUtils::log2(nVal1111);


	int len2 = embDB::BitsUtils::log2((uint32)1);
	int len3 = embDB::BitsUtils::log2((uint32)2);

	bitWrite.writeBits(nVal1111, len + 1);
 	bitRead.readBits(nValue, len);



	bitWrite.writeBits(nValue, 1);
	nValue = -1024;
	bitWrite.writeBits(nValue, 64);

	int64 nReadValue = 1;

	bitRead.readBits(nReadValue, 1);
	bitRead.readBits(nReadValue, 64);


	bitWrite.writeBits(nValue, 11);

	/*CommonLib::CWriteFileStream writeFSteam;
	writeFSteam.open(L"D:\\1.fs", CommonLib::ofmCreateAlways,  CommonLib::arWrite,  CommonLib::smNoMode);


	writeFSteam.write((uint64)23);
	writeFSteam.write((uint32)15);
	writeFSteam.write((byte)1);

	writeFSteam.close();

	CommonLib::CReadFileStream readFSteam;
	readFSteam.open(L"D:\\1.fs", CommonLib::ofmOpenExisting,  CommonLib::arRead,  CommonLib::smNoMode);

	uint64 retu64 = readFSteam.readIntu64();
	uint64 retu32 = readFSteam.readIntu32();
	byte retByte = readFSteam.readByte();


	readFSteam.close();
	memStream.write((uint64)23);
	memStream.write((uint32)15);
	memStream.write((byte)1);


	readStrim.attachBuffer(memStream.buffer(), memStream.size());
	uint64 nType;
	readStrim.read(nType);
	
	CommonLib::FxMemoryWriteStream writeStrim(alloc);


	

	std::vector<CommonLib::CString> vecFiles;
	CommonLib::FileSystem::getFiles(CommonLib::CString(L"D:\\2\\") + L"*.shp", vecFiles);


	CommonLib::CString path(L"C:\\test\\ddf\\1.shp");
	CommonLib::CString path1(L"C:\\test\\ddf\\");
	CommonLib::CString path2(L"C:\\test\\ddf\\1");
	CommonLib::CString path3(L"C:\\test\\ddf\\1.");
	CommonLib::CString path4(L"2.shp");
	CommonLib::CString path5(L"2.");
	CommonLib::CString path6(L"vdfvdfvfd");

	CommonLib::CString sFile = CommonLib::FileSystem::FindFileName(path);
	CommonLib::CString sFile1 = CommonLib::FileSystem::FindFileName(path1);
	CommonLib::CString sFile2 = CommonLib::FileSystem::FindFileName(path2);
	CommonLib::CString sFile3 = CommonLib::FileSystem::FindFileName(path3);


	CommonLib::CString sOnlyFile = CommonLib::FileSystem::FindOnlyFileName(path);
	CommonLib::CString sOnlyFile1 = CommonLib::FileSystem::FindOnlyFileName(path1);
	CommonLib::CString sOnlyFile2 = CommonLib::FileSystem::FindOnlyFileName(path2);
	CommonLib::CString sOnlyFile3 = CommonLib::FileSystem::FindOnlyFileName(path3);
	CommonLib::CString sOnlyFile4 = CommonLib::FileSystem::FindOnlyFileName(path4);
	CommonLib::CString sOnlyFile5 = CommonLib::FileSystem::FindOnlyFileName(path5);
	CommonLib::CString sOnlyFile6 = CommonLib::FileSystem::FindOnlyFileName(path6);

	CommonLib::CString sExt = CommonLib::FileSystem::FindFileExtension(path);
	CommonLib::CString sExt1 = CommonLib::FileSystem::FindFileExtension(path1);
	CommonLib::CString sExt2 = CommonLib::FileSystem::FindFileExtension(path2);
	CommonLib::CString sExt3 = CommonLib::FileSystem::FindFileExtension(path3);


	CommonLib::CString sph = CommonLib::FileSystem::FindFilePath(path);
	CommonLib::CString sph1 = CommonLib::FileSystem::FindFilePath(path1);
	CommonLib::CString sph2 = CommonLib::FileSystem::FindFilePath(path2);
	CommonLib::CString sph3 = CommonLib::FileSystem::FindFilePath(path3);
	testAlloc();
	return 0;


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



 

 

	byte bBit1, bBit2, bBit3, bBit4, bBit5, bBit6, bBit7, bBit8, bBit9;


 
	readBitStream.readBits(nBit16_14_r, 14);
	readBitStream.readBits(nBit16_r, 16);

	readBitStream.readBits(nBit32_30_r, 30);
	readBitStream.readBits(nBit32_r, 32);

	readBitStream.readBits(nBit64_60_r, 60);
	readBitStream.readBits(nBit64_r, 64);

	/*CommonLib::CString str;
 
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

