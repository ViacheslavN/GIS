#include "stdafx.h"
#include "../../EmbDB/DB/transactions/Loks/WaitForGraph.h"


#define  TABLE_1 1
#define  TABLE_2 2
#define  TABLE_3 3
#define  TABLE_4 4

#define  TRAN_1 1
#define  TRAN_2 2
#define  TRAN_3 3
#define  TRAN_4 4
#define  TRAN_5 4


void SimpleCycle()
{

	//R1->T1->R2->T2->R1
	embDB::CWaitForGraph waitForGraph;
	embDB::eWaitRes type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_1, TRAN_1, embDB::wltWriteLock);
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_2, TRAN_2, embDB::wltWriteLock);
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_1, TRAN_2, embDB::wltWriteLock);
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_2, TRAN_1, embDB::wltWriteLock);



	waitForGraph.FreeObject(embDB::wotTable, TABLE_2, TRAN_2, embDB::wltWriteLock);
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_2, TRAN_1, embDB::wltWriteLock);


	waitForGraph.FreeObject(embDB::wotTable, TABLE_1, TRAN_2, embDB::wltWriteLock);
	waitForGraph.FreeObject(embDB::wotTable, TABLE_1, TRAN_1, embDB::wltWriteLock);
	waitForGraph.FreeObject(embDB::wotTable, TABLE_2, TRAN_1, embDB::wltWriteLock);
}


void SimpleCycle2()
{

	//R3->T2->R1->T1->R2->T3->R3
	embDB::CWaitForGraph waitForGraph;
	//R3->T2
	embDB::eWaitRes type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_3, TRAN_2, embDB::wltWriteLock);
	//R1->T1
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_1, TRAN_1, embDB::wltWriteLock);
	//R2->T3
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_2, TRAN_3, embDB::wltWriteLock);
	//T3->R3
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_3, TRAN_3, embDB::wltWriteLock);

	//T2->R1
	type = waitForGraph.TryToLockObject(embDB::wotTable, TABLE_2, TRAN_2, embDB::wltWriteLock);
 
}

void TestWaitForGraph()
{

	SimpleCycle();
	SimpleCycle2();

	 
}