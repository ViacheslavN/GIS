#include "stdafx.h"
#include <vector>
typedef unsigned int uint32;
int log2(const int x)
{
	int y;
	__asm
	{
		bsr eax, x
		mov y, eax
	}
	return y;
}


void GeneratePoint16Bit()
{

	int nMaxCol = 10;
	int nCurCol = 0;
	std::vector<uint32> vecForBack;
	FILE *pFile = fopen("D:\\tableZPoint16Bit.h","w");
	fprintf(pFile, "{ 0x0 ");
	vecForBack.push_back(0);
	for (int i = 1; i <= 255; ++i )
	{
		fprintf(pFile, ", ");
		int nBitCount = log2(i);
		uint32 nRes = 0;
		uint32 tmp = i;
		uint32 nSymbol = 0x01;
		nRes |= tmp & nSymbol;
		for(int b = 1; b <=nBitCount; ++b)
		{
			if(tmp >> b & nSymbol)
			{
				int val = nSymbol << (b*2) ;
				nRes |= val;
			}
		}
		vecForBack.push_back(nRes);
		fprintf(pFile, "0x%x ", nRes);
		nCurCol++;
		if(nCurCol == nMaxCol)
		{
			fprintf(pFile, "\n");
			nCurCol = 0;
		}
	}
	fprintf(pFile, "} ");
	fclose(pFile);

	pFile = fopen("D:\\BacktableZPoint16Bit.h","w");
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
};