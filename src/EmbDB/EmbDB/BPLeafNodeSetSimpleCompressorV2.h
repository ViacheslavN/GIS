#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "CompressorParams.h"
namespace embDB
{

	template<typename _TKey	,class _Transaction = IDBTransaction>
	class BPLeafNodeSetSimpleCompressorV2
	{
	public:	
		typedef _TKey TKey;
		typedef  TBPVector<TKey> TLeafMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;

		BPLeafNodeSetSimpleCompressorV2(_Transaction *pTran = 0, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL, TLeafMemSet *pLeafMemSet = NULL) : m_nSize(0)
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
			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			Set.reserve(m_nSize);

			/*uint32 nKeySize = stream.readInt32();*/
			uint32 nKeySize =  m_nSize * sizeof(TKey);
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			TKey nkey;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				KeyStreams.read(nkey);
				Set.push_back(nkey);
			}
			
			return true;
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nSize == Set.size());
			stream.write(m_nSize);
			if(!m_nSize)
				return true;
			uint32 nKeySize =  m_nSize * sizeof(TKey);
			/*stream.write(nKeySize);*/
			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			for(size_t i = 0, sz = Set.size(); i < sz; ++i)
			{
				KeyStreams.write(Set[i]);
			}
			
			return true;
		}

		virtual bool insert(int nIndex, const TKey& key)
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
		virtual bool update(int nIndex, const TKey& key)
		{
			return true;
		}
		virtual bool remove(int nIndex, const TKey& key)
		{
			m_nSize--;
			return true;
		}
		virtual size_t size() const
		{
			return sizeof(TKey) *  m_nSize +  sizeof(uint32);
		}
		virtual bool isNeedSplit(uint32 nPageSize) const
		{
			return nPageSize < size();
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
			return sizeof(TKey) *  m_nSize;
		}
		size_t tupleSize() const
		{
			return  sizeof(TKey);
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeSetSimpleCompressorV2 *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
	private:
		size_t m_nSize;
	};
}

#endif