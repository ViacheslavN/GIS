#ifndef _EMBEDDED_DATABASE_MULTI_INDEX_LEAF_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_MULTI_INDEX_LEAF_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "utils/compress/UnsignedNumLenDiffCompressor2.h"
#include "EmptyMultiKeyCompress.h"
namespace embDB
{

	template<typename _TKey, class _Transaction = IDBTransaction,
	class _TKeyCompress = TEmptyMultiKeyCompress<_TKey> >
	class BPLeafNodeMultiIndexCompressor
	{
	public:	
		typedef _TKey TKey;
		typedef IndexTuple<TKey> TIndex;
		typedef TBPVector<TIndex> TLeafMemSet;
		typedef CompressorParamsBaseImp TLeafCompressorParams;
		typedef _TKeyCompress TKeyCompress;
		TKeyCompress m_KeyCompressor;

		template<typename _Transactions  >
		static TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TLeafCompressorParams();
		}

		BPLeafNodeMultiIndexCompressor(uint32 nPageSize, _Transaction *pTran, CommonLib::alloc_t *pAlloc, TLeafCompressorParams *pParams, TLeafMemSet *pLeafMemSet) : 
		m_nCount(0),	m_nPageSize(nPageSize), m_KeyCompressor(pAlloc, nPageSize, pParams), m_pLeafMemSet(pLeafMemSet)
		{}
		virtual ~BPLeafNodeMultiIndexCompressor(){}
		virtual bool Load(TLeafMemSet& Set, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			uint32 nKeySize = stream.readInt32();

			Set.reserve(m_nCount);
 

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);

			return m_KeyCompressor.decompress(m_nCount, Set, &KeyStreams);
 
		}
		virtual bool Write(TLeafMemSet& Set, CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nCount == Set.size());
			stream.write(m_nCount);
			if(!m_nCount)
				return true;
			uint32 nKeySize = m_KeyCompressor.GetCompressSize();
			stream.write(nKeySize);

			CommonLib::FxMemoryWriteStream KeyStreams;
			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);

			m_KeyCompressor.compress(Set, &KeyStreams);

			stream.seek(stream.pos() + nKeySize, CommonLib::soFromBegin);
			/*for(uint32 i = 0, sz = Set.size(); i < sz; ++i)
			{
				KeyStreams.write(Set[i].m_key);
				KeyStreams.write(Set[i].m_nRowID);
			}*/

			return true;
		}

		virtual bool insert(int nIndex, const TIndex& key)
		{
			m_nCount++;
			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *m_pLeafMemSet);
			return true;
		}
		virtual bool add(const TLeafMemSet& Set)
		{
			m_nCount += Set.size();

			if(!Set.empty())
			{
				this->m_KeyCompressor.AddDiffSymbol(Set[0], (*this->m_pLeafMemSet)[this->m_nCount - 1]);
			}


			for (uint32 i = 1, sz = Set.size(); i < sz; ++i)
			{
				this->m_KeyCompressor.AddDiffSymbol(Set[i], Set[i - 1]);
			}

			return true;
		}
		virtual bool recalc(const TLeafMemSet& Set)
		{
			this->m_nCount = this->m_pLeafMemSet->size();
			this->m_KeyCompressor.clear();
			for (uint32 i = 1, sz = this->m_pLeafMemSet->size(); i < sz; 	++i)
			{
				this->m_KeyCompressor.AddDiffSymbol((*this->m_pLeafMemSet)[i], (*this->m_pLeafMemSet)[i - 1]); 
			}
			return true;
		}
		virtual void recalc()
		{
			this->m_nCount = this->m_pLeafMemSet->size();
			this->m_KeyCompressor.clear();
			for (uint32 i = 1, sz = this->m_pLeafMemSet->size(); i < sz; 	++i)
			{
				this->m_KeyCompressor.AddDiffSymbol((*this->m_pLeafMemSet)[i], (*this->m_pLeafMemSet)[i - 1]); 
			}
		}


		virtual bool update(int nIndex, const TIndex& key)
		{
			return true;
		}
		virtual bool remove(int nIndex, const TIndex& key)
		{
			m_nCount--;
			this->m_KeyCompressor.RemoveSymbol(this->m_nCount, nIndex, key, *(this->m_pLeafMemSet));
			return true;
		}
		virtual uint32 size() const
		{
			return rowSize() +  headSize();
		}
		virtual bool isNeedSplit() const
		{
			return !(m_nPageSize > size()) || m_nCount > 2 *m_nPageSize;
		}
		virtual uint32 count() const
		{
			return m_nCount;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32) + sizeof(uint32); //count + compsize
		}
		uint32 rowSize() const
		{
			return m_KeyCompressor.GetCompressSize();
		}
		uint32 tupleSize() const
		{
			return  sizeof(TIndex) + sizeof(int64);
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPLeafNodeMultiIndexCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
			if(bRecalcSrc)
				recalc();
			if(bRecalcDst)
				pCompressor->recalc();

		}

		bool IsHaveUnion(BPLeafNodeMultiIndexCompressor *pCompressor) const
		{
			return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPLeafNodeMultiIndexCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TIndex) + sizeof(int64) );
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TIndex) + sizeof(int64));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
		TLeafMemSet *m_pLeafMemSet;
	};
}

#endif