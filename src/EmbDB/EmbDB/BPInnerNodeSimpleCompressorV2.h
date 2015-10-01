#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "RBSet.h"
#include "BPVector.h"
#include "Key.h"
#include "BPCompressors.h"
namespace embDB
{

	template<typename _TKey >
	class BPInnerNodeSimpleCompressorV2  
	{
	public:

		typedef _TKey TKey;
		typedef  int64 TLink;
		typedef  TBPVector<TKey> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;


		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			return NULL;
		}


		BPInnerNodeSimpleCompressorV2(CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL) : m_nSize(0)
		{}
		virtual ~BPInnerNodeSimpleCompressorV2(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize * sizeof(TKey);
			uint32 nLinkSize =  m_nSize * sizeof(int64);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(key);
				LinkStreams.read(nlink);

				keySet.push_back(key);
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

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nLinkSize =  nSize * sizeof(int64);

			/*stream.write(nKeySize);
			stream.write(nLinkSize);*/

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				KeyStreams.write(keySet[i]);
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
			return (sizeof(TKey) + sizeof(TLink) ) *  m_nSize + sizeof(uint32) ;
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
			return (sizeof(TKey) + sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TLink));
		}
	private:
		size_t m_nSize;
	};
}

#endif