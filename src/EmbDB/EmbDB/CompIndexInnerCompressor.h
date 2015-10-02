#ifndef _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_INNER_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_COMPSITE_UNIQUE_INDEX_INNER_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CompressorParams.h"
#include "CompositeIndexKey.h"
#include "CompressCompIndexParams.h"
#include "BPVectorNoPod.h"
namespace embDB
{

	class BPInnerCompIndexCompressor  
	{
	public:
				
		typedef  int64 TLink;
		typedef TBPVectorNoPOD<CompositeIndexKey> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;
		typedef CompIndexParams TInnerCompressorParams;


		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(int64 nPage, _Transactions *pTran)
		{
			CompIndexParams *pCompParams = new  CompIndexParams();
			pCompParams->setRootPage(nPage);
			pCompParams->read(pTran);
			return pCompParams;
		}


		BPInnerCompIndexCompressor(TKeyMemSet* pKeyMemset, TLinkMemSet* pLinkMemset, CommonLib::alloc_t *pAlloc, TInnerCompressorParams *pParams) 
			: m_nSize(0), m_pAlloc(pAlloc), m_pCompParams(pParams)
		{}
		virtual ~BPInnerCompIndexCompressor(){}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nSize = stream.readInt32();
			if(!m_nSize)
				return true;

			keySet.reserve(m_nSize);
			linkSet.reserve(m_nSize);

			uint32 nKeySize =  m_nSize *  m_pCompParams->getRowSize();
			uint32 nLinkSize =  m_nSize * sizeof(TLink);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
 
			TLink nlink;
			for (uint32 nIndex = 0; nIndex < m_nSize; ++nIndex)
			{
				CompositeIndexKey key(m_pAlloc);
				if(!key.load(m_pCompParams->getScheme(), KeyStreams))
				{
					return false;
				}
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
			assert(m_nSize == nSize);
			stream.write(nSize);
			if(!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize =  nSize * m_pCompParams->getRowSize();
			uint32 nLinkSize =  nSize * sizeof(TLink);

			KeyStreams.attach(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attach(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);			 
			for(size_t i = 0, sz = keySet.size(); i < sz; ++i)
			{
				keySet[i].write(KeyStreams);
				LinkStreams.write(linkSet[i]);
			}
			
			return true;
		}

		virtual bool insert(int nIndex, const CompositeIndexKey& key, TLink link )
		{
			m_nSize++;
			return true;
		}
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize += keySet.size();
			return true;
		}
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nSize = keySet.size();
			return true;
		}
		virtual bool remove(int nIndex, const CompositeIndexKey& key, TLink link)
		{
			m_nSize--;
			return true;
		}
		virtual bool update(int nIndex, const CompositeIndexKey& key, TLink link)
		{
			return true;
		}
		virtual size_t size() const
		{
			return (m_pCompParams->getRowSize() + sizeof(TLink) ) *  m_nSize + sizeof(uint32) ;
		}
		virtual size_t count() const
		{
			return m_nSize;
		}
		size_t headSize() const
		{
			return  sizeof(uint32);
		}
		size_t rowSize()
		{
			return (m_pCompParams->getRowSize() + sizeof(TLink)) *  m_nSize;
		}
		void clear()
		{
			m_nSize = 0;
		}
		size_t tupleSize() const
		{
			return  (m_pCompParams->getRowSize() + sizeof(TLink));
		}
	private:
		size_t m_nSize;
		CommonLib::alloc_t* m_pAlloc;
		TInnerCompressorParams* m_pCompParams;
	};
}

#endif