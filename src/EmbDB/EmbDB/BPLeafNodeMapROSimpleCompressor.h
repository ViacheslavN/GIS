#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_MAP_RO_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_MAP_RO_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "simple_vector.h"
namespace embDB
{

	template<typename _TKey, typename _TValue>
	class BPLeafNodeMapROSimpleCompressor
	{
	public:

		typedef typename _TKey TKey;
		typedef typename _TValue TValue;
		typedef  TVectorRO<TKey> TKeyMemSet;
		typedef  TVectorRO<TValue> TValueMemSet;
 


		BPLeafNodeMapROSimpleCompressor() : m_nSize(0)
		{}
		virtual ~BPLeafNodeMapROSimpleCompressor(){}
		virtual bool Load(TKeyMemSet& KeySet, TValueMemSet& ValueSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream ValStream;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			KeySet.reserve(m_nSize);
			ValueSet.reserve(m_nSize);


			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStream.attach(stream.buffer() + stream.pos(), nKeySize);
			ValStream.attach(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TKey nkey;
			TValue val;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStream.read(nkey);
				ValStream.read(val);

				KeySet.push_back(nkey);
				ValueSet.push_back(val);
			}
	 
			return true;
		}
	private:
		size_t m_nSize;
	};
}

#endif