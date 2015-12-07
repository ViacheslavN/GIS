#include "stdafx.h"
#include "TestAC.h"
#include <string>
#include "CommonLibrary/FixedBitStream.h"
#include <math.h>
#include <cmath>
double Log2( double n )  
{  
	// log(n)/log(2) is log2.  
	return log( n ) / log( 2. );  
}
ACComp::ACComp()
{

}
ACComp::~ACComp()
{

}

struct SSymInfo
{
	double dP;
	int nCount;
	double dA;
	double dB;
	
	SSymInfo() : nCount(0), dA(0.), dB(0.), dP(0.)
	{

	}

};
typedef std::map<char, SSymInfo> TMapFreq;

typedef std::map<char, SSymInfo> TMapFreq;
struct SymbolInfo
{
 
	uint32 m_nFreq;
	uint32 m_nA;
	uint32 m_nB;
	uint32 m_nP;

	SymbolInfo() : m_nFreq(0), m_nA(0), m_nB(0)
	{

	}

};

typedef std::map<char, SymbolInfo> TMapFreqInt;


void BitsPlusFollow(bool bBit, uint32& nBits_to_follow, CommonLib::FxBitWriteStream *pStream)
{
	pStream->writeBit(bBit);
	for (; nBits_to_follow > 0; nBits_to_follow--)
	{
		pStream->writeBit(!bBit);
	}
}
void  ACComp::compressInteger(const char *pszText, CommonLib::IWriteStream* pStream)
{
	TMapFreqInt mapFreq;
	const char *pCh = pszText;
	int nLen = 0;
	while (*pCh != '\0')
	{
		SymbolInfo& info = mapFreq[*pCh];
		info.m_nFreq += 1;
		pCh++;
		nLen++;
	}

	int32 nCode_value_bits =  16;     /* Количество битов для кода */
	int32 Top_value =  (((uint32) 1 << nCode_value_bits) - 1);	/* Максимальное значение кода */
	 /* УКАЗАТЕЛИ HА СЕРЕДИHУ И ЧЕТВЕРТИ ИHТЕРВАЛА ЗHАЧЕHИЙ КОДА */

	int32 First_qtr (Top_value/4+1);  /* Конец пеpвой чеpвеpти */
	int32 Half      (2*First_qtr);    /* Конец пеpвой половины */
	int32 Third_qtr (3*First_qtr);   /* Конец тpетьей четвеpти */		
	int32 nPrevF = 0;
	int32 nLow = 0;
	int32 nHigh = Top_value;

	for (TMapFreqInt::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SymbolInfo& info = it->second;
		info.m_nB = nPrevF + info.m_nFreq;
		nPrevF = info.m_nB;
	}

	pStream->write(nLen);
	pStream->write(mapFreq.size());

	uint32 nSize = 0;
	for (TMapFreqInt::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SymbolInfo& info = it->second;
		pStream->write(it->first);
		pStream->write(info.m_nFreq);

		nSize += info.m_nFreq * (int)(-1*Log2((double)info.m_nFreq/nLen));

	}
	CommonLib::simple_alloc_t alloc;
	CommonLib::FxBitWriteStream bitStream(&alloc);
	bitStream.create(100);

	pCh = pszText;
	uint32 nBitsToFollow = 0;
	while (*pCh != '\0')
	{
		
		SymbolInfo& info = mapFreq[*pCh];

		int32 range = nHigh - nLow + 1; 
		int32 nPrevB = info.m_nB - info.m_nFreq;
		
		nHigh = nLow + (range* info.m_nB)/nLen - 1;
		nLow = nLow + (range*nPrevB )/nLen;
		for (;;)
		{
			if(nHigh < Half)
			{
				BitsPlusFollow(false, nBitsToFollow, &bitStream);
			}
			else if(nLow >= Half)
			{
				BitsPlusFollow(true, nBitsToFollow, &bitStream);
				nLow -= Half;
				nHigh -= Half;
			}
			else if((nLow >= First_qtr) && (nHigh < Third_qtr))
			{
				nBitsToFollow++;
				nLow -= First_qtr;
				nHigh -= First_qtr;
			}
			else
				break;

			nLow += nLow;
			nHigh += (nHigh  +1);
		}
		pCh++;

	}
	//bitStream.writeBits(nLow, 16);

	std::string sText;
	CommonLib::FxBitReadStream bitReadStream(&alloc);
	bitReadStream.attach(bitStream.buffer(), bitStream.size());
	uint16 value = 0;
	bitReadStream.readBits(value, 16);

	int readSymbol = 0;
	nLow = 0;
	nHigh = Top_value;

	while(readSymbol < nLen)
	{
		int32 freq = ((value - nLow + 1) * nLen -1)/(nHigh - nLow + 1);

		TMapFreqInt::iterator it = mapFreq.begin();
		for (; it != mapFreq.end(); ++it)
		{
			SymbolInfo& info = it->second;
			if(info.m_nB <= freq)
			{
				it++;
			}
			else
				break;
		}
		int32 range = nHigh - nLow + 1; 
		int32 nPrevB = it->second.m_nB - it->second.m_nFreq;

		nHigh = nLow + (range* it->second.m_nB)/nLen - 1;
		nLow = nLow + (range*nPrevB )/nLen;

		sText += it->first;
		readSymbol++;
		for (;;)
		{
			if(nHigh < Half)
				;
			else if(nLow >= Half)
			{
				nLow -= Half;
				nHigh -= Half;
				value -=Half;

			}
			else if ((nLow >= First_qtr) && (nHigh < Third_qtr))
			{
				nLow -= First_qtr;
				nHigh -= First_qtr;
				value -=First_qtr;
			}
			else
				break;
			nLow += nLow;
			nHigh += (nHigh  +1);

			bool bBit = bitReadStream.readBit();

			value += (value + (bBit ? 1 : 0));
		}

	}
	
}


