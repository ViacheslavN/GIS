#pragma once
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "../../EmbDB/Utils/alloc/STLAlloc.h"

	template<typename _TKey >
	class BPInnerNodeSimpleCompressor
	{
	public:

		typedef _TKey TKey;
		typedef  int64 TLink;
		typedef embDB::CompressorParamsBaseImp TInnerCompressorParams;
		typedef embDB::STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;
		typedef std::vector<TLink, TAlloc> TLinkMemSet;
		static const int __nmax_elemen = 5;

		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return new TInnerCompressorParams();
		}

		template<typename _Transactions  >
		bool  init(TInnerCompressorParams *pParams, _Transactions *pTran)
		{
			return true;
		}

		BPInnerNodeSimpleCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc = nullptr, TInnerCompressorParams *pParams = nullptr) :
			m_nCount(0), m_nPageSize(nPageSize)
		{}
		virtual ~BPInnerNodeSimpleCompressor() {}
		virtual bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream)
		{
			CommonLib::FxMemoryReadStream KeyStreams;
			CommonLib::FxMemoryReadStream LinkStreams;

			m_nCount = stream.readInt32();
			if (!m_nCount)
				return true;

			keySet.reserve(m_nCount);
			linkSet.reserve(m_nCount);

			uint32 nKeySize = m_nCount * sizeof(TKey);
			uint32 nLinkSize = m_nCount * sizeof(int64);

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
			if (!nSize)
				return true;

			CommonLib::FxMemoryWriteStream KeyStreams;
			CommonLib::FxMemoryWriteStream LinkStreams;

			uint32 nKeySize = nSize * sizeof(TKey);
			uint32 nLinkSize = nSize * sizeof(int64);

			/*stream.write(nKeySize);
			stream.write(nLinkSize);*/

			KeyStreams.attachBuffer(stream.buffer() + stream.pos(), nKeySize);
			LinkStreams.attachBuffer(stream.buffer() + stream.pos() + nKeySize, nLinkSize);
			stream.seek(stream.pos() + nKeySize + nLinkSize, CommonLib::soFromBegin);
			for (uint32 i = 0, sz = keySet.size(); i < sz; ++i)
			{
				KeyStreams.write(keySet[i]);
				LinkStreams.write(linkSet[i]);
			}

			return true;
		}

		virtual bool insert(int nIndex, const TKey& key, TLink link, const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
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
		virtual bool remove(int nIndex, const TKey& key, TLink link, const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			m_nCount--;
			return true;
		}
		virtual bool update(int nIndex, const TKey& key, TLink link, const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			return true;
		}
		virtual bool updateKey(int nIndex, const TKey& key, const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			return true;
		}
		virtual bool updateLink(int nIndex, TLink link, const TKeyMemSet& keySet, const TLinkMemSet& linkSet)
		{
			return true;
		}
		virtual uint32 size() const
		{
			return (sizeof(TKey) + sizeof(TLink)) *  m_nCount + sizeof(uint32);
		}
		virtual bool isNeedSplit() const
		{
		   return  m_nCount > __nmax_elemen;

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
		uint32 rowSize() const
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

		void SplitIn(uint32 nBegin, uint32 nEnd, BPInnerNodeSimpleCompressor &pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
		{
			

			uint32 nSize = nEnd - nBegin;

			m_nCount -= nSize;
			pCompressor.m_nCount += nSize;
		}
		bool IsHaveUnion(BPInnerNodeSimpleCompressor& pCompressor) const
		{

			return ((m_nCount + 1 + pCompressor.m_nCount) <= __nmax_elemen);

			uint32 nNoCompSize = (m_nCount + 1) * (sizeof(TKey) + sizeof(TLink)); // 1 for less elem
			uint32 nNoCompSizeUnion = pCompressor.m_nCount * (sizeof(TKey) + sizeof(TLink));

			return (nNoCompSize + nNoCompSizeUnion) < (m_nPageSize - headSize());


		}
		bool IsHaveAlignment(BPInnerNodeSimpleCompressor& pCompressor) const
		{
			return m_nCount < __nmax_elemen;

			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize < (m_nPageSize - headSize());
		}
		bool isHalfEmpty() const
		{
			return m_nCount < __nmax_elemen / 2;

			uint32 nNoCompSize = m_nCount * (sizeof(TKey) + sizeof(TLink));
			return nNoCompSize  < (m_nPageSize - headSize()) / 2;
		}
	private:
		uint32 m_nCount;
		uint32 m_nPageSize;
	};


