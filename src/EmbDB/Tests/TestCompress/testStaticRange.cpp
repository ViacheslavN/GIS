#include "stdafx.h"
#include "RangeCoder.h"
#include "CommonLibrary/FileStream.h"
#include <iostream>
typedef embDB::TRangeEncoder<uint64, 64> TRangeEncoder; 
typedef embDB::TRangeDecoder<uint64, 64> TRangeDecoder; 



double Log2( double n );

uint32 compressStaticFile(const wchar_t *pszFileNameIn, const wchar_t* pszCompressFile)
{
	CommonLib::CReadFileStream srcFile;
	CommonLib::CWriteFileStream dstFile;
	srcFile.open(pszFileNameIn, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
	dstFile.open(pszCompressFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);

	uint32 nFileSize = srcFile.size();

	uint32 Freq[257];
	uint32 FreqPrev[257];
	for(int i = 0;i < 257; i++) 
	{
		Freq[i] = 0;
		FreqPrev[i] = 0;
	}



	for (uint32 i = 0; i < nFileSize; ++i)
	{
		byte ch= srcFile.readByte();
		Freq[ch]++;
		//	if(Freq[256] >= rangeCoder.MaxRange)
		//		Rescale(Freq);
	}
	int32 nPrevF = 0;
	for(int i = 0;i < 256; i++) 
	{
		
		FreqPrev[i + 1] = Freq[i] + nPrevF;
		nPrevF = FreqPrev[i + 1];
	}
	srcFile.seek(0, CommonLib::soFromBegin);

	double dMinSize = 0;
	for(int i = 0;i < 256; i++) 
	{
		if(Freq[i] == 0)
			continue;
		dMinSize += Freq[i] * (-1*Log2((double)Freq[i]/nFileSize));
	}
 
	for(int i = 0;i < 257; i++) 
	{

		dstFile.write((uint32)Freq[i]);
	}

	TRangeEncoder rangeCoder(&dstFile);
	for (uint32 i = 0; i < nFileSize; ++i)
	{

		byte ch= srcFile.readByte();
		rangeCoder.EncodeSymbol(FreqPrev[ch],FreqPrev[ch+1],nFileSize);

	//	for(int j=ch+1;j<257;j++)
	//		Freq[j]++;	
	//	if(Freq[256] >= rangeCoder.MaxRange)
	//		Rescale(Freq);
	}

	rangeCoder.EncodeFinish();
	uint64 nOutFileSize = dstFile.size() - (256 * sizeof(uint32));
	uint64 nMinByteSize = (dMinSize + 7)/8;
	uint64 nError = nOutFileSize - nMinByteSize ;
	double dErr = (double)nError * 100/nMinByteSize;
	srcFile.close();
	dstFile.close();
	std::cout <<"MinByteSize: " << nMinByteSize << " OutSize: " << nOutFileSize << " Diff: " << nError  << " Error: "<< dErr <<" Compress: " <<(double)nFileSize /nOutFileSize << std::endl;
	return nFileSize;
}


void DecompressStaticFile(uint32 nFileSize, const wchar_t *pszCompressFile, const wchar_t* pszFileOut)
{
	CommonLib::CReadFileStream srcFile;
	CommonLib::CWriteFileStream dstFile;
	srcFile.open(pszCompressFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
	dstFile.open(pszFileOut, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);



	uint32 Freq[257];
	uint32 FreqPrev[257];
	for(int i = 0;i < 257; i++) 
	{
		Freq[i] = 0;
		FreqPrev[i] = 0;
	}
	int32 nPrevF = 0;
	for(int i = 0;i < 256; i++) 
	{
		 Freq[i] = srcFile.readIntu32();
		 FreqPrev[i + 1] = Freq[i] + nPrevF;
		 nPrevF = FreqPrev[i + 1];
	}

	double dRowSize = 0;


	srcFile.readIntu32();
	TRangeDecoder rangeDecoder(&srcFile);
	rangeDecoder.StartDecode();
	for (uint32 i = 0; i < nFileSize; ++i)
	{
		unsigned int freq = rangeDecoder.GetFreq(nFileSize);

		byte Symbol;
		for(Symbol = 255;FreqPrev[Symbol] > freq;Symbol--);
		//Symbol--;

		dstFile.write(Symbol);
		rangeDecoder.DecodeSymbol(FreqPrev[Symbol], FreqPrev[Symbol+1], nFileSize);


//		for(int j = Symbol + 1; j < 257; j++)
//			Freq[j]++;
		//if(Freq[256]>=rangeDecoder.MaxRange)
		//	Rescale(Freq);



	}
	srcFile.close();
	dstFile.close();

}
