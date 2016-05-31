#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_BLOB_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_BLOB_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeMapv2.h"
#include "BlobVal.h"
#include "BlobLeafNodeCompressor.h"
#include "PageAlloc.h"
namespace embDB
{
	template<typename _TKey, typename _Transaction, typename _TCompParams>
	class TBlobLeafNode : public  BPTreeLeafNodeMapv2<_TKey, sBlobVal, _Transaction, BlobLeafNodeCompressor<_Transaction, _TCompParams> >
	{
	public:
		typedef   BPTreeLeafNodeMapv2<_TKey, sBlobVal, _Transaction, BlobLeafNodeCompressor<_Transaction, _TCompParams> > TBase;
		typedef sBlobVal TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams	TLeafCompressorParams;
		typedef TBPVector<sBlobVal>		TValueMemSet;

		TBlobLeafNode( CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
		TBase(pAlloc, bMulti, nPageSize), m_pPageAlloc(NULL)
		{

		}
		~TBlobLeafNode()
		{
			if(this->m_pCompressor)
				this->m_pCompressor->Free();
		}


		bool init(TLeafCompressorParams *pParams, Transaction* pTransaction)
		{
			assert(!this->m_pCompressor);
			this->m_pCompressor = new TCompressor(this->m_nPageSize - 2 * sizeof(TLink),  pTransaction, (CommonLib::alloc_t*)m_pPageAlloc, pParams, &this->m_leafKeyMemSet, &this->m_leafValueMemSet);
			return true;
		}

		bool insertBlob(const TKey& key, const CommonLib::CBlob& blob)
		{

		}
		void SetPageAlloc(CPageAlloc *pPageAlloc)
		{
			assert(pPageAlloc != NULL);
			assert(this->m_pCompressor == NULL);
			m_pPageAlloc = pPageAlloc;
		}
	public:
		CPageAlloc *m_pPageAlloc;


	};

}
#endif