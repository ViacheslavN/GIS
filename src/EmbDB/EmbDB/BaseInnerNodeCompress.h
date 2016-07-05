#ifndef _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "RBSet.h"
#include "BPVector.h"
#include "Key.h"
#include "CompressorParams.h"
#include "LinkCompress.h"
#include "EmptyValueCompress.h"
namespace embDB
{


	template<class _TKey = int64, class _TKeyCompressor = TEmptyValueCompress<int64>, class _TLinkCompressor = InnerLinkCompress,
	class _TInnerCompressorParams = CompressorParamsBaseImp
	>
	class TBPBaseInnerNodeCompressor
	{
	public:

		typedef  _TKey TKey;
		typedef  int64 TLink;
		typedef  TBPVector<TKey> TKeyMemSet;
		typedef  TBPVector<int64> TLinkMemSet;
		typedef _TKeyCompressor	TKeyCompressor;
		typedef _TLinkCompressor TLinkCompressor;
		typedef _TInnerCompressorParams TInnerCompressorParams;



		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TInnerCompressorParams();
		}


		TBPBaseInnerNodeCompressor(uint32 nPageSize,  TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL) : 
		m_pKeyMemSet(pKeyMemSet), m_pLinkMemSet(pLinkMemSet), m_nCount(0), m_nPageSize(nPageSize), m_KeyCompressor(pAlloc,nPageSize, pParams), m_LinkCompressor(pAlloc, nPageSize, pParams)
		{

		}
		~TBPBaseInnerNodeCompressor()
		{

		}


		virtual bool IsNoComp() const
		{

			uint32 nRowNoCompSize = m_nCount * (sizeof(TLink) + sizeof(TKey));
			return nRowNoCompSize < (m_nPageSize - sizeof(uint32));
		}

		bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream OIDStreams;
			CommonLib::FxMemoryReadStream LinkStreams;


		

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;


			keySet.reserve(m_nCount);
			linkSet.reserve(m_nCount);


			bool bNoComp = IsNoComp();
			uint32 nOIDSize =   stream.readInt32();
			uint32 nLinkSize = stream.readInt32();

			OIDStreams.attachBuffer(stream.buffer() + stream.pos(), nOIDSize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nOIDSize, nLinkSize);


			m_KeyCompressor.decompress(m_nCount, keySet, &OIDStreams);
			m_LinkCompressor.decompress(m_nCount, linkSet, &LinkStreams);
			stream.seek(stream.pos() + nOIDSize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nCount == nSize);
			assert(!((stream.size() - stream.pos()) < size()));

			stream.write(nSize);
			if(!nSize)
				return true;

			 

			uint32 nKeySize = m_KeyCompressor.GetCompressSize();	
			uint32 nLinkSize =  m_LinkCompressor.GetCompressSize();


		
			stream.write(nKeySize);
			stream.write(nLinkSize);
		

			CommonLib::FxMemoryWriteStream KeyStream;
			CommonLib::FxMemoryWriteStream LinkStream;


			//uint32 nLinkSize =  nSize * sizeof(int64);


			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);	


			m_KeyCompressor.compress(keySet, &KeyStream);
			m_LinkCompressor.compress(linkSet, &LinkStream);
			return true;
		}


		virtual bool insert(int nIndex, const TKey& key, TLink link )
		{
			m_nCount++;

			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.AddSymbol(m_nCount, nIndex, link, *m_pLinkMemSet);

			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			uint32 nOff = m_nCount;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], linkSet[i]);
			}
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount = keySet.size();
			m_LinkCompressor.clear();
			m_KeyCompressor.clear();
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
			
				m_KeyCompressor.AddSymbol(i + 1, i, keySet[i], keySet);
				m_LinkCompressor.AddSymbol(i + 1, i, linkSet[i], linkSet);
			}

			return true;
		}
		virtual bool remove(int nIndex, const TKey& key, TLink link)
		{
			m_nCount--;
			m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.RemoveSymbol(m_nCount, nIndex, link, *m_pLinkMemSet);

			return true;
		}
		virtual bool update(int nIndex, const TKey& key, TLink link)
		{
			m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, (*m_pKeyMemSet)[nIndex], *m_pKeyMemSet);
			m_LinkCompressor.RemoveSymbol(m_nCount, nIndex, (*m_pLinkMemSet)[nIndex], *m_pLinkMemSet);


			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *m_pKeyMemSet);
			m_LinkCompressor.AddSymbol(m_nCount, nIndex, link, *m_pLinkMemSet);
			return true;
		}
		virtual uint32 size() const
		{

			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();

			return nRowSize + nHeadSize;
		}
		virtual bool  isNeedSplit() const
		{
			if(m_nCount > m_nPageSize)
				return true;

			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();

			if(m_nPageSize > (nRowSize + nHeadSize))
			{
				return false;
			}

			return true;
		}
		virtual uint32 count() const
		{
			return m_nCount;
		}
		uint32 headSize() const
		{
				return  sizeof(uint32) + sizeof(uint32) + sizeof(uint32);
		}
		uint32 rowSize() const
		{
			uint32 nOIDSize =  m_KeyCompressor.GetCompressSize();
			uint32 nLinkSize = m_LinkCompressor.GetCompressSize();
			return nOIDSize + nLinkSize;
		}
		void clear()
		{
			m_nCount = 0;
			m_KeyCompressor.clear();
			m_LinkCompressor.clear();
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TLink));
		}
		void SplitIn(uint32 nBegin, uint32 nEnd, TBPBaseInnerNodeCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}


		bool IsHaveUnion(TBPBaseInnerNodeCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TKey) + sizeof(TLink));

			return (nNoCompSize + nNoCompSizeUnion + tupleSize()) < (m_nPageSize - headSize());
		}
		bool IsHaveAlignment(TBPBaseInnerNodeCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize())/2;
		}
	protected:
		uint32 m_nCount;
		TKeyMemSet* m_pKeyMemSet;
		TLinkMemSet* m_pLinkMemSet;
		TKeyCompressor m_KeyCompressor;
		TLinkCompressor m_LinkCompressor;
		uint32 m_nPageSize;
	};
}

#endif