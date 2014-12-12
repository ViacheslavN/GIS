#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_MULTI_INDEX_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_MULTI_INDEX_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "BPVector.h"
#include "Key.h"
#include "BPCompressors.h"
#include "MultiIndexBase.h"
namespace embDB
{

	template<typename _TKey >
	class BPMultiIndexInnerNodeCompressor  
	{
	public:

		typedef _TKey TKey;
		typedef IndexTuple<TKey> TIndex;
	
		typedef int64 TLink;
		typedef TBPVector<TIndex> TKeyMemSet;
		typedef TBPVector<TLink> TLinkMemSet;

		BPMultiIndexInnerNodeCompressor(ICompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPMultiIndexInnerNodeCompressor(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * (sizeof(TKey) + sizeof(int64));
			uint32 nLinkSize =  m_nSize * sizeof(int64);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TIndex index;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(index.key);
				KeyStreams.read(index.nObjectID);

				LinkStreams.read(nlink);

				keySet.push_back(index);
				linkSet.push_back(nlink);
			}
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * (sizeof(TKey) + sizeof(int64));
			uint32 nLinkSize =  nSize * sizeof(int64);

			/*stream.write(nKeySize);
			stream.write(nLinkSize);*/

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				KeyStreams.write(keySet[i].key);
				KeyStreams.write(keySet[i].nObjectID);
				LinkStreams.write(linkSet[i]);
			}
			
			return true;
		}

		virtual bool insert(const TKey& key, TLink link )
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize += keySet.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(const TKey& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(const TKey& key, TLink link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (sizeof(TKey) + 2 * sizeof(TLink) ) *  m_nSize + sizeof(uint32) ;
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize()
		{
			return (sizeof(TKey) + 2 *sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (sizeof(TKey) + 2* sizeof(TLink));
		}
	private:
		size_t m_nSize;
	};
}

#endif