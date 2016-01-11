#include "stdafx.h"
#include "InnerNodeCompress.h"

namespace embDB
{
	
		BPInnerNodeFieldCompressor::BPInnerNodeFieldCompressor(TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc, TInnerCompressorParams *pParams) : 
		m_pKeyMemSet(pKeyMemSet), m_pLinkMemSet(pLinkMemSet), m_nSize(0)
		{}
		BPInnerNodeFieldCompressor:: ~BPInnerNodeFieldCompressor(){}
		bool BPInnerNodeFieldCompressor::Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
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
		bool BPInnerNodeFieldCompressor::Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
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

		bool BPInnerNodeFieldCompressor::insert(int nIndex, const TKey& key, TLink link )
		{
			m_nSize++;
			if(nIndex != 0)
				m_OIDCompress.AddSymbol(key - (*m_pKeyMemSet)[nIndex - 1]);
			return true;
		}
		bool BPInnerNodeFieldCompressor::add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], linkSet[i]);
			}
			return true;
		}
		bool BPInnerNodeFieldCompressor::recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = 0;
		
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i, keySet[i], linkSet[i]);
			}

			return true;
		}
		bool BPInnerNodeFieldCompressor::remove(int nIndex, const TKey& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		bool BPInnerNodeFieldCompressor::update(int nIndex, const TKey& key, TLink link)
		{
			return true;
		}
		uint32 BPInnerNodeFieldCompressor::size() const
		{
			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();


			uint32 nCompSize = m_OIDCompress.GetRowSize();

			return nRowSize + nHeadSize;
		}
		bool  BPInnerNodeFieldCompressor::isNeedSplit(uint32 nPageSize) const
		{
			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();

			if(nPageSize > (nRowSize + nHeadSize))
			{
				return false;
			}

			return nPageSize < size();
		}
		uint32 BPInnerNodeFieldCompressor::count() const
		{
			return m_nSize;
		}
		uint32 BPInnerNodeFieldCompressor::headSize() const
		{
			return  sizeof(uint32);
		}
		uint32 BPInnerNodeFieldCompressor::rowSize() const
		{
			return (sizeof(TKey) + sizeof(TLink)) *  m_nSize;
		}
		void BPInnerNodeFieldCompressor::clear()
		{
			m_nSize = 0;
		}
		uint32 BPInnerNodeFieldCompressor::tupleSize() const
		{
			return  (sizeof(TKey) + sizeof(TLink));
		}

		void BPInnerNodeFieldCompressor::SplitIn(uint32 nBegin, uint32 nEnd, BPInnerNodeFieldCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;

			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
}