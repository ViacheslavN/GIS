#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TLeafMemSet>
	class BPLeafNodeSetSimpleCompressor
	{
	public:			
		typedef typename _TLeafMemSet TLeafMemSet;
		typedef typename TLeafMemSet::TTreeNode TMemSetNode;
		typedef typename TLeafMemSet::TKey TKey;
		BPLeafNodeSetSimpleCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPLeafNodeSetSimpleCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream ValStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize = stream.readInt32();
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			TKey nkey;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				Set.insert(nkey);
			}
			assert(ValStreams.pos() < stream.size());
			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == Set.size());
			stream.write(m_nSize);
			if(!m_nSize)
				return true;
			uint32 nKeySize =  m_nSize * sizeof(TKey);
			stream.write(nKeySize);
			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			TLeafMemSet::iterator it = Set.begin();
			for(; !it.isNull(); ++it)
			{
				KeyStreams.write(it.key());
			}
			assert(KeyStreams.pos() < stream.size());
			return true;
		}

		virtual bool insert(TMemSetNode *pObj)
		{
			m_nSize++;
			return true;
		}
		virtual bool update(TMemSetNode *pObj)
		{
			return true;
		}
		virtual bool remove(TMemSetNode *pObj)
		{
			m_nSize--;
			return true;
		}
		virtual uint32 size() const
		{
			return sizeof(TKey) *  m_nSize +  2*sizeof(uint32);
		}
		virtual uint32 count() const
		{
			return m_nSize;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return sizeof(TKey) *  m_nSize;
		}
		uint32 tupleSize() const
		{
			return  sizeof(TKey);
		}
	private:
		uint32 m_nSize;
	};
}

#endif