#pragma once

#include "CommonLibrary/stream.h"
#include "../../EmbDB/ArithmeticCoder.h"
#include "../../EmbDB/RangeCoder.h"
#include "../../EmbDB/NumLenCompress.h"

class CTestNumLem
{
	public:

		typedef embDB::TRangeEncoder<uint64, 64> TRangeEncoder;
		typedef embDB::TRangeDecoder<uint64, 64> TRangeDecoder;

		typedef embDB::TACEncoder<uint64, 32> TACEncoder;
		typedef embDB::TACDecoder<uint64, 32> TACDecoder;

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