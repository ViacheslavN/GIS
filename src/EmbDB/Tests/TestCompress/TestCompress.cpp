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

uint32 compressFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile);
uint32 compressStaticFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile);
void DecompressFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut);
void DecompressStaticFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut);
void TestDoubleCompress();
int _tmain(int argc, _TCHAR* argv[])
{

	//TestDiffComp();
	//TestNumLen();
	TestDoubleCompress();
	//TestBWTSort();
	return 0;


/*	ACComp comp;
	CommonLib::MemoryStream stream;
	CommonLib::MemoryStream stream1;
//	comp.compress(" Œ¬. Œ–Œ¬¿", &stream);
	comp.compressInteger(" Œ¬. Œ–Œ¬¿", &stream1);
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
	CTestCompess test;
	//test.compressFile(L"D:\\test\\files\\world_adm0.shp", L"D:\\test\\files\\world_adm0.compress");
	test.TestCompress(L"D:\\test\\files");
	//test.compressFile(L"D:\\db\\1", L"D:\\db\\2");
	//TestNumLen();
	//TestDiffComp();
	return 0;
}