void ACComp::compress(const char *pszText, CommonLib::IWriteStream* pStream)
{

	TMapFreq mapFreq;
	const char *pCh = pszText;
	int nCount = 0;
	while (*pCh != '\0')
	{
		SSymInfo& info = mapFreq[*pCh];
		info.nCount += 1;
		pCh++;
		nCount++;
	}

	if(mapFreq.empty())
		return;

	double dL = 0.;
	double dH = 1.;
	double dSym = 0;

	for (TMapFreq::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SSymInfo& info = it->second;
		info.dP = ((double)info.nCount/nCount);
		info.dA = dH -  info.dP;
		info.dB = dH;
		dH = info.dA;
		if(info.dA < 0)
			info.dA = 0;
	}
 	dL = 0.;
	dH = 1.;

	pCh = pszText;
	while (*pCh != '\0')
	{
		SSymInfo& info = mapFreq[*pCh];
		double dL1 = dL + info.dA *(dH - dL);
		double dH1 = dL + info.dB * (dH - dL);
		pCh++;

		dL = dL1;
		dH = dH1;

	}

	dSym = (dL + dH)/2;
	pStream->write(dSym);
	pStream->write(nCount);
	pStream->write(mapFreq.size());

	for (TMapFreq::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SSymInfo& info = it->second;
		pStream->write(it->first);
		pStream->write(info.nCount);

		
	}

}


void ACComp::decompress(CommonLib::IReadStream* pStream, CommonLib::CString& str)
{
	double dSym = pStream->readDouble();
	int nCount = pStream->readInt32();
	size_t size = pStream->readIntu32();
	str.reserve(nCount);
	TMapFreq mapFreq;

	for (size_t i = 0; i < size; ++i)
	{
		char ch = pStream->readChar();
		int nFreq = pStream->readInt32();

		SSymInfo& info = mapFreq[ch];
		info.nCount = nFreq;
	}

	double dL = 0.;
	double dH = 1.;

	for (TMapFreq::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SSymInfo& info = it->second;
		info.dP = ((double)info.nCount/nCount);
		info.dA = dH -  info.dP;
		info.dB = dH;
		dH = info.dA;
		if(info.dA < 0)
			info.dA = 0;
	}
	dL = 0.;
	dH = 1.;
	std::string sBuf;
	int nReadSymbol = 0;
	while(nReadSymbol < nCount)
	{
		for (TMapFreq::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
		{
			SSymInfo& info = it->second;
			 if(dSym >= info.dA && dSym < info.dB)
			 {
				 nReadSymbol++;
				sBuf += it->first;

				dSym = (dSym - info.dA)/(info.dB - info.dA);
				break;
			 }
		}
	}
	str = sBuf.c_str();
}