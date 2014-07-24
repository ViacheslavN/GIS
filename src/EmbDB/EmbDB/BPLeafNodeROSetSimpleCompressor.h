#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_SET_RO_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_SET_RO_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "simple_vector.h"
namespace embDB
{

	template<typename _TKey>
	class BPLeafNodeSetROSimpleCompressor
	{
	public:

		typedef typename _TKey TKey;
		typedef  TVectorRO<TKey> TLeafMemSet;
		typedef typename TLeafMemSet TLeafMemSet;


		BPLeafNodeSetROSimpleCompressor() : m_nSize(0)
		{}
		virtual ~BPLeafNodeSetROSimpleCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			TKey nkey;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				Set.push_back(nkey);
			}
			assert(KeyStreams.pos() < stream.size());
			return true;
		}
	private:
		size_t m_nSize;
	};
}

#endif