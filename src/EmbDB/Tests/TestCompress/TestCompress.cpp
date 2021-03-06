// TestCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedBitStream.h"
#include "TestAC.h"
#include "Test.h"
#include "testNumLem.h"
#include "TestDiffComp.h"
#include "BWTSort.h"
#include "CommonLibrary/FileStream.h"


uint32 compressFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile);
uint32 compressStaticFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile);
void DecompressFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut);
void DecompressStaticFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut);
void TestDoubleCompress();
void ZlibTest();
void TestStringCompress();
void TestSignCompress();
void TestCompressShape();
int _tmain(int argc, _TCHAR* argv[])
{


	//TestCompressShape();

	/*CommonLib::CWriteFileStream File;
	File.open(L"D:\\test\\files\\1aaa.txt", CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);

	for (size_t i = 0; i< 100000; ++i)
	{
		File.write((byte)0);
		File.write((byte)1);
	}
	File.close();

	CTestCompess test;
	test.TestCompress(L"D:\\test\\files");*/
	//test.compressFile(L"D:\\1.data", L"D:\\2.data");
	//TestStringCompress();
	//ZlibTest();
	//TestDiffComp();
	//TestNumLen();
	TestDoubleCompress();
	//TestBWTSort();

//	TestSignCompress();
	return 0;


/*	ACComp comp;
	CommonLib::MemoryStream stream;
	CommonLib::MemoryStream stream1;
//	comp.compress("���.������", &stream);
	comp.compressInteger("���.������", &stream1);
//	stream.seek(0, CommonLib::soFromBegin);
//	CommonLib::CString str;
//	comp.decompress(&stream, str);*/

/*	TestCompressAC("aa22dggaaaaaahhggfghhgfaaaaaddfddggaaaaaaaaaaaaaffgjaaaaaaaaaaaaaaaaaafgtthtjhjaaaaaaaaafhghfghhfhaaaaaaaaaaa");

	CommonLib::CString sSrcFile;
	//sSrcFile = L"D:\\1\\jscript.dll";
	sSrcFile = L"D:\\2\\1.log";
	//sSrcFile = L"D:\\2\\brok_XML_EN.xml";
	//sSrcFile = L"D:\\2\\CALC_FUTOPT";*/



	/*uint32 nOutSize1 = compressStaticFile(sSrcFile.cwstr(), L"D:\\2\\1.log.static.compress");
	DecompressStaticFile(nOutSize1, L"D:\\2\\1.log.static.compress", L"D:\\2\\1.log.static.decompress");
	*/
	CommonLib::CString sSrcFile;
	//sSrcFile = L"D:\\1\\jscript.dll";
	//sSrcFile = L"D:\\2\\1.log";
	sSrcFile = L"D:\\test\\files\\zOrder";
	//sSrcFile = L"D:\\2\\CALC_FUTOPT";

	/*uint32 nOutSize = compressFile(sSrcFile.cwstr(), L"D:\\test\\files\\bib.compress");
	DecompressFile(nOutSize, L"D:\\test\\files\\bib.compress", L"D:\\test\\files\\bib.compress.decompress");

	compressStaticFile(sSrcFile.cwstr(), L"D:\\test\\files\\bib.static.compress");
	DecompressStaticFile(nOutSize, L"D:\\test\\files\\bib.static.compress", L"D:\\test\\files\\bib.static.compress.decompress");
	*/
	//CTestCompess test;
	//test.compressFile(L"D:\\test\\files\\world_adm0.shp", L"D:\\test\\files\\world_adm0.compress");
	//test.TestCompress(L"D:\\test\\files");
	//test.compressFile(L"D:\\db\\1", L"D:\\db\\2");
	//TestNumLen();
	//TestDiffComp();
	return 0;
}

