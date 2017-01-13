// SQliteZorder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sqlite3\sqlite3.h"
#include "sqlite3\Btree.h"


int _tmain(int argc, _TCHAR* argv[])
{
	sqlite3BtreeOpen(0, 0, 0, 0, 	0, 0);
	return 0;
}

