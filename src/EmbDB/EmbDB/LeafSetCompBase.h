#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_COMPRESSOR_BASE_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_COMPRESSOR_BASE_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TKey>
	class BPLeafNodeSetSimpleCompressorV2
	{
	public:	
		typedef _TKey TKey;
		typedef  TBPVector<TKey> TLeafMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;

		TLeafMemSet m_LeafMemSet;

		BPLeafNodeSetSimpleCompressorV2(CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL) : m_nCount(0)
		{}

	
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}
		virtual ~BPLeafNodeSetSimpleCompressorV2(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			Set.reserve(m_nCount);

			/*uint32 nKeySize = stream.readInt32();*/
			uint32 nKeySize =  m_nCount * sizeof(TKey);
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			TKey nkey;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
			{
				KeyStreams.read(nkey);
				Set.push_back(nkey);
			}

			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nCount == Set.size());
			stream.write(m_nCount);
			if(!m_nCount)
				return true;
			uint32 nKeySize =  m_nCount * sizeof(TKey);
			/*stream.write(nKeySize);*/
			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			for(size_t i = 0, sz = Set.size(); i < sz; ++i)
			{
				KeyStreams.write(Set[i]);
			}

			return true;
		}

		virtual bool insert(int nIndex, const TKey& key)
		{
			m_nCount++;
			return true;
		}
		virtual bool add(const TLeafMemSet& Set)
		{
			m_nCount += Set.size();
			return true;
		}
		virtual bool recalc(const TLeafMemSet& Set)
		{
			m_nCount = Set.size();
			return true;
		}
		virtual bool update(int nIndex, const TKey& key)
		{
			return true;
		}
		virtual bool remove(int nIndex, const TKey& key)
		{
			m_nCount--;
			return true;
		}
		virtual size_t size() const
		{
			return sizeof(TKey) *  m_nCount +  sizeof(uint32);
		}
		virtual bool isNeedSplit(uint32 nPageSize) const
		{
			return nPageSize < size();
		}
		virtual size_t count() const
		{
			return m_nCount;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize() const
		{
			return sizeof(TKey) *  m_nCount;
		}
		size_t tupleSize() const
		{
			return  sizeof(TKey);
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeSetSimpleCompressorV2 *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}
	private:
		size_t m_nCount;
	};
}

#endif