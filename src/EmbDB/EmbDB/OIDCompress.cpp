#include "stdafx.h"
#include "OIDCompress.h"

namespace embDB
{


	OIDCompress::OIDCompress() : m_nCount(0)
	{

	}
	OIDCompress::~OIDCompress()
	{

	}
	double OIDCompress::GetRowBitSize() const
	{
		double dBitRowSize = 0;
		for (TDiffFreq::const_iterator it = m_DiffFreq.begin(); it != m_DiffFreq.end(); ++it)
		{
			const SymbolInfo& info = it->second;
			dBitRowSize += (info.m_nFreq * (-1*Log2((double)(info.m_nFreq)/m_nCount)));

		}
		if(dBitRowSize < 32)
			dBitRowSize = 32;
		dBitRowSize  += (dBitRowSize *0.05); //error range code 0.05%
		return dBitRowSize;
	}
	uint32 OIDCompress::GetRowSize() const
	{
		double dCodeBit = m_CalcNum.GetCodeBitSize();
		if(dCodeBit < 32)
			dCodeBit = 32;

		dCodeBit  += (dCodeBit *0.05); //error range code 0.05%

		uint32 nByteSize = uint32(((dCodeBit + 7)/8 + (m_CalcNum.GetBitLenSize() + 7)/8));


		

		return nByteSize + 1;
	}

	uint32 OIDCompress::GetNeedByteForDiffCompress() const
	{
		double dCodeBit =  OIDCompress::GetRowBitSize();
		if(dCodeBit < 32)
			dCodeBit = 32;

		dCodeBit  += (dCodeBit *0.05); //error range code 0.05%

		uint32 nByteSize = ((dCodeBit + 7)/8);
		nByteSize += (sizeof(int64) + 1);

		nByteSize += ((m_DiffFreq.size() * (sizeof(int64) + sizeof(short))) + sizeof(short));

		return nByteSize;
	}
	uint32 OIDCompress::GetNeedByteForNumLen() const
	{
		double dCodeBit = m_CalcNum.GetCodeBitSize();
		if(dCodeBit < 32)
			dCodeBit = 32;

		dCodeBit  += (dCodeBit *0.05); //error range code 0.05%

		uint32 nByteSize = ((dCodeBit + 7)/8 + (m_CalcNum.GetBitLenSize() + 7)/8) + 1;
		return nByteSize;
	}
	void OIDCompress::AddSymbol(int64 nDiff)
	{
		SymbolInfo& symInfo =  m_DiffFreq[nDiff];
		symInfo.m_nFreq++;
		m_nCount++;
		m_CalcNum.AddSymbol(nDiff);
	}
	void OIDCompress::RemoveSymbol(int64 nDiff)
	{
		SymbolInfo& symInfo =  m_DiffFreq[nDiff];
		symInfo.m_nFreq--;
		m_nCount--;
		m_CalcNum.RemoveSymbol(nDiff);
	}

	void OIDCompress::compress(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream)
	{
		if(oids.empty())
			return;


		uint32 nNeedByteForDiffSheme = GetNeedByteForDiffCompress();
		uint32 nNeedByteForNumLen= GetNeedByteForNumLen();

		if(nNeedByteForNumLen < nNeedByteForDiffSheme)
		{
			compressNumLen(oids, pStream);
		}
		else
		{
			compressDiffScheme(oids, pStream);
		}
		
	}


	void  OIDCompress::compressDiffScheme(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream)
	{
		pStream->write((byte)eCompressDiff);
		pStream->write(oids[0]);

		TRangeEncoder rgEncode(pStream);

		int32 nPrevF = 0;
		pStream->write((uint16)m_DiffFreq.size());
		for (TDiffFreq::iterator it = m_DiffFreq.begin(); it != m_DiffFreq.end(); ++it)
		{
			SymbolInfo& info = it->second;
			info.m_nB = nPrevF + info.m_nFreq;
			nPrevF = info.m_nB;

			pStream->write(it->first);
			pStream->write(info.m_nFreq);
		}


		for (size_t i = 1, sz = oids.size(); i < sz; ++i)
		{
			int64 nDiff = oids[1] - oids[0];
			TDiffFreq::iterator it =  m_DiffFreq.find(nDiff);
			assert(it == m_DiffFreq.end());
			SymbolInfo& info = it->second;
			int32 nPrevB = info.m_nB - info.m_nFreq;
			rgEncode.EncodeSymbol(nPrevB, info.m_nB, m_nCount);
		}
		rgEncode.EncodeFinish();

	}
	void  OIDCompress::compressNumLen(TBPVector<int64>& oids, CommonLib::IWriteStream* pStream)
	{

	}

	void OIDCompress::read(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream)
	{
		if(nSize == 0)
			return;
		byte nCompSchema = pStream->readByte();
		if(nCompSchema == (byte)eCompressDiff)
		{
			readDiffScheme(nSize, oids, pStream);
		}
	}

	void OIDCompress::readDiffScheme(uint32 nSize, TBPVector<int64>& oids, CommonLib::IReadStream* pStream)
	{
		int64 nOID = pStream->readInt64();
		uint16 nSymbols = pStream->readint16();
		int32 nPrevF = 0;
		for (size_t i = 0; i < nSymbols; ++i)
		{
			int64 nDiff = pStream->readInt64();
			uint16 nFreq = pStream->readint16();

			SymbolInfo info;
			info.m_nB = nPrevF + info.m_nFreq;
			nPrevF = info.m_nB;

			m_DiffFreq.insert(std::make_pair(nDiff, info));
		}
		
		oids.push_back(nOID);

	}
}