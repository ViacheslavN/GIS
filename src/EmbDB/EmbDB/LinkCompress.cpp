#include "stdafx.h"
#include "LinkCompress.h"

namespace embDB
{
	InnerLinkCompress::InnerLinkCompress(CommonLib::alloc_t *pAlloc, uint32 nPageSize,CompressorParamsBaseImp *pParams  ) :
		m_compressor(pParams->m_compressType, pParams->m_nErrorCalc, pParams->m_bCalcOnlineSize)
	{

	}
	InnerLinkCompress::~InnerLinkCompress()
	{

	}

	void InnerLinkCompress::AddSymbol(uint32 nSize,  int nIndex, int64 nLink, const embDB::TBPVector<int64>& vecLinks)
	{
		m_compressor.AddSymbol(nLink);
	}
	void InnerLinkCompress::RemoveSymbol(uint32 nSize,  int nIndex, int64 nLink, const embDB::TBPVector<int64>& vecLinks)
	{
		m_compressor.RemoveSymbol(nLink);
	}
	uint32 InnerLinkCompress::GetCompressSize() const
	{
		return m_compressor.GetCompressSize();
	}
	
	void InnerLinkCompress::compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream)
	{
		m_compressor.compress(vecLinks, pStream);
	}
	void InnerLinkCompress::decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream)
	{
		m_compressor.decompress(nSize,vecLinks, pStream);
	}
	void InnerLinkCompress::clear()
	{
		m_compressor.clear();
	}
}