#pragma once

#include "CommonLibrary/stream.h"
#include "../../EmbDB/ArithmeticCoder.h"
#include "../../EmbDB/RangeCoder.h"
#include "../../EmbDB/IntegerDiffCompress.h"

class CTestDiffComp
{
public:

	typedef embDB::TRangeEncoder<uint64, 64> TRangeEncoder;
	typedef embDB::TRangeDecoder<uint64, 64> TRangeDecoder;

	typedef embDB::TACEncoder<uint64, 32> TACEncoder;
	typedef embDB::TACDecoder<uint64, 32> TACDecoder;

	typedef embDB::TUnsignedIntegerDiffCompress<uint64, TRangeEncoder, TACEncoder, 
		TRangeDecoder, TACDecoder> TCompressor;

	CTestDiffComp();
	~CTestDiffComp();
	void AddDiff(int64 nDiff);
	uint32 GetCompressSize();
	void compress( const embDB::TBPVector<uint64>& vecLinks, embDB::TBPVector<uint64>& vecCheck,CommonLib::IWriteStream *pStream);
	void decompress( embDB::TBPVector<uint64>& vecLinks,embDB::TBPVector<uint64>& vecCheck, CommonLib::IReadStream *pStream);

private:



	TCompressor m_compressor; 
};


void TestDiffComp();