#include "stdafx.h"
#include "General.h"
#include <vector>
typedef unsigned int uint32;
typedef unsigned __int64 uint64;
void GenerateRect16Bit()
{
	int nMaxCol = 10;
	int nCurCol = 0;
	std::vector<uint32> vecForBack;
	FILE *pFile = fopen("D:\\tableZRect16Bit.h","w");
	fprintf(pFile, "{ 0x0 ");
	vecForBack.push_back(0);
	uint64 nLastRes = 0;
	for (int i = 1; i <= 255; ++i )
	{
		fprintf(pFile, ", ");
		int nBitCount = log2(i);
		uint64 nRes = 0;
		uint64 tmp = i;
		uint64 nSymbol = 0x01;
		nRes |= tmp & nSymbol;
		for(int b = 1; b <=nBitCount; ++b)
		{
			if((tmp >> b) & nSymbol)
			{
				uint64 val = nSymbol << (b*4) ;
				nRes |= val;
			}
		}
		if(nRes == 0 || nLastRes > nRes )
		{
			int dd = 0;
			++dd;
		}
		nLastRes = nRes;
		vecForBack.push_back(nRes);
		fprintf(pFile, "0x%I64x ", nRes);
		nCurCol++;
		if(nCurCol == nMaxCol)
		{
			fprintf(pFile, "\n");
			nCurCol = 0;
		}
	}
	fprintf(pFile, "} ");
	fclose(pFile);

	vecForBack.clear();
	vecForBack.push_back(0);
	for (int i = 1; i <= 15; ++i )
	{
		int nBitCount = log2(i);
		uint32 nRes = 0;
		uint32 tmp = i;
		uint32 nSymbol = 0x01;
		nRes |= tmp & nSymbol;
		for(int b = 1; b <=nBitCount; ++b)
		{
			if((tmp >> b) & nSymbol)
			{
				int val = nSymbol << (b*4) ;
				nRes |= val;
			}
		}
		vecForBack.push_back(nRes);
	}
	pFile = fopen("D:\\BacktableZRect16Bit.h","w");
	fprintf(pFile, "{");

	uint32 nBegin = 0;
	nCurCol = 0;
	for (size_t i = 0 , sz = vecForBack.size(); i < sz; ++i)
	{
		uint32  nVal = vecForBack[i];
		for (uint32 v = nBegin; v < nVal; ++v)
		{
			fprintf(pFile, ", ");
			fprintf(pFile, "%d ", 0);
			nCurCol++;
			if(nCurCol == nMaxCol)
			{
				fprintf(pFile, "\n");
				nCurCol = 0;
			}
		}
		nBegin = nVal + 1;
		if(i != 0)
			fprintf(pFile, ", ");
		fprintf(pFile, "0x%x ", i);
		nCurCol++;
		if(nCurCol == nMaxCol)
		{
			fprintf(pFile, "\n");
			nCurCol = 0;
		}
	}


	fprintf(pFile, "} ");
	fclose(pFile);
}