// TestSpatialTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommonLibrary/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include <iostream>

 

void testZPoint();
void testZPoint1();
void testZRect1();
void TestPointSpatialTree();
void TestRectSpatialTree();
void TestPointZorder();
void TestRectZorder();
void TestRectZorder(int Xmax, int Ymax, int qXmin, int qYmin, int qXmax, int qYmax);
void WriteZorderTable(uint16 nBegin, uint16 nEnd);
int _tmain(int argc, _TCHAR* argv[])
{
	//testZRect1();
	//testZPoint();
//	testZPoint1();
//	TestPointSpatialTree();
	//WriteZorderTable(0, 10);
  // TestRectSpatialTree();
	TestPointZorder();
//	TestRectZorder();
	return 0;
}

