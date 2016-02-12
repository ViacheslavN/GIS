#include "stdafx.h"
#include "testNumLem.h"
#include <iostream>

CTestNumLem::CTestNumLem() : m_compressor(200)
{

}
CTestNumLem::~CTestNumLem()
{

}
void CTestNumLem::AddLink(int64 nLink)
{
	m_compressor.AddSymbol(nLink);
 
}
uint32 CTestNumLem::GetCompressSize()
{
	return m_compressor.GetCompressSize();
}
void CTestNumLem::compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream)
{
	m_compressor.compress(vecLinks, pStream);
}
void CTestNumLem::decompress(embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream)
{
	m_compressor.decompress(vecLinks, pStream);
}


void TestNumLen()
{
	double d2 = log( 2. );
	CTestNumLem test, test1;
	CommonLib::CWriteMemoryStream WriteStream;
	CommonLib::CReadMemoryStream ReadStream;


	embDB::TBPVector<int64> vec1, vec2;
	uint64 nCnt = 0;
	uint32 nSize = 0;
	for (uint64 i = 0; i < 0xFFFFFFFFFFFF; i += 1024)
	{
		nCnt++;
		test.AddLink(255);
		vec1.push_back(255);
		nSize = test.GetCompressSize();
		if(nSize > 8192 || nCnt > 1259)
		{
			WriteStream.create(nSize);
			test.compress(vec1, &WriteStream);
			break;
		}
	}

	ReadStream.attachBuffer(WriteStream.buffer(), WriteStream.pos(), false);
	test1.decompress(vec2, &ReadStream);

	for (uint64 i = 0; i < nCnt; ++i)
	{
		uint64 nValue1 = vec1[i];
		uint64 nValue2 = vec2[i];
		if(nValue1 != nValue2)
			std::cout << "error i: " << i << " value vec1: " <<nValue1 << std::endl;
	}
	 std::cout << "compress: row size " << nCnt * sizeof(int64) << " compress size " <<nSize <<" k: " << ((double)nCnt * sizeof(int64)) /nSize << std::endl;
}