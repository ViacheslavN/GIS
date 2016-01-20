#pragma once
#include "stdafx.h"
#include "Test.h"
#include "CommonLibrary/BitStream.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/FileStream.h"
#include "CommonLibrary/DebugTime.h"
#include "ACEncoder.h"
#include "ACDecoder.h"
#include "RangeCoder.h"
#include <iostream>
#include <vector>

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

	CommonLib::MemoryStream m_pReadStream;
	CommonLib::MemoryStream m_pWriteStream;

	TestCompressor()
	{
		 m_pReadStream.create(_nFileBufSize);
		 m_pWriteStream.create(_nFileBufSize);
	}
	~TestCompressor()
	{
	
	}
	void Rescale(uint32 *Frequency) {
		for(int i = 1; i <= 256;i++) {
			Frequency[i]/=2;
			if(Frequency[i]<=Frequency[i-1]) Frequency[i]=Frequency[i-1]+1;
		}
	}
	
	template<class TCoder>
	int64 compressDynamicModel(const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);



		uint64 nFileSize = srcFile.size();

		if(m_pReadStream.size() < nFileSize)
			m_pReadStream.resize(nFileSize);


		srcFile.read(m_pReadStream.buffer(), nFileSize);

		m_pReadStream.seek(0, CommonLib::soFromBegin);
		m_pWriteStream.seek(0, CommonLib::soFromBegin);

		uint32 CalcFreq[257];
		uint32 Freq[257];
		for(int i = 0;i < 257; i++) 
		{
			Freq[i] = i;
			CalcFreq[i] = 0;
		}

		TCoder coder(&m_pWriteStream);
		int64 nMax = coder.MaxRange;


		for (uint32 i = 0; i < nFileSize; ++i)
		{
			byte ch= m_pReadStream.readByte();
			CalcFreq[ch] += 1;
			coder.EncodeSymbol(Freq[ch],Freq[ch+1], Freq[256]);

			for(int j=ch+1;j<257;j++)
				Freq[j]++;	
			if(Freq[256] >= coder.MaxRange)
				Rescale(Freq);
		}

		coder.EncodeFinish();
	

		dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
		
	
	
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

		std::wcout <<pszDesc <<" MinByteSize: " << nMinByteSize << " OutSize: " << nOutFileSize  << " Diff: " << nError << " Error: "<< dErr <<" Compress: " <<(double)nFileSize/nOutFileSize <<std::endl;
		return nFileSize;
	}


	template<class TDecoder>
	void decompessDynamicModel(uint64 nFileSize, const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		CommonLib::MemoryStream stream; 

	

		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);


		uint64 nCompFileSize = srcFile.size();

		if(m_pReadStream.size() < nCompFileSize)
			m_pReadStream.resize(nCompFileSize);

		m_pReadStream.seek(0, CommonLib::soFromBegin);
		m_pWriteStream.seek(0, CommonLib::soFromBegin); 


		srcFile.read(m_pReadStream.buffer(), nCompFileSize);

		uint32 Freq[257];
		for(int i = 0;i < 257; i++) 
			Freq[i] = i;

		TDecoder coder(&m_pReadStream);
		coder.StartDecode();

		for (size_t i = 0; i < nFileSize; ++i)
		{
			uint32 freq = coder.GetFreq(Freq[256]);
			byte Symbol;
			for(Symbol=255;Freq[Symbol] > freq;Symbol--);
			m_pWriteStream.write(Symbol);
		 
			coder.DecodeSymbol(Freq[Symbol], Freq[Symbol+1], Freq[256]);


			for(int j = Symbol + 1; j < 257; j++)
				Freq[j]++;
			if(Freq[256]>=coder.MaxRange)
				Rescale(Freq);
		}

		dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
		m_pWriteStream.seek(0, CommonLib::soFromBegin);


		srcFile.close();
		dstFile.close();
	}




	template<class TCoder>
	int64 compressStaticModel(const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);



		uint64 nFileSize = srcFile.size();

		if(m_pReadStream.size() < nFileSize)
			m_pReadStream.resize(nFileSize);


		srcFile.read(m_pReadStream.buffer(), nFileSize);

		m_pReadStream.seek(0, CommonLib::soFromBegin);
		m_pWriteStream.seek(0, CommonLib::soFromBegin);


		uint32 Freq[257];
		uint32 FreqPrev[257];
		for(int i = 0;i < 257; i++) 
		{
			Freq[i] = 0;
			FreqPrev[i] = 0;
		}


		for (uint32 i = 0; i < nFileSize; ++i)
		{
			byte ch = m_pReadStream.readByte();
			Freq[ch]++;
		
		}
		int32 nPrevF = 0;
		for(int i = 0;i < 256; i++) 
		{

			FreqPrev[i + 1] = Freq[i] + nPrevF;
			nPrevF = FreqPrev[i + 1];
		}
		srcFile.seek(0, CommonLib::soFromBegin);
		m_pReadStream.seek(0, CommonLib::soFromBegin);

		
		double dMinSize = 0;
		for(int i = 0;i < 256; i++) 
		{
			if(Freq[i] == 0)
				continue;
			dMinSize += Freq[i] * (-1*Log2((double)Freq[i]/nFileSize));
		}

		for(int i = 0;i < 257; i++) 
		{

			m_pWriteStream.write((uint32)Freq[i]);
		}
		
		
		
		TCoder coder(&m_pWriteStream);
		int64 nMax = coder.MaxRange;




		for (uint32 i = 0; i < nFileSize; ++i)
		{
			byte ch= m_pReadStream.readByte();
			coder.EncodeSymbol(FreqPrev[ch], FreqPrev[ch+1], nFileSize);
		}
		coder.EncodeFinish();
		dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
		dstFile.flush();
		uint64 nOutFileSize = dstFile.size() - (256 * sizeof(uint32));;

		srcFile.close();
		dstFile.close();
		
		uint64 nMinByteSize = (uint64)(dMinSize + 7)/8;
		int64 nError = nOutFileSize - nMinByteSize;
		double dErr = (double)nError * 100/nMinByteSize;

		std::wcout <<pszDesc <<" MinByteSize: " << nMinByteSize << " OutSize: " << nOutFileSize  << " Diff: " << nError << " Error: "<< dErr <<" Compress: " <<(double)nFileSize/nOutFileSize <<std::endl;
		return nFileSize;
	}


	template<class TDecoder>
	void decompessStaticModel(uint64 nFileSize, const wchar_t *pszSrcFile, const wchar_t *pszOutFile, const wchar_t *pszDesc )
	{
		CommonLib::CReadFileStream srcFile;
		CommonLib::CWriteFileStream dstFile;
		CommonLib::MemoryStream stream; 



		srcFile.open(pszSrcFile, CommonLib::ofmOpenExisting, CommonLib::arRead, CommonLib::smNoMode);
		dstFile.open(pszOutFile, CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);


		uint64 nCompFileSize = srcFile.size();

		if(m_pReadStream.size() < nCompFileSize)
			m_pReadStream.resize(nCompFileSize);

		m_pReadStream.seek(0, CommonLib::soFromBegin);
		m_pWriteStream.seek(0, CommonLib::soFromBegin); 


		srcFile.read(m_pReadStream.buffer(), nCompFileSize);

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
			Freq[i] = m_pReadStream.readIntu32();
			FreqPrev[i + 1] = Freq[i] + nPrevF;
			nPrevF = FreqPrev[i + 1];
		}

	 


		m_pReadStream.readIntu32();

		TDecoder coder(&m_pReadStream);
		coder.StartDecode();

		for (size_t i = 0; i < nFileSize; ++i)
		{
			unsigned int freq = coder.GetFreq(nFileSize);

			byte Symbol;
			for(Symbol = 255;FreqPrev[Symbol] > freq;Symbol--);
	
			m_pWriteStream.write(Symbol);
			coder.DecodeSymbol(FreqPrev[Symbol], FreqPrev[Symbol+1], nFileSize);
		}

		dstFile.write(m_pWriteStream.buffer(), m_pWriteStream.pos());
		m_pWriteStream.seek(0, CommonLib::soFromBegin);


		srcFile.close();
		dstFile.close();
	}

