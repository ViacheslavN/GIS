#include "stdafx.h"
#include "../../EmbDB/NumLenCompress.h"
#include "../../EmbDB/SignCompressor.h"
#include "CommonLibrary/BitStream.h"
#include "CommonLibrary/WriteBitStream.h"
#include "CommonLibrary/FixedBitStream.h"
 
void TestSignCompress()
{



	embDB::TSignCompressor compress;

	CommonLib::WriteBitStream bitStream;


	bitStream.resize(1000);

	CommonLib::FxBitReadStream bitReadStream;



	uint16 nVal = 0xFFFF, nVal1 = 0;

	uint16 nLen = embDB::TFindMostSigBit::FMSB(nVal) - 1;


	bitStream.writeBits(nVal, nLen);
	bitReadStream.attachBuffer(bitStream.buffer(), bitStream.size());

	bitReadStream.readBits(nVal1, nLen);


	nVal1 |= ((uint16)1 << (nLen));

	bool bSign = false;
	for (int i = 0; i < 40000; ++i)
	{

		compress.AddSymbol(bSign);
		bSign = !bSign;

		uint32 nSize = compress.GetCompressSize();
		if(nSize > 8192)
			break;

	}
	uint32 nSize = compress.GetCompressSize();

}