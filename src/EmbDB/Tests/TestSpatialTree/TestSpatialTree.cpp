// TestSpatialTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../EmbDB/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include <iostream>

 

void testZPoint();
void testZPoint1();
void testZRect1();
void TestPointSpatialTree();
void TestRectSpatialTree();
void WriteZorderTable(uint16 nBegin, uint16 nEnd);
int _tmain(int argc, _TCHAR* argv[])
{
	//testZRect1();
	//testZPoint();
//	testZPoint1();
//	TestPointSpatialTree();
	//WriteZorderTable(0, 10);
	TestRectSpatialTree();
//	return 0;
}

