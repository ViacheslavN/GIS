#pragma once
#include "stdafx.h"
#include "Test.h"
#include "CommonLibrary/BitStream.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/FileStream.h"
#include "ACEncoder.h"
#include "ACDecoder.h"
#include "RangeCoder.h"
#include <iostream>

double Log2( double n );


template<class _TCodeValue, uint16 _nValueBit >
class TestCompressor
{
public:
	static const uint32 _nFileBufSize = 5 *1024*1024;
	typedef _TCodeValue TCodeValue;
	typedef embDB::TACEncoder<TCodeValue, _nValueBit/2> TACEncoder;
	typedef embDB::TACDecoder<TCodeValue, _nValueBit/2> TACDecoder;
	typedef embDB::TRangeEncoder<TCodeValue, _nValueBit> TRangeEncoder; 
	typedef embDB::TRangeDecoder<TCodeValue, _nValueBit> TRangeDecoder; 

	//CommonLib::MemoryStream m_pReadStream;
	CommonLib::MemoryStream m_pWriteStream;

	TestCompressor() : m_nBufSize(0)
	{
		m_nFileBuf = new byte[_nFileBufSize];
		//m_pReadStream.create(_nFileBufSize);
	}
	~TestCompressor()
	{
		delete m_nFileBuf;
	}
	void Rescale(uint32 *Frequency) {
		for(int i = 1; i <= 256;i++) {
			Frequency[i]/=2;
			if(Frequency[i]<=Frequency[i-1]) Frequency[i]=Frequency[i-1]+1;
		}
	}
	bool ReadFromFile(CommonLib::CReadFileStream* pFileStream)
	{
		int64 nFileSize = pFileStream->size();
		int64 nFilePos = pFileStream->pos();

		int64 nFree = nFileSize - nFilePos;
		if(nFree == 0)
			return false;

		m_nBufSize = _nFileBufSize < nFree ? _nFileBufSize : nFree;
		pFileStream->read(m_nFileBuf, m_nBufSize);
		return true;
	}

	template<class TCoder>
	int64 compressDynamicModel(const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		m_nBufSize = 0;
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);



		uint64 nFileSize = srcFile.size();

		uint32 CalcFreq[257];
		uint32 Freq[257];
		for(int i = 0;i < 257; i++) 
		{
			Freq[i] = i;
			CalcFreq[i] = 0;
		}

		TCoder coder(&m_pWriteStream);
		int64 nMax = coder.MaxRange;
		while(ReadFromFile(&srcFile))
		{
			for (uint32 i = 0; i < m_nBufSize; ++i)
			{
				byte ch= m_nFileBuf[i];
				CalcFreq[ch] += 1;
				coder.EncodeSymbol(Freq[ch],Freq[ch+1], Freq[256]);

				for(int j=ch+1;j<257;j++)
					Freq[j]++;	
				if(Freq[256] >= coder.MaxRange)
					Rescale(Freq);
			}

			if(m_pWriteStream.pos() > _nFileBufSize)
			{
				dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
				m_pWriteStream.seek(0, CommonLib::soFromBegin);
			}
		}
		coder.EncodeFinish();
		if(m_pWriteStream.pos())
		{
			dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
			m_pWriteStream.seek(0, CommonLib::soFromBegin);
		}


		dstFile.flush();
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
		uint64 nMinByteSize = (uint64)(dMinSize + 7)/8;
		int64 nError = nOutFileSize - nMinByteSize;
		double dErr = (double)nError * 100/nMinByteSize;

		std::cout <<"MinByteSize: " << nMinByteSize << " OutSize: " << nOutFileSize  << " Diff: " << nError << " Error: "<< dErr <<" Compress: " <<(double)nFileSize/nOutFileSize <<std::endl;
		return nFileSize;
	}


	template<class TDecoder>
	void decompessDynamicModel(uint64 nFileSize, const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		m_nBufSize = 0;
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		CommonLib::MemoryStream stream; 

	

		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);


		if(!ReadFromFile(&srcFile))
			return;

		stream.attach(m_nFileBuf, m_nBufSize);

		uint32 Freq[257];
		for(int i = 0;i < 257; i++) 
			Freq[i] = i;

		TDecoder coder(&stream);

		int64 nReadSymbols = 0;
		for(;;)
		{
			while (!stream.IsEndOfStream())
			{
				unsigned int freq = coder.GetFreq(Freq[256]);

				byte Symbol;
				for(Symbol=255;Freq[Symbol] > freq;Symbol--);
				//Symbol--;

				nReadSymbols++;
				m_pWriteStream.write(Symbol);
				if(nReadSymbols == nFileSize)
					break;
				coder.DecodeSymbol(Freq[Symbol], Freq[Symbol+1], Freq[256]);


				for(int j = Symbol + 1; j < 257; j++)
					Freq[j]++;
				if(Freq[256]>=coder.MaxRange)
					Rescale(Freq);

			}

			if(m_pWriteStream.pos() > _nFileBufSize)
			{
				dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
				m_pWriteStream.seek(0, CommonLib::soFromBegin);
			}
			if(nReadSymbols == nFileSize)
				break;
			ReadFromFile(&srcFile);
			stream.attach(m_nFileBuf, m_nBufSize);
		}
 
		if(m_pWriteStream.pos())
		{
			dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
			m_pWriteStream.seek(0, CommonLib::soFromBegin);
		}

		srcFile.close();
		dstFile.close();
	}
private:
	byte* m_nFileBuf;
	uint32 m_nBufSize;

};

typedef TestCompressor<uint32, 16> TTestCompressor32;
typedef TestCompressor<uint64, 64> TTestCompressor64;


void CTestCompess::compressFile(const wchar_t* pszFileName, const wchar_t* pszFileOut)
{
	TTestCompressor64 compress64;
	TTestCompressor64::TRangeEncoder coder(NULL);
	int64 nMax = coder.MaxRange;
	int64 nFileSize = compress64.compressDynamicModel<TTestCompressor64::TRangeEncoder>(pszFileName, pszFileOut, L"RangeEncoder64");
	compress64.decompessDynamicModel<TTestCompressor64::TRangeDecoder>(nFileSize, pszFileOut, L"D:\\2\\1.log.decompress", L"RangeEncoder64");
}