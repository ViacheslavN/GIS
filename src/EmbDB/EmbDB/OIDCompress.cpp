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

			dBitRowSize += (it->second * (-1*Log2((double)(it->second)/m_nCount)));

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

		uint32 nByteSize = ((dCodeBit + 7)/8 + (m_CalcNum.GetBitLenSize() + 7)/8);


		

		return nByteSize + 1;
	}
	void OIDCompress::AddSymbol(int64 nDiff)
	{
		TDiffFreq::iterator it =  m_DiffFreq.find(nDiff);
		if(it == m_DiffFreq.end())
		{
			m_DiffFreq.insert(std::make_pair(nDiff, 1));
		}
		else
		{
			it->second++;
		}
		m_nCount++;
		m_CalcNum.AddSymbol(nDiff);
	}
	void OIDCompress::RemoveSymbol(int64 nDiff)
	{
		TDiffFreq::iterator it =  m_DiffFreq.find(nDiff);
		if(it != m_DiffFreq.end())
		{
			it->second--;
		}
		m_nCount--;
		m_CalcNum.RemoveSymbol(nDiff);
	}

	void OIDCompress::compress(TBPVector<int64>& oids, CommonLib::FxBitWriteStream& stream)
	{

	}
}