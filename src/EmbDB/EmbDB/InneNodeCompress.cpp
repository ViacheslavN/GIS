#include "stdafx.h"
#include "InnerNodeCompress.h"

namespace embDB
{
	
		BPInnerNodeFieldCompressor::BPInnerNodeFieldCompressor(TOIDMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc, TInnerCompressorParams *pParams) : 
		m_pKeyMemSet(pKeyMemSet), m_pLinkMemSet(pLinkMemSet), m_nSize(0)
		{}
		BPInnerNodeFieldCompressor:: ~BPInnerNodeFieldCompressor(){}
		bool BPInnerNodeFieldCompressor::Load(TOIDMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =   stream.readInt32();//m_nSize * sizeof(TKey);
			uint32 nLinkSize =  stream.readInt32();//m_nSize * sizeof(int64);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

		//	TKey key;
		//	TLink nlink;

			m_OIDCompressor.decompress(m_nSize, keySet, &KeyStreams);
			m_LinkCompressor.decompress(m_nSize, linkSet, &LinkStreams);
		/*	for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				//KeyStreams.read(key);
				LinkStreams.read(nlink);

				//keySet.push_back(key);
				linkSet.push_back(nlink);
			}*/
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		bool BPInnerNodeFieldCompressor::Write(TOIDMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream)
		{
			uint32 nSize = (uint32)keySet.size();
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize = m_OIDCompressor.GetComressSize();			// nSize * sizeof(TKey);
			uint32 nLinkSize=  m_LinkCompressor.GetComressSize();
			//uint32 nLinkSize =  nSize * sizeof(int64);

			stream.write(nKeySize);
			stream.write(nLinkSize);

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);	

			m_OIDCompressor.compress(keySet, &KeyStreams);
			m_LinkCompressor.compress(linkSet, &LinkStreams);

			/*for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				//KeyStreams.write(keySet[i]);
				LinkStreams.write(linkSet[i]);
			}*/
			

			/*uint32 nByte = m_OIDCompress.GetRowSize();

			CommonLib::FxMemoryWriteStream OIDStreams;

			OIDStreams.create(nByte);

			m_OIDCompress.compress(keySet, &OIDStreams);


			CommonLib::FxMemoryReadStream OIDReadStreams;
			OIDReadStreams.attachBuffer(OIDStreams.buffer(), OIDStreams.size());
			TKeyMemSet oidSet;
			m_OIDCompress.read(m_nSize, oidSet, &OIDReadStreams);*/

			return true;
		}

		bool BPInnerNodeFieldCompressor::insert(int nIndex, const TOID& oid, TLink link )
		{
			m_nSize++;

			m_OIDCompressor.AddSymbol(m_nSize, nIndex, oid, *m_pKeyMemSet);
			m_LinkCompressor.AddLink(link);
			return true;
		}
		bool BPInnerNodeFieldCompressor::add(const TOIDMemSet& keySet, const TLinkMemSet& linkSet)
		{
			uint32 nOff = m_nSize;
			for (uint32 i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				insert(i + nOff, keySet[i], linkSet[i]);
			}
			return true;
		}
		bool BPInnerNodeFieldCompressor::recalc(const TOIDMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
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
			m_nSize--;
			m_OIDCompressor.RemoveSymbol(m_nSize, nIndex, oid, *m_pKeyMemSet);
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
		bool  BPInnerNodeFieldCompressor::isNeedSplit(uint32 nPageSize) const
		{

			if(m_nSize > 8192)
				return true;

			uint32 nRowSize = rowSize();
			uint32 nHeadSize = headSize();

			if(nPageSize > (nRowSize + nHeadSize))
			{
				return false;
			}

			return true;
		}
		uint32 BPInnerNodeFieldCompressor::count() const
		{
			return m_nSize;
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
			m_nSize = 0;
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
		 
			m_nSize -= nSize;
			pCompressor->m_nSize += nSize;
		}
}