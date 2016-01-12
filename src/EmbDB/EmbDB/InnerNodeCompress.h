#ifndef _EMBEDDED_DATABASE_FIELD_INNER_NODE_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIELD_INNER_NODE_COMPRESSOR_H_

#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/alloc_t.h"
#include "RBSet.h"
#include "BPVector.h"
#include "Key.h"
#include "CompressorParams.h"
#include "OIDCompress.h"
namespace embDB
{

	class BPInnerNodeFieldCompressor
	{
	public:

		typedef  int64 TKey;
		typedef  int64 TLink;
		typedef  TBPVector<int64> TKeyMemSet;
		typedef  TBPVector<int64> TLinkMemSet;
		typedef CompressorParamsBaseImp TInnerCompressorParams;



		template<typename _Transactions  >
		static TInnerCompressorParams *LoadCompressorParams(_Transactions *pTran)
		{
			return NULL;
		}


		BPInnerNodeFieldCompressor(TKeyMemSet* pKeyMemSet, TLinkMemSet* pLinkMemSet, CommonLib::alloc_t *pAlloc = 0, TInnerCompressorParams *pParams = NULL);
		~BPInnerNodeFieldCompressor();
		bool Load(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryReadStream& stream);
		bool Write(TKeyMemSet& keySet, TLinkMemSet& linkSet, CommonLib::FxMemoryWriteStream& stream);
		

		virtual bool insert(int nIndex, const TKey& key, TLink link );
		virtual bool add(const TKeyMemSet& keySet, const TLinkMemSet& linkSet);
		virtual bool recalc(const TKeyMemSet& keySet, const TLinkMemSet& linkSet);
		virtual bool remove(int nIndex, const TKey& key, TLink link);
		virtual bool update(int nIndex, const TKey& key, TLink link);
		virtual uint32 size() const;
		virtual bool  isNeedSplit(uint32 nPageSize) const;
		virtual uint32 count() const;
		uint32 headSize() const;
		uint32 rowSize() const;
		void clear();
		uint32 tupleSize() const;
		void SplitIn(uint32 nBegin, uint32 nEnd, BPInnerNodeFieldCompressor *pCompressor);
	
	private:
		uint32 m_nSize;
		TKeyMemSet* m_pKeyMemSet;
		TLinkMemSet* m_pLinkMemSet;
		OIDCompress m_OIDCompress;
	};
}

#endif