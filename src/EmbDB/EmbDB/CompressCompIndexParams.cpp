#include "stdafx.h"
#include "CompressCompIndexParams.h"

namespace embDB
{

	CompIndexParams::CompIndexParams() : m_nRowSize(0)
	{}
	CompIndexParams::~CompIndexParams()
	{}

	bool CompIndexParams::read(IDBTransactions *pTran)
	{

		FilePagePtr pPage = pTran->getFilePage(getRootPage());
		if(!pPage.get())
			return false;//TO DO log error
		CommonLib::FxMemoryReadStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream);
		if(!header.isValid())
			return false; //TO DO log error

		if(header.m_nObjectPageType != INDEX_PAGE || header.m_nSubObjectPageType != COMP_INDEX_INFO_PAGE)
			return false; //TO DO log error

		uint32 nCount = stream.readIntu32();
		for (size_t i = 0; i < nCount; ++i)
		{
			short nType = stream.readintu16();
			assert(nType >= ftUnknown && nType <= _countof(arrTypeSizes));
			uint32 nFieldSize = arrTypeSizes[nType].nSize;
			if(nType == ftString)
				nFieldSize = stream.readIntu32();
			m_nRowSize += nFieldSize;
			m_vecScheme.push_back(nType);
			m_vecSchemeParams.push_back(std::make_pair(nType, nFieldSize));
		}

		return true;

	}
	bool CompIndexParams::save(IDBTransactions *pTran)
	{
		FilePagePtr pPage = pTran->getFilePage(getRootPage());
		if(!pPage.get())
			return false;//TO DO log error
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, INDEX_PAGE, COMP_INDEX_INFO_PAGE);
		stream.write(uint32(m_vecScheme.size()));
		for (size_t i = 0, sz = m_vecScheme.size(); i < sz; ++i)
		{
			stream.write(m_vecScheme[i]);
			if(m_vecScheme[i] == ftString)
			{
				stream.write(m_vecSchemeParams[i].second);
			}
		}

		header.writeCRC32(stream);
		pTran->saveFilePage(pPage);

		return true;
	}

	void CompIndexParams::addParams(eDataTypes type, uint32 nSize)
	{
		m_vecScheme.push_back(type);
		m_vecSchemeParams.push_back(std::make_pair(type, nSize));
	}
}
