#include "stdafx.h"
#include "LinkCompress.h"

namespace embDB
{
	InnerLinkCompress::InnerLinkCompress()
	{

	}
	InnerLinkCompress::~InnerLinkCompress()
	{

	}

	void InnerLinkCompress::AddLink(int64 nLink)
	{
		m_compressor.AddSymbol(nLink);
	}
	void InnerLinkCompress::RemoveLink(int64 nLink)
	{
		m_compressor.RemoveSymbol(nLink);
	}
	uint32 InnerLinkCompress::GetComressSize() const
	{
		return m_compressor.GetCompressSize();
	}
	
	void InnerLinkCompress::compress( const embDB::TBPVector<int64>& vecLinks, CommonLib::IWriteStream *pStream)
	{
		m_compressor.compress(vecLinks, pStream);
	}
	void InnerLinkCompress::decompress(uint32 nSize, embDB::TBPVector<int64>& vecLinks, CommonLib::IReadStream *pStream)
	{
		m_compressor.decompress(vecLinks, pStream);
	}
	void InnerLinkCompress::clear()
	{
		m_compressor.clear();
	}
}