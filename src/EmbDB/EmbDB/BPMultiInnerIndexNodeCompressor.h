#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_MULTI_INDEX_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_MULTI_INDEX_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "BPVector.h"
#include "Key.h"
#include "MultiIndexBase.h"
#include "BaseValueDiffCompressor.h"
#include "utils/compress/SignedNumLenDiffCompress.h"
namespace embDB
{

	template<typename _TKey , class _Transaction = IDBTransaction,
	class _TKeyCompress = TEmptyMultiKeyCompress<_TKey> >
	class BPMultiIndexInnerNodeCompressor  
	{
	public:

		typedef _TKey TKey;
		typedef IndexTuple<TKey> TIndex;
	
		typedef int64 TLink;
		typedef TBPVector<TIndex> TKeyMemSet;
		typedef TBPVector<TLink> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;
		typedef TBaseValueDiffCompress<int64, int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress;
		typedef _TKeyCompress TKeyCompress;


		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TInnerCompressorParams();
		}

		BPMultiIndexInnerNodeCompressor(uint32 nPageSize, TKeyMemSet* pKeyMemset, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc, TInnerCompressorParams *pParams ) : m_nCount(0),
			m_nPageSize(nPageSize), m_InnerLinkCompressor(pAlloc, nPageSize, pParams),
			m_KeyCompressor(pAlloc, nPageSize, pParams), m_pkeyMemSet(pKeyMemset), m_pLinkMemSet(pLinkMemSet)

		{}
		virtual ~BPMultiIndexInnerNodeCompressor(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			keySet.reserve(m_nCount);
			linkSet.reserve(m_nCount);


 

			uint32 nKeySize = stream.readIntu32();
			uint32 nLinkSize =  stream.readIntu32();

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

		
			m_KeyCompressor.decompress(m_nCount, keySet, &KeyStreams);
			m_InnerLinkCompressor.decompress(m_nCount, linkSet, &LinkStreams);
		 
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		virtual bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  m_KeyCompressor.GetCompressSize();
			uint32 nLinkSize =  m_InnerLinkCompressor.GetCompressSize();

			stream.write(nKeySize);
			stream.write(nLinkSize);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			
			m_KeyCompressor.compress(keySet, &KeyStreams);
			m_InnerLinkCompressor.compress(linkSet, &LinkStreams);

			
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			return true;
		}

		virtual bool insert(int nIndex, const TIndex& key, TLink link )
		{
			m_nCount++;
			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *m_pkeyMemSet);
			m_InnerLinkCompressor.AddSymbol(m_nCount, nIndex, link, *m_pLinkMemSet);
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount += keySet.size();
	
			if(!keySet.empty())
			{
				m_KeyCompressor.AddDiffSymbol(keySet[0], (*this->m_pkeyMemSet)[this->m_nCount - 1]);
				m_InnerLinkCompressor.AddDiffSymbol(linkSet[0] - m_pLinkMemSet->GetAt(this->m_nCount - 1));
			}


			for (uint32 i = 1, sz = keySet.size(); i < sz; ++i)
			{
				m_KeyCompressor.AddDiffSymbol(keySet[i], keySet[i - 1]);
				m_InnerLinkCompressor.AddDiffSymbol(linkSet[i] - linkSet[i - 1]);
			}
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount = keySet.size();
			m_KeyCompressor.clear();
			m_InnerLinkCompressor.clear();
			for (uint32 i = 1, sz = m_pkeyMemSet->size(); i < sz; 	++i)
			{
				this->m_KeyCompressor.AddDiffSymbol(m_pkeyMemSet->GetAt(i), m_pkeyMemSet->GetAt(i - 1)); 
				this->m_InnerLinkCompressor.AddDiffSymbol(m_pLinkMemSet->GetAt(i)- m_pLinkMemSet->GetAt(i - 1)); 
			}
			return true;
		}


		virtual void recalc()
		{
			m_nCount = m_pkeyMemSet->size();
			m_KeyCompressor.clear();
			m_InnerLinkCompressor.clear();
			for (uint32 i = 1, sz = (uint32)m_pkeyMemSet->size(); i < sz; 	++i)
			{
				this->m_KeyCompressor.AddDiffSymbol(m_pkeyMemSet->GetAt(i), m_pkeyMemSet->GetAt(i - 1)); 
				this->m_InnerLinkCompressor.AddDiffSymbol(m_pLinkMemSet->GetAt(i)- m_pLinkMemSet->GetAt(i - 1)); 
			}
		}
		virtual bool remove(int nIndex, const TIndex& key, TLink link)
		{
			m_nCount--;
			m_KeyCompressor.RemoveSymbol(this->m_nCount, nIndex, key, *(this->m_pkeyMemSet));
			m_InnerLinkCompressor.RemoveSymbol(this->m_nCount, nIndex, link, *(this->m_pLinkMemSet));
			return true;
		}
		virtual bool update(int nIndex, const TIndex& key, TLink link)
		{
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
			return  m_nCount;
		}
		uint32 headSize() const
		{
			return  sizeof(uint32) + sizeof(uint32)+ sizeof(uint32); //count + compsize
		}
		uint32 rowSize() const
		{
			return m_KeyCompressor.GetCompressSize() + m_InnerLinkCompressor.GetCompressSize();
		}
		void clear()
		{
			m_nCount = 0;
			m_KeyCompressor.clear();
			m_InnerLinkCompressor.clear();
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TIndex) + 2* sizeof(TLink));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, BPMultiIndexInnerNodeCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
			if(bRecalcSrc)
				recalc();
			if(bRecalcDst)
				pCompressor->recalc();
		}
		bool IsHaveUnion(BPMultiIndexInnerNodeCompressor *pCompressor) const
		{
			return (rowSize() + pCompressor->rowSize()) < (m_nPageSize - headSize());
		}
		bool IsHaveAlignment(BPMultiIndexInnerNodeCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TIndex) + sizeof(TLink) );
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TIndex) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
		TInnerLinkCompress m_InnerLinkCompressor;
		TKeyCompress    m_KeyCompressor;
		TLinkMemSet *m_pLinkMemSet;
		TKeyMemSet *m_pkeyMemSet;
	};
}

#endif