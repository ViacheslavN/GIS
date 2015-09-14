// TestCommons.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestSQLiteWks.h"
void testLoadXML();
void TestXML();
#include <memory>
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
	TestSQLiteWks();
 
	//TestXML();
	return 0;
}

