#ifndef _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_SIMPLE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_BTREE_PLUS_V2_INNER_NODE_SIMPLE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "RBSet.h"
#include "BPVector.h"
#include "Key.h"
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
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}


		BPInnerNodeSimpleCompressorV2(uint32 nPageSize, TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL) : 
		m_pKeyMemSet(pKeyMemSet), m_pLinkMemSet(pLinkMemSet), m_nCount(0), m_nPageSize(nPageSize)
		{}
		virtual ~BPInnerNodeSimpleCompressorV2(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;

			keySet.reserve(m_nCount);
			linkSet.reserve(m_nCount);

			uint32 nKeySize =  m_nCount * sizeof(TKey);
			uint32 nLinkSize =  m_nCount * sizeof(int64);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			TKey key;
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
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
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * sizeof(TKey);
			uint32 nLinkSize =  nSize * sizeof(int64);

			/*stream.write(nKeySize);
			stream.write(nLinkSize);*/

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			for(uint32 i = 0, sz = keySet.size(); i < sz; ++i)
			{
				KeyStreams.write(keySet[i]);
				LinkStreams.write(linkSet[i]);
			}
			
			return true;
		}

		virtual bool insert(int nIndex, const TKey& key, TLink link )
		{
			m_nCount++;
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount += keySet.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount = keySet.size();
			return true;
		}
		virtual bool remove(int nIndex, const TKey& key, TLink link)
		{
			m_nCount--;
			return true;
		}
		virtual bool update(int nIndex, const TKey& key, TLink link)
		{
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TLink) ) *  m_nCount + sizeof(uint32) ;
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
		uint32 rowSize()
		{
			return (sizeof(TKey) + sizeof(TLink)) *  m_nCount;
		}
		void clear()
		{
			m_nCount = 0;
		}
		uint32 tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TLink));
		}

		void SplitIn(uint32 nBegin, uint32 nEnd, BPInnerNodeSimpleCompressorV2 *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			uint32 nSize = nEnd- nBegin;

			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}
		bool IsHaveUnion(BPInnerNodeSimpleCompressorV2 *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TKey) + sizeof(TLink));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPInnerNodeSimpleCompressorV2 *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{ 
			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize  < (m_nPageSize - headSize())/2;
		}
	private:
		uint32 m_nCount;
		TKeyMemSet* m_pKeyMemSet;
		TLinkMemSet* m_pLinkMemSet;
		uint32 m_nPageSize;
	};
}

#endif