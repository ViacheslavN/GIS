#ifndef _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_DIFF_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_DIFF_COMPRESSOR_H_

#include "BaseInnerNodeCompress.h"
#include "OIDCompress.h"


namespace embDB
{


	template<class _TKey = int64, class _TKeyCommpressor = OIDCompressor, class _TLinkCOmpressor = InnerLinkCompress>
	class TBPBaseInnerNodeDiffCompressor : public TBPBaseInnerNodeCompressor<_TKey, _TKeyCommpressor, _TLinkCOmpressor>
	{
	public:

		typedef TBPBaseInnerNodeCompressor<_TKey, _TKeyCommpressor, _TLinkCOmpressor> TBase;
		typedef  typename TBase::TKey TKey;
		typedef  int64 TLink;
		typedef  typename TBase::TKeyMemSet TKeyMemSet;
		typedef  typename TBase::TLinkMemSet TLinkMemSet;
		typedef  typename TBase::TKeyCommpressor	TKeyCommpressor;
		typedef  typename TBase::TLinkCOmpressor TLinkCOmpressor;
		typedef  typename TBase::TInnerCompressorParams TInnerCompressorParams;


 


		TBPBaseInnerNodeDiffCompressor(uint32 nPageSize,  TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL) : 
			TBase(nPageSize, pKeyMemSet, pLinkMemSet, pAlloc, pParams)
		{

		}
		~TBPBaseInnerNodeDiffCompressor()
		{

		}


		bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStream;
			CommonLib::FxMemoryReadStream LinkStream;

			this->m_nCount = stream.readInt32();
			if(!this->m_nCount)
				return true;


			keySet.reserve(this->m_nCount);
			linkSet.reserve(this->m_nCount);


			uint32	nKeySize =   stream.readIntu32();
			uint32	nLinkSize =  stream.readIntu32();
			


			KeyStream.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStream.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);

			this->m_KeyCompressor.decompress(this->m_nCount, keySet, &KeyStream);
			this->m_LinkCompressor.decompress(this->m_nCount, linkSet, &LinkStream);

			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);		
			return true;
		}
		
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			this->m_nCount = keySet.size();
			this->m_LinkCompressor.clear();
			this->m_KeyCompressor.clear();
			for (size_t i = 0, sz = keySet.size(); i < sz; 	++i)
			{
				if(i != 0)
					this->m_KeyCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
				this->m_LinkCompressor.AddLink(linkSet[i]);
			}

			return true;
		}
	};
}

#endif