private:

};

typedef TestCompressor<uint32, 16> TTestCompressor32;
typedef TestCompressor<uint64, 64> TTestCompressor64;


void CTestCompess::compressFile(const wchar_t* pszFileName, const wchar_t* pszFileOut)
{
	TTestCompressor64 compress64;
	TTestCompressor64::TRangeEncoder coder(NULL);
	int64 nMax = coder.MaxRange;



	CommonLib::CString sDRG64CompFile = pszFileName;
	CommonLib::CString sDAC64CompFile = pszFileName;
	CommonLib::CString sSRG64CompFile = pszFileName;
	CommonLib::CString sSAC64CompFile = pszFileName;


	sDRG64CompFile += L".DynamicRangeCode.compress";
	sDAC64CompFile += L".DynamicARCode.compress";
	sSRG64CompFile += L".StaticRangeCode.compress";
	sSAC64CompFile += L".StaticARCode.compress";


	CommonLib::CString sDRG64DecompCompFile = pszFileOut;
	CommonLib::CString sDAC64DecompCompFile = pszFileOut;
	CommonLib::CString sSRG64DecompCompFile = pszFileOut;
	CommonLib::CString sSAC64DecompCompFile = pszFileOut;
	sDRG64DecompCompFile += L".DynamicRangeCode.decompress";
	sDAC64DecompCompFile += L".DynamicARCode.decompress";
	sSRG64DecompCompFile += L".StaticRangeCode.decompress";
	sSAC64DecompCompFile += L".StaticAR.decompress";
 

	std::wcout << L"compress\\decompress file: " << pszFileName << std::endl;

	int64 nFileSize = compress64.compressDynamicModel<TTestCompressor64::TRangeEncoder>(pszFileName, sDRG64CompFile.cwstr(), L"D RC64");
	compress64.decompessDynamicModel<TTestCompressor64::TRangeDecoder>(nFileSize, sDRG64CompFile.cwstr(), sDRG64DecompCompFile.cwstr(), L"D RC64");
	
	compress64.compressDynamicModel<TTestCompressor64::TACEncoder>(pszFileName, sDAC64CompFile.cwstr(), L"D AC64");
	compress64.decompessDynamicModel<TTestCompressor64::TACDecoder>(nFileSize, sDAC64CompFile.cwstr(), sDAC64DecompCompFile.cwstr(), L"D AC64");



	 compress64.compressStaticModel<TTestCompressor64::TRangeEncoder>(pszFileName, sSRG64CompFile.cwstr(), L"S RC64");
	 compress64.decompessStaticModel<TTestCompressor64::TRangeDecoder>(nFileSize, sSRG64CompFile.cwstr(), sSRG64DecompCompFile.cwstr(), L"S RC64");



	 compress64.compressStaticModel<TTestCompressor64::TACEncoder>(pszFileName, sSAC64CompFile.cwstr(), L"S AC64");
	 compress64.decompessStaticModel<TTestCompressor64::TACDecoder>(nFileSize, sSAC64CompFile.cwstr(), sSAC64DecompCompFile.cwstr(), L"S RC64");
}


void CTestCompess::TestCompress(const wchar_t* pszPath)
{
	std::vector<CommonLib::CString> vecFiles;

	CommonLib::CString sPath = pszPath;
	sPath += + L"\\";
	CommonLib::CString sFindPath = sPath + L"*";
	CommonLib::FileSystem::getFiles(sFindPath, vecFiles);
	for (size_t i = 0, sz = vecFiles.size(); i < sz; ++i)
	{
		if(vecFiles[i] == L"." || vecFiles[i] == L".." )
			continue;

		CommonLib::CString sExt = CommonLib::FileSystem::FindFileExtension(vecFiles[i]);
		

		CommonLib::CString sFile = sPath + vecFiles[i];
		CommonLib::CString sCompFile = sFile + L".compress";

		if(sExt == L"compress" || sExt == L"decompress" )
		{
			CommonLib::FileSystem::deleteFile(sFile.cwstr());
			continue;
		}
		compressFile(sFile.cwstr(), sCompFile.cwstr());
	}


}