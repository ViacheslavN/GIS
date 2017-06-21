#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_FIXED_STRING_H_

#include  "../../../BTreePlus/BPMap.h"
#include "../../BaseFieldEncoders.h"
#include "utils/alloc/PageAlloc.h"

 
#include "FixedStringCompressor.h"
#include "FixedStringBPLeafNode.h"
namespace embDB
{
 
 	
template<class _TKey, class _Transaction>
class TBPFixedString : public TBPMap<_TKey, CommonLib::CString, comp<_TKey>, _Transaction, 
		TInnerNodeLinkDiffComp,	 TBPFixedStringLeafCompressor<_TKey, _Transaction> , 
		 BPTreeInnerNodeSet<_TKey, _Transaction, TInnerNodeLinkDiffComp >,
		 TFixedStringLeafNode<_TKey, _Transaction>	>
{
public:

	typedef TBPMap<_TKey, CommonLib::CString, comp<_TKey>, _Transaction,
		TInnerNodeLinkDiffComp,
		TBPFixedStringLeafCompressor<_TKey, _Transaction>, 	
		BPTreeInnerNodeSet<_TKey, _Transaction, TInnerNodeLinkDiffComp >,
		TFixedStringLeafNode<_TKey, _Transaction> > TBase;

	typedef typename TBase::TKey TKey; 
	typedef typename TBase::TInnerCompressorParams TInnerCompressorParams;
	typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
	typedef typename TBase::TBTreeNode TBTreeNode;
	typedef typename TBase::iterator iterator; 

	TBPFixedString(int64 nPageBTreeInfo, embDB::IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
	  TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nPageSize, bMulti, bCheckCRC32)/*, m_PageAlloc(pAlloc, 1024*1024, 2)*/
	  {

	  }

	  ~TBPFixedString()
	  {
		//  this->DeleteNodes();
	  }

	/*  virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
	  {
		  TBTreeNode *pNode = new TBTreeNode(-1, this->m_pAlloc, nAdd, this->m_bMulti, bIsLeaf, this->m_bCheckCRC32, this->m_nNodesPageSize, this->m_InnerCompParams.get(),
			  this->m_LeafCompParams.get());
		  pNode->m_LeafNode.SetPageAlloc((CommonLib::alloc_t*)&m_PageAlloc);
		return pNode;
	  }*/

	/*  void convert(const CommonLib::CString& sString, sFixedStringVal& sValue)
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
	  }*/
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
		  
/*	  bool insert(int64 nValue, const CommonLib::CString& sString, iterator* pFromIterator = NULL, iterator*pRetItertor = NULL)
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
	  }*/
	private:
		//CPageAlloc m_PageAlloc;
};

}

#endif