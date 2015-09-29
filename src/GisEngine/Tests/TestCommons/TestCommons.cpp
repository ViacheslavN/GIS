// TestCommons.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include "TestSQLiteWks.h"
void testLoadXML();
void TestXML();
void TestShapeIndex();

class ITest : public CommonLib::AutoRefCounter
{
public:
	ITest(){}
	~ITest(){}
};

COMMON_LIB_REFPTR_TYPEDEF(ITest);
typedef std::shared_ptr<ITest> TIShared;

typedef std::map<ITestPtr, int> TtestMap;



int _tmain(int argc, _TCHAR* argv[])
{

	TestShapeIndex();
	//TestSQLiteWks();
 
	//TestXML();
	/*for(int i =0; i < 100000000; ++i)
	{
		CommonLib::CString s1 = L"vfdgggf";
		CommonLib::CString s2 = s1;
	}*/
	return 0;
}

