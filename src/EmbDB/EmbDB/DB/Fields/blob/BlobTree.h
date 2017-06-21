#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_BLOB_H_


#include  "../../BTreePlus/BPMap.h"
#include "../BaseFieldEncoders.h"
#include "BlobLeafNodeCompressor.h"
namespace embDB
{
	
	template<class _Transaction, class _TCompParams = BlobFieldCompressorParams>
	class TBPBlobTree : public TBPMap<int64, sBlobVal, comp<int64>, _Transaction, 
		TInnerNodeLinkDiffComp,
		BlobLeafNodeCompressor<_TCompParams> >
	{
	public:

		typedef TBPMap<int64, sBlobVal, comp<int64>, _Transaction,
			TInnerNodeLinkDiffComp, BlobLeafNodeCompressor<_TCompParams> > TBase;


		typedef typename TBase::TBTreeNode	TBTreeNode;
		typedef typename TBase::iterator	iterator;
		typedef typename TBase::TKey	TKey;

		TBPBlobTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

	

	
		void convert(const CommonLib::CBlob& blob, sBlobVal& sValue)
		{
			sValue.m_blob = blob;
			sValue.m_nPage = -1;
			sValue.m_nBeginPos = 0;
		}
		void convert(const sBlobVal& blobVal, CommonLib::CBlob& blob) 
		{
			blob = blobVal.m_blob;
		}

		bool insert(int64 nValue, const CommonLib::CBlob& blob, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::insert(nValue, sValue/*, pFromIterator, pRetItertor*/);

		}
		bool update(const TKey& key, const CommonLib::CBlob& blob)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::update(key, sValue);
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::CBlob& blob, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
		{
			sBlobVal sValue;
			convert(blob, sValue);
			return TBase::insertLast(keyFunctor, sValue, pKey/*, pFromIterator, pRetIterator*/);
		}
	
	private:
		CPageAlloc m_PageAlloc;
 
	};

}

#endif