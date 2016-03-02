#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_H_

#include "BaseBPMapv2.h"
#include "FixedStringBPNode.h"
#include "PageAlloc.h"

#include "BaseInnerNodeDIffCompress.h"
#include "BaseLeafNodeCompDiff.h"
#include "BaseInnerNodeDIffCompress2.h"
#include "BaseValueDiffCompressor.h"
#include "SignedNumLenDiffCompress.h"

namespace embDB
{
 

typedef TBaseValueDiffCompress<int64, SignedDiffNumLenCompressor64i> TInnerLinkCompress;

	
template<class _TKey, class _Transaction>
class TBPFixedString : public TBPMapV2<_TKey, sFixedStringVal, comp<_TKey>, _Transaction, 
		 embDB::TBPBaseInnerNodeDiffCompressor2<_TKey, embDB::OIDCompressor, TInnerLinkCompress> ,
		 TBPFixedStringLeafCompressor<_TKey, _Transaction> /*BPFixedStringLeafNodeCompressor<_TKey, _Transaction>*/, 
		 BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		 TFixedStringLeafNode<_TKey, _Transaction>,
		 BPFixedStringTreeNodeMapv2<_TKey, _Transaction>	>
{
public:

	typedef TBPMapV2<_TKey, sFixedStringVal, comp<_TKey>, _Transaction, 
		embDB::TBPBaseInnerNodeDiffCompressor2<_TKey, embDB::OIDCompressor, TInnerLinkCompress>  ,
		TBPFixedStringLeafCompressor<_TKey, _Transaction> /*BPFixedStringLeafNodeCompressor<_TKey, _Transaction>*/, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		TFixedStringLeafNode<_TKey, _Transaction>,
		BPFixedStringTreeNodeMapv2<_TKey, _Transaction>	> TBase;

	typedef typename TBase::TKey TKey; 
	typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
	typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
	typedef typename TBase::TBTreeNode TBTreeNode;
	typedef typename TBase::iterator iterator; 

	TBPFixedString(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, uint32 nPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
	  TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nPageSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
	  {

	  }

	  ~TBPFixedString()
	  {
		  this->DeleteNodes();
	  }

	  virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
	  {
		  TBTreeNode *pNode = new TBTreeNode(-1, this->m_pAlloc, nAdd, this->m_bMulti, bIsLeaf, this->m_bCheckCRC32, this->m_nNodesPageSize, this->m_InnerCompParams.get(),
			  this->m_LeafCompParams.get());
		  pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
		return pNode;
	  }

	  void convert(const CommonLib::CString& sString, sFixedStringVal& sValue)
	  {
		  if(this->m_LeafCompParams->GetStringCoding() == embDB::scASCII)
		  {
			  sValue.m_nLen = sString.length() + 1;
			  sValue.m_pBuf = (byte*)m_PageAlloc.alloc(sValue.m_nLen);
			  strcpy((char*)sValue.m_pBuf, sString.cstr());
			  sValue.m_pBuf[sValue.m_nLen] = 0;
		  }
		  else if(this->m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
		  {
			  sValue.m_nLen  = sString.calcUTF8Length() + 1;
			  sValue.m_pBuf = (byte*)m_PageAlloc.alloc(sValue.m_nLen);
			  sString.exportToUTF8((char*)sValue.m_pBuf, sValue.m_nLen);
		  }
	  }
	  void convert(const sFixedStringVal& sStrVal, CommonLib::CString& sString) 
	  {
		  CommonLib::CString sVal(this->m_pAlloc);
		  if(this->m_LeafCompParams->GetStringCoding() == embDB::scASCII)
		  {
			  sString.loadFromASCII((const char*)sStrVal.m_pBuf);
		  }
		   else if(this->m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
		  {
			  sString.loadFromUTF8((const char*)sStrVal.m_pBuf);
		  }
	  }
		  
	  bool insert(int64 nValue, const CommonLib::CString& sString, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
	  {
		  embDB::sFixedStringVal sValue;
		  convert(sString, sValue);
		  return TBase::insert(nValue, sValue, pFromIterator, pRetItertor);

	  }
	  bool update(const TKey& key, const CommonLib::CString& sString)
	  {
		  embDB::sFixedStringVal sValue;
		  convert(sString, sValue);
		  return TBase::update(key, sValue);
	  }
	  	template<class TKeyFunctor>
	  bool insertLast(TKeyFunctor& keyFunctor, const CommonLib::CString& sString, TKey* pKey = NULL,  iterator* pFromIterator = NULL,  iterator* pRetIterator = NULL)
	  {
		  embDB::sFixedStringVal sValue;
		  convert(sString, sValue);
		  return TBase::insertLast(keyFunctor, sValue, pKey, pFromIterator, pRetIterator);
	  }
	private:
		CPageAlloc m_PageAlloc;
};

}

#endif