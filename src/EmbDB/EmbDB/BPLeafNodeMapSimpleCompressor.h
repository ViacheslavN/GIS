#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_MAP_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "BPCompressors.h"
namespace embDB
{

	template<typename _TLeafMemSet  >
	class BPLeafNodeMapSimpleCompressor 
	{
	public:
		typedef _TLeafMemSet TLeafMemSet;
		typedef typename _TLeafMemSet::TTreeNode  TTreeNode;
		typedef typename _TLeafMemSet::TKey TKey;
		typedef typename _TLeafMemSet::TValue TValue;

		BPLeafNodeMapSimpleCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPLeafNodeMapSimpleCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			uint32 nValSize = stream.readInt32();

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TKey nkey;
			TValue nval;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				ValStreams.read(nval);
				Set.insert(nkey, nval);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == Set.size());
			uint32 nSize = (uint32)Set.size();
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream ValStreams;

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nValSize =  nSize * sizeof(TValue);

			stream.write(nKeySize);
			stream.write(nValSize);
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			ValStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nValSize);

			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
				ValStreams.write(it.value());
			}
			assert((stream.pos() + KeyStreams.pos() +  ValStreams.pos())< stream.size());
			return true;
		}

		virtual bool insert(TTreeNode *pObj)
		{
			m_nSize++;
			return true;
		}
		virtual bool update(TTreeNode *pObj,  const TValue& nValue)
		{
			return true;
		}
		virtual bool remove(TTreeNode *pObj)
		{
			m_nSize--;
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nSize + 3 * sizeof(uint32);
		}
		virtual bool isNeedSplit(uint32 nPageSize) const
		{
			return nPageSize < size();
		}
		virtual uint32 count() const
		{
			return m_nSize;
		}
		uint32 headSize() const
		{
			return  3 * sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return (sizeof(TKey) + sizeof(TValue)) *  m_nSize;
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TValue));
		}
	private:
		uint32 m_nSize;
	};
}

#endif