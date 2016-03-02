#include "stdafx.h"
#include "TestDiffComp.h"
#include <iostream>
#include "CommonLibrary/FileStream.h"

CTestDiffComp::CTestDiffComp()  : m_compressor(200)
{

}
CTestDiffComp::~CTestDiffComp()
{

}
void CTestDiffComp::AddDiff(int64 nLink)
{
	m_compressor.AddSymbol(nLink);

}
uint32 CTestDiffComp::GetCompressSize()
{
	return m_compressor.GetCompressSize();
}
void CTestDiffComp::compress( const embDB::TBPVector<uint64>& vecLinks,  CommonLib::IWriteStream *pStream)
{
	m_compressor.compress(vecLinks, pStream);
}
void CTestDiffComp::decompress(embDB::TBPVector<uint64>& vecLinks,  CommonLib::IReadStream *pStream)
{
	m_compressor.decompress(10, vecLinks, pStream);
}
void CTestDiffComp::RemoveDiff(int64 nDiff)
{
	m_compressor.RemoveSymbol(nDiff);
}

void TestDiffComp()
{
	double d2 = log( 2. );
	CTestDiffComp test, test1;
	CommonLib::CWriteMemoryStream WriteStream;
	CommonLib::CReadMemoryStream ReadStream;


	embDB::TBPVector<uint64> vec1, vec2;
	uint64 nCnt = 0;
	uint32 nSize = 0;
	uint64 nK = 1;
	for (uint64 i = 0; i < 0xFFFFFFFFFFFF; i += nK)
	{
		vec1.push_back(i);

		nCnt++;
		if((nCnt%1000) == 0)
			nK += 1;
		if(i != 0)
		{
			uint32 nSize = vec1.size() - 1;
			int64 nDiff = vec1[nSize] - vec1[nSize-1];
			test.AddDiff(nDiff);
		}
	
		
		nSize = test.GetCompressSize();
		if(nSize > 8192 || nCnt > 20000)
		{
			WriteStream.create(nSize);
			test.compress(vec1, &WriteStream);
			break;
		}
	}

	/*CommonLib::CWriteFileStream fStream;
	fStream.open(L"D:\\adigit", CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);
	for (size_t i = 0, sz = vecData.size(); i < sz; ++i)
	{
		CommonLib::CString str;
		str.format(L"%d", (int)vecData[i]);
		fStream.write((const byte*)str.cstr(), 1);
	}
	
	fStream.close();*/
	ReadStream.attachBuffer(WriteStream.buffer(), WriteStream.pos(), false);
	test1.decompress(vec2, &ReadStream);

	for (uint64 i = 0; i < nCnt; ++i)
	{
		uint64 nValue1 = vec1[i];
		uint64 nValue2 = vec2[i];
		if(nValue1 != nValue2)
			std::cout << "error i: " << i << " value vec1: " <<nValue1 << std::endl;
		if(i != 0)
			test1.RemoveDiff(vec2[i] - vec2[i-1]);
	}
	std::cout << "Cnt: " << nCnt << " compress: row size " << nCnt * sizeof(int64) << " compress size " <<nSize <<" k: " << ((double)nCnt * sizeof(int64)) /nSize << std::endl;
}