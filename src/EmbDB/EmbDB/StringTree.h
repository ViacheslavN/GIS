#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_

#include "BaseBPMapv2.h"
#include "StringBPNode.h"
#include "PageAlloc.h"
namespace embDB
{


	template<class _TKey, class _Transaction>
	class TBPStringTree : public TBPMapV2<_TKey, sStringVal, comp<_TKey>, _Transaction, 
		BPInnerNodeSimpleCompressorV2<_TKey> ,
		BPStringLeafNodeCompressor<_TKey,  _Transaction>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		TStringLeafNode<_TKey, _Transaction>,
		BPStringTreeNodeMapv2<_TKey, _Transaction>	>
	{
	public:

		typedef TBPMapV2<_TKey, sStringVal, comp<_TKey>, _Transaction, 
			BPInnerNodeSimpleCompressorV2<_TKey> ,
			BPStringLeafNodeCompressor<_TKey,  _Transaction>, 
			BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
			TStringLeafNode<_TKey, _Transaction>,
			BPStringTreeNodeMapv2<_TKey, _Transaction>	> TBase;

		TBPStringTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, uint32 nNodesPageSize,  bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodesPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
		{

		}

		~TBPStringTree()
		{
			DeleteNodes();
		}

		virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, nAdd, m_bMulti, bIsLeaf, m_bCheckCRC32, m_nNodesPageSize,  m_InnerCompParams.get(),
				m_LeafCompParams.get());
			pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
			return pNode;
		}

		void convert(const CommonLib::CString& sString, sStringVal& sValue)
		{
			if(m_LeafCompParams->GetStringCoding() == embDB::scASCII)
			{
				sValue.m_nLen = sString.length() + 1;
				sValue.m_pBuf = (byte*)m_PageAlloc.alloc(sValue.m_nLen);
				strcpy((char*)sValue.m_pBuf, sString.cstr());
				sValue.m_pBuf[sValue.m_nLen] = 0;
			}
			else if(m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
			{
				sValue.m_nLen  = sString.calcUTF8Length() + 1;
				sValue.m_pBuf = (byte*)m_PageAlloc.alloc(sValue.m_nLen);
				sString.exportToUTF8((char*)sValue.m_pBuf, sValue.m_nLen);
			}
		}
		void convert(const sStringVal& sStrVal, CommonLib::CString& sString) 
		{

			if(sStrVal.m_nLen < m_LeafCompParams->GetMaxPageStringSize())
			{
				if(m_LeafCompParams->GetStringCoding() == embDB::scASCII)
				{
					sString.loadFromASCII((const char*)sStrVal.m_pBuf);
				}
				else if(m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
				{
					sString.loadFromUTF8((const char*)sStrVal.m_pBuf);
				}
			}
			else
			{
				embDB::ReadStreamPagePtr pReadStream = m_LeafCompParams->GetReadStream(m_pTransaction, sStrVal.m_nPage, sStrVal.m_nPos);
				m_CacheBlob.resize(sStrVal.m_nLen);
				pReadStream->read(m_CacheBlob.buffer(), sStrVal.m_nLen);
				if(m_LeafCompParams->GetStringCoding() == embDB::scASCII)
					sString.loadFromASCII((const char*)m_CacheBlob.buffer());
				else
					sString.loadFromUTF8((const char*)m_CacheBlob.buffer());

			}
			
		}

		bool insert(int64 nValue, const CommonLib::CString& sString, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			embDB::sStringVal sValue;
			convert(sString, sValue);
			return TBase::insert(nValue, sValue, pFromIterator, pRetItertor);

		}
		bool update(const TKey& key, const CommonLib::CString& sString)
		{
			embDB::sStringVal sValue;
			convert(sString, sValue);
			return TBase::update(key, sValue);
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::CString& sString, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
		{
			embDB::sStringVal sValue;
			convert(sString, sValue);
			return TBase::insertLast(keyFunctor, sValue, pKey, pFromIterator, pRetIterator);
		}
	private:
		CPageAlloc m_PageAlloc;
		CommonLib::CBlob m_CacheBlob;
	};

}

#endif