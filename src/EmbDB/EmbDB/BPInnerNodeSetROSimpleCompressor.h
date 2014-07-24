#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_INNER_NODE_RO_SET_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_INNER_NODE_RO_SET_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "simple_vector.h"
namespace embDB
{

	template<typename _TKey, typename _TLink>
	class BPInnerNodeSetROSimpleCompressor  
	{
	public:


		typedef _TKey TKey;
		typedef _TLink TLink;
		typedef  TVectorRO<TKey> TKeyMemSet;
		typedef  TVectorRO<TLink> TLinkMemSet;

		BPInnerNodeSetROSimpleCompressor() : m_nSize(0)
		{}
		virtual ~BPInnerNodeSetROSimpleCompressor(){}
		virtual bool Load(TKeyMemSet& KeySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			KeySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nLinkSize = stream.readInt32();

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);
				KeySet.push_back(key);
				linkSet.push_back(nlink);
			}
			assert(LinkStreams.pos() < stream.size());
			return true;
		}
	private:
		size_t m_nSize;
	};
}

#endif