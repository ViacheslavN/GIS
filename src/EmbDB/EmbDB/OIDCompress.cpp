#include "stdafx.h"
#include "OIDCompress.h"

namespace embDB
{


	OIDCompress::OIDCompress()
	{

	}
	OIDCompress::~OIDCompress()
	{

	}

	uint32 OIDCompress::GetRowSize()
	{
		return 0;
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

		m_CalcNum.AddSymbol(nDiff);
	}
	void OIDCompress::RemoveSymbol(int64 nDiff)
	{
		TDiffFreq::iterator it =  m_DiffFreq.find(nDiff);
		if(it != m_DiffFreq.end())
		{
			it->second--;
		}

		m_CalcNum.RemoveSymbol(nDiff);
	}
}