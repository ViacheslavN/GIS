// TestCommons.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include "TestSQLiteWks.h"
void testLoadXML();
void TestXML();
void testLoadXML1();
void TestShapeIndex();
void TestSQLiteWks();
void TestEmbDBWks();
class ITest : public CommonLib::AutoRefCounter
{
public:
	ITest(){}
	ITest(const CommonLib::CString & str) : m_sStr(str){}
	~ITest(){}
private:
	CommonLib::CString m_sStr;
};

COMMON_LIB_REFPTR_TYPEDEF(ITest);
typedef std::shared_ptr<ITest> TIShared;

typedef std::map<ITestPtr, int> TtestMap;



int _tmain(int argc, _TCHAR* argv[])
{
	//testLoadXML1();
	//return 1;
	//TestShapeIndex();
	TestEmbDBWks();

	/*CommonLib::CString s1(L"vfdgggf");
	CommonLib::CString s2(s1);

	ITest *pTest = new ITest(s1);
	delete pTest;*/
	//TestShapeIndex();
	//TestSQLiteWks();
 
	//TestXML();
	/*for(int i =0; i < 100000000; ++i)
	{
		CommonLib::CString s1 = L"vfdgggf";
		CommonLib::CString s2 = s1;
	}*/
	return 0;
}

