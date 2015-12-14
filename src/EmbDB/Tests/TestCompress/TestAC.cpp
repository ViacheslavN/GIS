#include "stdafx.h"
#include "TestAC.h"
#include <string>
#include "CommonLibrary/FixedBitStream.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include <math.h>
#include <cmath>

#include "ArithmeticCoder.h"
#include "ACEncoder.h"
#include "ACDecoder.h"
#include "RangeCoder.h"

typedef embDB::TACEncoder<int32, 16> TACEncoder;
typedef embDB::TACDecoder<int32, 16> TACDecoder;
typedef embDB::TRangeEncoder<uint32, 32> TRangeEncoder; 
typedef embDB::TRangeDecoder<uint32, 32> TRangeDecoder; 
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

static int bitwrite = 0;
void BitsPlusFollow(bool bBit, uint32& nBits_to_follow, CommonLib::FxBitWriteStream *pStream)
{
	pStream->writeBit(bBit);
	bitwrite++;
	for (; nBits_to_follow > 0; nBits_to_follow--)
	{
		pStream->writeBit(!bBit);
		bitwrite++;
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
	double dSize = 0;
	for (TMapFreqInt::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SymbolInfo& info = it->second;
		pStream->write(it->first);
		pStream->write(info.m_nFreq);

		dSize += info.m_nFreq * (-1*Log2((double)info.m_nFreq/nLen));

	}


	nSize = (int)dSize;
	CommonLib::simple_alloc_t alloc;
	CommonLib::FxBitWriteStream bitStream(&alloc);
	bitStream.create(nSize/8 + 1);

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

	nBitsToFollow += 1;           /* опpеделяющих чеpвеpть,*/
	if (nLow < First_qtr) 
			BitsPlusFollow(false, nBitsToFollow, &bitStream); /* лежащую в     */
	
	else BitsPlusFollow(true, nBitsToFollow, &bitStream);     

	//bitStream.writeBits(nLow, 16);

	uint32 nBitsSize = bitStream.sizeInBits();
	uint32 nBitPos = bitStream.posInBits();
	std::string sText;
	CommonLib::FxBitReadStream bitReadStream(&alloc);
	bitReadStream.attachBits(bitStream.buffer(), nBitPos);

	uint32 nBitPo2s = bitReadStream.posInBits();
	uint16 value = 0;
	bitReadStream.readBits(value, 16);
	bitReadStream.seek(0, CommonLib::soFromBegin);
	
	uint16 value1 = 0;
	uint16 value2 = 0;
	uint16 value3 = 0;
	int nCnt = 0;
	for (int i = 1; i<=nCode_value_bits; i++) { /* нения значе-  */

				uint32 nBit = (bitReadStream.readBit() ? 1 : 0);
		      value1 = 2*value1+ nBit;       /* ния кода      */
			  value2 = ( value2 << 1 ) | nBit;

			  if(nBit)
				  value3 |= (0x01 << (i-1));

			  nCnt++;
	 }
	value = value1;
	int readSymbol = 0;
	nLow = 0;
	nHigh = Top_value;

	int readBit = 16;
	int readinCycle = 0;
	int readinCycle2 = 0;
	while(readSymbol < nLen)
	{
		int32 freq = ((value - nLow + 1) * nLen -1)/(nHigh - nLow + 1);

		TMapFreqInt::iterator it = mapFreq.begin();
		for (; it != mapFreq.end();)
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
		if(readSymbol == nLen)
			break;
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
			nLow = 2 *nLow;
			nHigh = (2 *nHigh)  +1;

			bool bBit = readBit < nBitPos ?  bitReadStream.readBit() : 0;
			if(readBit < nBitPos)
			{
				readBit++;
				readinCycle++;
			}
			else
			{
				int dd = 0;
				dd++;
				readinCycle2++;
			}
			value = ((2 * value) + (bBit ? 1 : 0));
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



void TestCompressAC(const std::string& sStr)
{

	TMapFreqInt mapFreq;
	for (size_t i = 0, sz = sStr.size(); i < sz; ++i)
	{
		SymbolInfo& info = mapFreq[sStr[i]];
		info.m_nFreq += 1;
 
	}
	int32 nPrevF = 0;
	for (TMapFreqInt::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SymbolInfo& info = it->second;
		info.m_nB = nPrevF + info.m_nFreq;
		nPrevF = info.m_nB;
	}


	uint32 nSize = 0;
	double dSize = 0;
	double dRowSize = 0;
	for (TMapFreqInt::iterator it = mapFreq.begin(); it != mapFreq.end(); ++it)
	{
		SymbolInfo& info = it->second;
		dRowSize += info.m_nFreq * (-1*Log2((double)info.m_nFreq/sStr.size()));

	}
	dSize = dRowSize;
	if(dSize < 32)
		dSize = 32;
	nSize = (int32)dSize;

	CommonLib::simple_alloc_t alloc;
	CommonLib::FxBitWriteStream bitWriteStream(&alloc);
	CommonLib::WriteBitStream  BitStream(&alloc);
	CommonLib::MemoryStream  WriteStream(&alloc);
	bitWriteStream.create(nSize/8 + 1 + 100);
	BitStream.create(nSize/8 + 1); 


	ArithmeticCoderC ac;
	TACEncoder ac1(&BitStream);
	TRangeEncoder rgEncode(&WriteStream);
//	ac.SetBitWriteStream(&bitWriteStream);

	fstream source;
	source.open("D:\\2\\test.txt.comp", ios::in|ios::out| ios::binary );
	ac.SetFile(&source);
	for (size_t i = 0, sz = sStr.size(); i < sz; ++i)
	{
		SymbolInfo& info = mapFreq[sStr[i]];
		int32 nPrevB = info.m_nB - info.m_nFreq;
		ac.Encode(nPrevB, info.m_nB, sz);
		ac1.EncodeSymbol(nPrevB, info.m_nB, sz);
		rgEncode.EncodeSymbol(nPrevB, info.m_nB, sz);
	}
	ac.EncodeFinish();
	ac1.EncodeFinish();
	rgEncode.EncodeFinish();
	source.seekp(0);
	size_t sizeInBits = BitStream.posInBits();
	CommonLib::FxBitReadStream bitReadStream(&alloc);
	CommonLib::FxMemoryReadStream ReadStream(&alloc);

	bitReadStream.attachBits(BitStream.buffer(),sizeInBits);
	ReadStream.attach(WriteStream.buffer(), WriteStream.pos());
//	ac.SetBitReadStream(&bitReadStream);
	int32 nLens = 0;
	int32 nLens1 = 0;
	int32 nLens2 = 0;
	std::string sDecompStr;
	std::string sDecompStr1;
	std::string sDecompStr2;

	TACDecoder  ae(&bitReadStream);
	TRangeDecoder rgDecode(&ReadStream);
	ae.StartDecode();
	ac.DecodeStart();
	rgDecode.StartDecode();
/*	while (nLens < sStr.size())
	{
		

		// read value
		unsigned int freq = ac.DecodeTarget( sStr.size() );



		TMapFreqInt::iterator it = mapFreq.begin();
		for (; it != mapFreq.end();)
		{
			SymbolInfo& info = it->second;
			if(info.m_nB <= freq)
			{
				it++;
			}
			else
				break;
		}
		sDecompStr += it->first;

		nLens++;
		if(nLens == sStr.size())
			break;

		SymbolInfo& info = it->second;
		int32 nPrevB = info.m_nB - info.m_nFreq;
		ac.Decode( nPrevB, info.m_nB );
	

	} */

	nLens = 0;


	while (nLens1 < sStr.size())
	{
		unsigned int freq = ae.GetFreq( sStr.size() );

		TMapFreqInt::iterator it = mapFreq.begin();
		for (; it != mapFreq.end();)
		{
			SymbolInfo& info = it->second;
			if(info.m_nB <= freq)
			{
				it++;
			}
			else
				break;
		}
		sDecompStr1 += it->first;

		nLens1++;
		if(nLens1 == sStr.size())
			break;

		SymbolInfo& info = it->second;
		int32 nPrevB = info.m_nB - info.m_nFreq;
		ae.DecodeSymbol( nPrevB, info.m_nB, sStr.size());


	} 


	while (nLens2 < sStr.size())
	{
		unsigned int freq = rgDecode.GetFreq( sStr.size() );

		TMapFreqInt::iterator it = mapFreq.begin();
		for (; it != mapFreq.end();)
		{
			SymbolInfo& info = it->second;
			if(info.m_nB <= freq)
			{
				it++;
			}
			else
				break;
		}
		sDecompStr2 += it->first;

		nLens2++;
		if(nLens2 == sStr.size())
			break;

		SymbolInfo& info = it->second;
		int32 nPrevB = info.m_nB - info.m_nFreq;
		rgDecode.DecodeSymbol( nPrevB, info.m_nB, sStr.size());


	} 
}