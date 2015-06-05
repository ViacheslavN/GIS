#ifndef _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "CompositeIndex.h"
namespace embDB
{

	template<typename _TValue = uint64>
	class BPLeafCompIndexCompressor
	{
	public:	
		typedef _TValue TValue;
		typedef TBPVector<CompositeIndexKey> TLeafKeyMemSet;
		typedef  TBPVector<TValue> TLeafValueMemSet;

		BPLeafCompIndexCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPLeafCompIndexCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * (sizeof(TKey) + sizeof(int64));
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			TIndex index;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(index.m_key);
				KeyStreams.read(index.m_nObjectID);
				Set.push_back(index);
			}

			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == Set.size());
			stream.write(m_nSize);
			if(!m_nSize)
				return true;
			uint32 nKeySize =  m_nSize * (sizeof(TKey) + sizeof(int64));
			/*stream.write(nKeySize);*/
			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			for(size_t i = 0, sz = Set.size(); i < sz; ++i)
			{
				KeyStreams.write(Set[i].m_key);
				KeyStreams.write(Set[i].m_nObjectID);
			}

			return true;
		}

		virtual bool insert(const TIndex& key)
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TLeafMemSet& Set)
		{
			m_nSize += Set.size();
			return true;
		}
		virtual bool recalc(const TLeafMemSet& Set)
		{
			m_nSize = Set.size();
			return true;
		}
		virtual bool update(const TIndex& key)
		{
			return true;
		}
		virtual bool remove(const TIndex& key)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) + sizeof(int64))*  m_nSize +  sizeof(uint32);
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize() const
		{
			return (sizeof(TKey) + sizeof(int64)) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  sizeof(TKey) + sizeof(int64);
		}
	private:
		size_t m_nSize;
	};
}

#endif