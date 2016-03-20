#include "stdafx.h"
#include "BWTSort.h"
#include "CommonLibrary/string.h"
#include <string>
#include <algorithm>
#include <stdlib.h>

struct SuffixString
{
	int length;
	const char *StrPtr;
};

int cmpSuffStr(const void *a, const void *b)
{
	SuffixString *pa=(SuffixString *)a;
	SuffixString *pb=(SuffixString *)b;
	int cmpLen=(pa->length<pb->length)? pa->length : pb->length;
	int retval=memcmp(pa->StrPtr,pb->StrPtr,cmpLen);
	if(retval)
		return retval;
	assert(pa->length!=pb->length);
	return pa->length-pb->length;
}
int Compress(const char *allStrings, int totCharLen);
void TestBWTSort()
{
	std::string sText = "ABACABA";
	sText = "ABA";
	/*
	BACABA$A
	ACABA$AB
	CABA$ABA
	ABA$ABAC
	BA$ABACA
	A$ABACAB
	$ABACABA
	*/

	uint32 nFreq[256];
	memset(nFreq, 0, 256 * 4);
	std::string sText1 = sText;
	std::vector<std::string> vecSTr;
	std::vector<std::string> vecSTrSort;
	std::string sResultt;
	uint32 nLen = sText.length();
	std::string sST1;
	std::string sST2;
	sST1.reserve(nLen);
	sST2.reserve(nLen);

	vecSTr.push_back(sText);
	vecSTrSort.push_back(sText);
	for (uint32 i = 1; i < nLen; ++i)
	{
		sST1 = sText1[1];
		sST1 += sText1.substr(2, nLen - 1);
		sST1 += sText1[0];
		 
		vecSTr.push_back(sST1);
		vecSTrSort.push_back(sST1);
		sText1 = sST1;
	}


//	for (size_t i = 0 , sz = vecSTrSort.size(); i< sz; ++i)
	{
		 std::sort(vecSTrSort.begin(), vecSTrSort.end());
	}


	for (size_t i = 0 , sz = vecSTrSort.size(); i< sz; ++i)
	{
		sResultt += vecSTrSort[i][nLen-1];
	}
	int dd = 0;
	dd++;

	for (uint32 i = 0; i < sResultt.length(); ++i)
	{
		nFreq[sResultt[i]]++;
	}
	uint32 nSum = 0;
	for (uint32 i = 0; i < sResultt.length(); ++i)
	{
		nSum += nFreq[sResultt[i]];
		if(nSum != 0)
		{
			int dd = 0;
			dd++;
		}
		nFreq[sResultt[i]] = nSum - nFreq[sResultt[i]];
		if(nSum == 0)
		{
			int dd = 0;
			dd++;
		}
	}

	Compress(sText.c_str(), sText.length() + 1);
}

int Compress(const char *allStrings, int totCharLen)
{

	std::string sResult;
	if(!totCharLen)
		return 0;
	const char *ptr=allStrings;
	int charStat[257];
	memset(charStat,0,sizeof(charStat));
	for(int i=0;i<totCharLen;i++){
		charStat[(unsigned char)(*ptr)]++;
		ptr++;
	}
	unsigned char alphabet[256];
	int nLetters=0;
	for(int i=0;i<256;i++){
		if(charStat[i]){
			alphabet[nLetters]=(unsigned char)i;
			nLetters++;
		}
	}
	assert(charStat[0]>0);
	assert(nLetters>0);
	if(nLetters==1){
		int retval=0;
		while(totCharLen>=128){
			retval++;
			totCharLen>>=7;
		}
		retval+=3;
		return retval*8;
	}

	int firstChar=(charStat[0]>0)? alphabet[1] : alphabet[0];
	 
	int nSize = (totCharLen+1)*sizeof(SuffixString);

	SuffixString *suffString=(SuffixString *)malloc(nSize);
	SuffixString *suffString1=suffString+1;
	suffString[0].length=0;
	suffString[0].StrPtr=allStrings+totCharLen;
	for(int i=0;i<totCharLen;i++){
		suffString1[i].length=totCharLen-i;
		suffString1[i].StrPtr=allStrings+i;
	}
	qsort(suffString1,totCharLen,sizeof(SuffixString),cmpSuffStr);
	int FirstCharIdx=-1;
	unsigned char *pAlpha=alphabet;
	for(int i=0;i<=totCharLen;i++){
		if(suffString[i].StrPtr==allStrings){
			FirstCharIdx=i;
			continue;
		}

		 unsigned char curVal=suffString[i].StrPtr[-1];
		  pAlpha=alphabet;

		  sResult += curVal;
	}
	return 0;
}