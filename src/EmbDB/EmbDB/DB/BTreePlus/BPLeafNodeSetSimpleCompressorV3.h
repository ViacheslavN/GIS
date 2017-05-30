#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_LEAF_NODE_SET_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../CompressorParams.h"
#include "../Utils/alloc/STLAlloc.h"
namespace embDB
{

	template<typename _TKey>
	class BPLeafNodeSetSimpleCompressorV3
	{
	public:
		typedef _TKey TKey;
		typedef CompressorParamsBaseImp TLeafCompressorParams;
		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TLeafMemSet;

		BPLeafNodeSetSimpleCompressorV3(uint32 nPageSize,  CommonLib::alloc_t *pAlloc = nullptr, TLeafCompressorParams *pParams = nullptr ) : m_nCount(0),
			m_nPageSize(nPageSize) 
		{}
		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TLeafCompressorParams();
		}
		virtual ~BPLeafNodeSetSimpleCompressorV3() {}

		template<typename _Transactions  >
		bool  init(TLeafCompressorParams *pParams, _Transactions *pTran)
		{
			return true;
		}

		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			m_nCount = stream.readInt32();
			if (!m_nCount)
				return true;

			Set.reserve(m_nCount);

			/*uint32 nKeySize = stream.readInt32();*/
			uint32 nKeySize = m_nCount * sizeof(TKey);
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
			if (!m_nCount)
				return true;
			uint32 nKeySize = m_nCount * sizeof(TKey);
			/*stream.write(nKeySize);*/
			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			for (uint32 i = 0, sz = Set.size(); i < sz; ++i)
			{
				KeyStreams.write(Set[i]);
			}

			return true;
		}

		virtual bool insert(int nIndex,  const TKey& key, const TLeafMemSet& vector)
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
		virtual bool update(int nIndex, const TKey& key, const TLeafMemSet& vector)
		{
			return true;
		}
		virtual bool remove(int nIndex, const TKey& key, const TLeafMemSet& vector)
		{
			m_nCount--;
			return true;
		}
		virtual uint32 size() const
		{
			return sizeof(TKey) *  m_nCount + sizeof(uint32);
		}
		virtual bool isNeedSplit() const
		{
			return m_nPageSize < size();
		}
		virtual uint32 count() const
		{
			return m_nCount;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32);
		}
		uint32 rowSize() const
		{
			return sizeof(TKey) *  m_nCount;
		}
		uint32 tupleSize() const
		{
			return  sizeof(TKey);
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeSetSimpleCompressorV3& pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd - nBegin;

			m_nCount -= nSize;
			pCompressor.m_nCount += nSize;
		}
		bool IsHaveUnion(BPLeafNodeSetSimpleCompressorV3 &pCompressor) const
		{

			uint32 nNoCompSize = m_nCount * (sizeof(TKey));
			uint32 nNoCompSizeUnion = pCompressor.m_nCount * (sizeof(TKey));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPLeafNodeSetSimpleCompressorV3 &pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{

			uint32 nNoCompSize = m_nCount * sizeof(TKey);
			return nNoCompSize < (m_nPageSize - headSize()) / 2;
		}

		void clear()
		{
			m_nCount = 0;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
	};
}

#endif