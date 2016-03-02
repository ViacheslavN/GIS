#ifndef _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_DIFF_COMPRESSOR_2_H_
#define _EMBEDDED_DATABASE_FIELD_BASE_INNER_NODE_DIFF_COMPRESSOR_2_H_

#include "BaseInnerNodeCompress.h"
#include "OIDCompress.h"


namespace embDB
{


	template<class _TKey, class _TKeyCommpressor, class _TLinkCompressor,
	class _TInnerCompressorParams = CompressorParamsBaseImp>
	class TBPBaseInnerNodeDiffCompressor2 : public TBPBaseInnerNodeCompressor<_TKey, _TKeyCommpressor, _TLinkCompressor, _TInnerCompressorParams>
	{
	public:

		typedef TBPBaseInnerNodeCompressor<_TKey, _TKeyCommpressor, _TLinkCompressor> TBase;
		typedef  typename TBase::TKey TKey;
		typedef  int64 TLink;
		typedef  typename TBase::TKeyMemSet TKeyMemSet;
		typedef  typename TBase::TLinkMemSet TLinkMemSet;
		typedef  typename TBase::TKeyCompressor	TKeyCompressor;
		typedef  typename TBase::TLinkCompressor TLinkCompressor;
		typedef  typename TBase::TInnerCompressorParams TInnerCompressorParams;





		TBPBaseInnerNodeDiffCompressor2(uint32 nPageSize,  TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL) : 
		TBase(nPageSize, pKeyMemSet, pLinkMemSet, pAlloc, pParams)
		{

		}
		~TBPBaseInnerNodeDiffCompressor2()
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
				{
					this->m_KeyCompressor.AddDiffSymbol(keySet[i] - keySet[i-1]); 
					this->m_LinkCompressor.AddDiffSymbol(linkSet[i] - linkSet[i-1]);
				}
			}

			return true;
		}

		virtual bool insert(int nIndex, const TKey& key, TLink link )
		{
			m_nCount++;

			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *(this->m_pKeyMemSet));
			m_LinkCompressor.AddSymbol(m_nCount, nIndex, link, *(this->m_pLinkMemSet));

			return true;
		}
		virtual bool remove(int nIndex, const TKey& key, TLink link)
		{
			m_nCount--;
			m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, key, *(this->m_pKeyMemSet));
			m_LinkCompressor.RemoveSymbol(m_nCount, nIndex, link, *(this->m_pLinkMemSet));

			return true;
		}
		virtual bool update(int nIndex, const TKey& key, TLink link)
		{
			m_KeyCompressor.RemoveSymbol(m_nCount, nIndex, (*this->m_pKeyMemSet)[nIndex], *(this->m_pKeyMemSet));
			m_LinkCompressor.RemoveSymbol(m_nCount, nIndex, (*this->m_pLinkMemSet)[nIndex], *(this->m_pLinkMemSet));


			m_KeyCompressor.AddSymbol(m_nCount, nIndex, key, *(this->m_pKeyMemSet));
			m_LinkCompressor.AddSymbol(m_nCount, nIndex, link, *(this->m_pLinkMemSet));
			return true;
		}
	};
}

#endif