#pragma once

#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "../../EmbDB/NumLenCompress.h"

class CTestNumLem
{
	public:

		typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
		typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

		typedef embDB::TUnsignedNumLenCompressor<int64, embDB::TFindMostSigBit, TRangeEncoder, TACEncoder, 
			TRangeDecoder, TACDecoder, 64> TCompressor;

		CTestNumLem();
		~CTestNumLem();
		void AddLink(int64 nLink);
		uint32 GetCompressSize();
		void compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream);
		void decompress( embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream);

	private:
	 
	

		TCompressor m_compressor; 
};


void TestNumLen();