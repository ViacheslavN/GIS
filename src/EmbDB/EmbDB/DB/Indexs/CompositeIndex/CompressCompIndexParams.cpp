#include "stdafx.h"
#include "CompressCompIndexParams.h"

namespace embDB
{

	CompIndexParams::CompIndexParams() : m_nRowSize(0)
	{}
	CompIndexParams::~CompIndexParams()
	{}

	bool CompIndexParams::load(CommonLib::IReadStream* pStream,  IDBTransaction *pTran)
	{
		 

		return true;

	}
	bool CompIndexParams::save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
	{
 

		return true;
	}

	void CompIndexParams::addParams(eDataTypes type, uint32 nSize)
	{
		m_vecScheme.push_back(type);
		m_vecSchemeParams.push_back(std::make_pair(type, nSize));
	}
}
