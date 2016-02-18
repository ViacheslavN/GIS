#include "stdafx.h"
#include "InnerNodeCompress.h"

namespace embDB
{
	
		BPInnerNodeFieldCompressor::BPInnerNodeFieldCompressor(uint32 nPageSize, TOIDMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc, TInnerCompressorParams *pParams) : 
		m_pKeyMemSet(pKeyMemSet), m_pLinkMemSet(pLinkMemSet), m_nCount(0), m_nPageSize(nPageSize)
		{}
		BPInnerNodeFieldCompressor:: ~BPInnerNodeFieldCompressor(){}
		bool BPInnerNodeFieldCompressor::Load(TOIDMemSet& oidSet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream OIDStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nCount = stream.readInt32();
			if(!m_nCount)
				return true;


			oidSet.reserve(m_nCount);
			linkSet.reserve(m_nCount);

			uint32 nRowNoCompSize = m_nCount * (sizeof(TLink) + sizeof(TOID));
			bool bNoComp = nRowNoCompSize < (m_nPageSize - sizeof(uint32));
			uint32 nOIDSize =   0;
			uint32 nLinkSize = 0;


			

			 if(bNoComp)
			 {
				nOIDSize =   m_nCount * (sizeof(TOID));
				nLinkSize =  m_nCount * (sizeof(TLink));
			 }
			 else
			 {
				 nOIDSize =   stream.readInt32();
				 nLinkSize =  stream.readInt32();
			 }


			OIDStreams.attachBuffer(stream.buffer() + stream.pos(), nOIDSize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nOIDSize, nLinkSize);


			if(bNoComp)
			{
				TOID oid;
				TLink nlink;

				for (uint32 nIndex = 0; nIndex < m_nCount; ++nIndex)
				{
					OIDStreams.read(oid);
					LinkStreams.read(nlink);

					oidSet.push_back(oid);
					linkSet.push_back(nlink);


					if(nIndex != 0)
						m_OIDCompressor.AddDiffSymbol(oidSet[nIndex] - oidSet[nIndex - 1]);

					m_LinkCompressor.AddLink(nlink);
				}
			}
			else
			{
				m_OIDCompressor.decompress(m_nCount, oidSet, &OIDStreams);
				m_LinkCompressor.decompress(m_nCount, linkSet, &LinkStreams);
			}

			stream.seek(stream.pos() + nOIDSize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		bool BPInnerNodeFieldCompressor::Write(TOIDMemSet& oidSet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)oidSet.size();
			assert(m_nCount == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;


			uint32 nRowNoCompSize = m_nCount * (sizeof(TLink) + sizeof(TOID));
			bool bNoComp = nRowNoCompSize < (m_nPageSize - sizeof(uint32));
			uint32 nOIDSize =   0;
			uint32 nLinkSize = 0;


			if(bNoComp)
			{
				nOIDSize =   m_nCount * (sizeof(TOID));
				nLinkSize =  m_nCount * (sizeof(TLink));
			}
			else
			{
				nOIDSize = m_OIDCompressor.GetComressSize();			// nSize * sizeof(TKey);
				nLinkSize=  m_LinkCompressor.GetComressSize();

				stream.write(nOIDSize);
				stream.write(nLinkSize);
			}


			CommonLib::FxMemoryWriteStream OIDStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

		
			//uint32 nLinkSize =  nSize * sizeof(int64);


			OIDStreams.attachBuffer(stream.buffer() + stream.pos(), nOIDSize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nOIDSize, nLinkSize);
			stream.seek(stream.pos() + nOIDSize + nLinkSize, CommonLib::soFromBegin);	



			if(bNoComp)
			{
				for(size_t i = 0, sz = oidSet.size(); i < sz; ++i)
				{
					OIDStreams.write(oidSet[i]);
					LinkStreams.write(linkSet[i]);
				}
			}
			else
			{
				m_OIDCompressor.compress(oidSet, &OIDStreams);
				m_LinkCompressor.compress(linkSet, &LinkStreams);
			}
			return true;
		}

		bool BPInnerNodeFieldCompressor::insert(int nIndex, const TOID& oid, TLink link )
		{
			m_nCount++;

			m_OIDCompressor.AddSymbol(m_nCount, nIndex, oid, *m_pKeyMemSet);
			m_LinkCompressor.AddLink(link);
			return true;
		}
		bool BPInnerNodeFieldCompressor::add(const TOIDMemSet& keySet, const TLinkMemSet& linkSet)
		{
			uint32 nOff = m_nCount;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], linkSet[i]);
			}
			return true;
		}
		bool BPInnerNodeFieldCompressor::recalc(const TOIDMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount = keySet.size();
			m_LinkCompressor.clear();
			m_OIDCompressor.clear();
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				if(i != 0)
					m_OIDCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				m_LinkCompressor.AddLink(linkSet[i]);
			}

			return true;
		}
		bool BPInnerNodeFieldCompressor::remove(int nIndex, const TOID& oid, TLink link)
		{
			m_nCount--;
			m_OIDCompressor.RemoveSymbol(m_nCount, nIndex, oid, *m_pKeyMemSet);
			m_LinkCompressor.RemoveLink(link);
			
			return true;
		}
		bool BPInnerNodeFieldCompressor::update(int nIndex, const TOID& key, TLink link)
		{
			return true;
		}
		uint32 BPInnerNodeFieldCompressor::size() const
		{
			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();


			//uint32 nCompSize = m_OIDCompress.GetComressSize();
			//uint32 nLinkSize = m_LinkCompressor.GetComressSize();
			return nRowSize + nHeadSize;
		}
		bool  BPInnerNodeFieldCompressor::isNeedSplit() const
		{

			if(m_nCount > 8192)
				return true;

			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();

			if(m_nPageSize > (nRowSize + nHeadSize))
			{
				return false;
			}

			return true;
		}
		uint32 BPInnerNodeFieldCompressor::count() const
		{
			return m_nCount;
		}
		uint32 BPInnerNodeFieldCompressor::headSize() const
		{
			return  sizeof(uint32) + sizeof(uint32) + sizeof(uint32);
		}
		uint32 BPInnerNodeFieldCompressor::rowSize() const
		{
			//return (sizeof(TKey) + sizeof(TLink)) *  m_nSize;


			uint32 nOIDSize =  m_OIDCompressor.GetComressSize();
			uint32 nLinkSize = m_LinkCompressor.GetComressSize();
			//uint32 nLinkSize = sizeof(TLink) *  m_nSize;

			return nOIDSize + nLinkSize;
		}
		void BPInnerNodeFieldCompressor::clear()
		{
			m_nCount = 0;
			m_OIDCompressor.clear();
			m_LinkCompressor.clear();
		}
		uint32 BPInnerNodeFieldCompressor::tupleSize() const
		{
			return  (sizeof(TOID) + sizeof(TLink));
		}

		void BPInnerNodeFieldCompressor::SplitIn(uint32 nBegin, uint32 nEnd, BPInnerNodeFieldCompressor *pCompressor)
		{
			uint32 nSize = nEnd- nBegin;
		 
			m_nCount -= nSize;
			pCompressor->m_nCount += nSize;
		}

		bool BPInnerNodeFieldCompressor::IsHaveUnion(BPInnerNodeFieldCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TLink));
			uint32 nNoCompSizeUnion = pCompressor->m_nCount * (sizeof(TOID) + sizeof(TLink));

			return (nNoCompSize + nNoCompSizeUnion + tupleSize()) < (m_nPageSize - headSize());


		}
		bool BPInnerNodeFieldCompressor::IsHaveAlignment(BPInnerNodeFieldCompressor *pCompressor) const
		{
			uint32 nNoCompSize = m_nCount * (sizeof(TOID) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize());
		}
}