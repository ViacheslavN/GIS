#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_


#include  "../../BTreePlus/BPMapv3.h"
#include "../BaseFieldEncoders.h"
#include "LeafStringCompress.h"

namespace embDB
{


	template<class _TKey, class _Transaction>
	class TBPStringTree : public TBPMapV3<_TKey, sStringVal, comp<_TKey>, _Transaction,
		TInnerNodeLinkDiffComp,
		TBPStringLeafCompressor >
	{
	public:

		typedef TBPMapV3 < _TKey, sStringVal, comp<_TKey>, _Transaction,	TInnerNodeLinkDiffComp, TBPStringLeafCompressor >  TBase;


		typedef typename TBase::TBTreeNode TBTreeNode;
		typedef typename TBase::iterator	iterator;
		typedef typename TBase::TKey	TKey;

		TBPStringTree(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodesPageSize,  bool bMulti = false, bool bCheckCRC32 = true) :
		TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodesPageSize, bMulti, bCheckCRC32)/*, m_PageAlloc(pAlloc, 1024*1024, 2)*/
		{

		}

		~TBPStringTree()
		{
		//	this->DeleteNodes();
		}

		/*virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
		{
			TBTreeNode *pNode = new TBTreeNode(-1, this->m_pAlloc, nAdd, this->m_bMulti, bIsLeaf, this->m_bCheckCRC32, this->m_nNodesPageSize,  this->m_InnerCompParams.get(),
				this->m_LeafCompParams.get());
			pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
			return pNode;
		}
		*/
		void convert(const CommonLib::CString& sString, sStringVal& sValue)
		{
			sValue.m_string = sString;

			if(this->m_LeafCompParams->GetStringCoding() == embDB::scASCII)
			{
				sValue.m_nLen = sString.length() + 1;
			}
			else if(this->m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
			{
				sValue.m_nLen  = sString.calcUTF8Length() + 1;
			}
		}
		void convert(const sStringVal& sStrVal, CommonLib::CString& sString) 
		{
			sString = sStrVal.m_string;

			/*if(sStrVal.m_nLen < this->m_LeafCompParams->GetMaxPageStringSize())
			{
				if(this->m_LeafCompParams->GetStringCoding() == embDB::scASCII)
				{
					sString.loadFromASCII((const char*)sStrVal.m_pBuf);
				}
				else if(this->m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
				{
					sString.loadFromUTF8((const char*)sStrVal.m_pBuf);
				}
			}
			else
			{
				embDB::ReadStreamPagePtr pReadStream = this->m_LeafCompParams->GetReadStream(this->m_pTransaction, sStrVal.m_nPage, sStrVal.m_nPos);
				m_CacheBlob.resize(sStrVal.m_nLen);
				pReadStream->read(m_CacheBlob.buffer(), sStrVal.m_nLen);
				if(this->m_LeafCompParams->GetStringCoding() == embDB::scASCII)
					sString.loadFromASCII((const char*)m_CacheBlob.buffer());
				else
					sString.loadFromUTF8((const char*)m_CacheBlob.buffer());

			}*/
			
		}

		bool insert(int64 nValue, const CommonLib::CString& sString, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
		{
			embDB::sStringVal sValue;
			convert(sString, sValue);
			return TBase::insert(nValue, sValue/*, pFromIterator, pRetItertor*/);

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
	//	CPageAlloc m_PageAlloc;
		CommonLib::CBlob m_CacheBlob;
	};

}

#endif