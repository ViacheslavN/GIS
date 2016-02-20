#pragma once

#include "CommonLibrary/stream.h"
#include "CommonLibrary/ArithmeticCoder.h"
#include "CommonLibrary/RangeCoder.h"
#include "../../EmbDB/IntegerDiffCompress.h"

class CTestDiffComp
{
public:

	typedef CommonLib::TRangeEncoder<uint64, 64> TRangeEncoder;
	typedef CommonLib::TRangeDecoder<uint64, 64> TRangeDecoder;

	typedef CommonLib::TACEncoder<uint64, 32> TACEncoder;
	typedef CommonLib::TACDecoder<uint64, 32> TACDecoder;

	typedef embDB::TUnsignedIntegerDiffCompress<uint64, TRangeEncoder, TACEncoder, 
		TRangeDecoder, TACDecoder> TCompressor;

	CTestDiffComp();
	~CTestDiffComp();
	void AddDiff(int64 nDiff);
	uint32 GetCompressSize();
	void compress( const embDB::TBPVector<uint64>& vecLinks, CommonLib::IWriteStream *pStream);
	void decompress( embDB::TBPVector<uint64>& vecLinks, CommonLib::IReadStream *pStream);
	void RemoveDiff(int64 nDiff);
private:



	TCompressor m_compressor; 
};


void TestDiffComp();