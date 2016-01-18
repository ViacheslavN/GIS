#include "stdafx.h"
#include "RangeCoder.h"
#include "CommonLibrary/FileStream.h"
#include <iostream>
typedef embDB::TRangeEncoder<uint64, 64> TRangeEncoder; 
typedef embDB::TRangeDecoder<uint64, 64> TRangeDecoder; 
double Log2( double n );
void Rescale(uint32 *Frequency) {
	for(int i = 1; i <= 256;i++) {
		Frequency[i]/=2;
		if(Frequency[i]<=Frequency[i-1]) Frequency[i]=Frequency[i-1]+1;
	}
}

uint32 compressFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile)
{
	CommonLib::CReadFileStream srcFile;
	CommonLib::CWriteFileStream dstFile;
	srcFile.open(pszFileNameIn, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
	dstFile.open(pszCompressFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);

	uint32 nFileSize = srcFile.size();

	uint32 CalcFreq[257];
	uint32 Freq[257];
	for(int i = 0;i < 257; i++) 
	{
		Freq[i] = i;
		CalcFreq[i] = 0;
	}

	TRangeEncoder rangeCoder(&dstFile);

	for (uint32 i = 0; i < nFileSize; ++i)
	{
		byte ch= srcFile.readByte();
		CalcFreq[ch] += 1;
		rangeCoder.EncodeSymbol(Freq[ch],Freq[ch+1],Freq[256]);

		for(int j=ch+1;j<257;j++)
			Freq[j]++;	
		if(Freq[256] >= rangeCoder.MaxRange)
			Rescale(Freq);
	}

	rangeCoder.EncodeFinish();
	uint64 nOutFileSize = dstFile.size();

	srcFile.close();
	dstFile.close();
	double dMinSize = 0;
	for(int i = 0;i < 256; i++) 
	{
		if(CalcFreq[i] == 0)
			continue;

		dMinSize += CalcFreq[i] * (-1*Log2((double)CalcFreq[i]/nFileSize));
	}
	uint64 nMinByteSize = (dMinSize + 7)/8;
	uint64 nError = nOutFileSize - nMinByteSize;
	double dErr = (double)nError * 100/nMinByteSize;

	std::cout <<"MinByteSize: " << nMinByteSize << " OutSize: " << nOutFileSize  << " Diff: " << nError << " Error: "<< dErr <<" Compress: " <<(double)nFileSize/nOutFileSize <<std::endl;
	return nFileSize;
}


void DecompressFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut)
{
	CommonLib::CReadFileStream srcFile;
	CommonLib::CWriteFileStream dstFile;
	srcFile.open(pszCompressFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
	dstFile.open(pszFileOut, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);

 

	uint32 Freq[257];
	for(int i = 0;i < 257; i++) 
		Freq[i] = i;

	TRangeDecoder rangeDecoder(&srcFile);
	rangeDecoder.StartDecode();
	for (uint32 i = 0; i < nFileSize; ++i)
	{
		unsigned int freq = rangeDecoder.GetFreq(Freq[256]);

		byte Symbol;
		for(Symbol=255;Freq[Symbol] > freq;Symbol--);
		//Symbol--;

		dstFile.write(Symbol);
		rangeDecoder.DecodeSymbol(Freq[Symbol], Freq[Symbol+1], Freq[256]);
	 

		for(int j = Symbol + 1; j < 257; j++)
			Freq[j]++;
		if(Freq[256]>=rangeDecoder.MaxRange)
			Rescale(Freq);
		
		

	}

 
}
