#include "stdafx.h"
#include "../../EmbDB/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include <iostream>


void WriteZorderTable(uint16 nBegin, uint16 nEnd)
{
	FILE *pFile = fopen("D:\\zOrderTable.h","w");
	fprintf(pFile, " ");
	for (uint16 x = nBegin; x <= nEnd; ++x)
	{
		fprintf(pFile, "%3d, ", x);
	}
	fprintf(pFile, "\n");
	for (int16 y = nEnd; y >= nBegin;--y)
	{
		fprintf(pFile, "%3d, ",  y);
		for (uint16 x = nBegin; x <= nEnd; ++x)
		{
			embDB::ZOrderPoint2DU16 point(x, y);
			fprintf(pFile, "%3d, ",  point.m_nZValue);
		}

		fprintf(pFile, "\n");
	}
	fclose(pFile);
}