#ifndef _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_
#define _EMBEDDED_DATABASE_B_PLUS_V2_TREE_STRING_H_

#include "BaseBPMapv2.h"
#include "StringBPNode.h"
#include "PageAlloc.h"
namespace embDB
{


template<class _TKey, class _Transaction>
class TBPString : public TBPMapV2<_TKey, sStringVal, comp<_TKey>, _Transaction, 
		 BPInnerNodeSimpleCompressorV2<_TKey> ,
		 BPStringLeafNodeSimpleCompressor<_TKey>, 
		 BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		 TStringLeafNode<_TKey, _Transaction>,
		 BPStringTreeNodeMapv2<_TKey, _Transaction>	>
{
public:

	typedef TBPMapV2<_TKey, sStringVal, comp<_TKey>, _Transaction, 
		BPInnerNodeSimpleCompressorV2<_TKey> ,
		BPStringLeafNodeSimpleCompressor<_TKey>, 
		BPTreeInnerNodeSetv2<_TKey, _Transaction, BPInnerNodeSimpleCompressorV2<_TKey> >, 
		TStringLeafNode<_TKey, _Transaction>,
		BPStringTreeNodeMapv2<_TKey, _Transaction>	> TBase;

	TBPString(int64 nPageBTreeInfo, embDB::IDBTransactions* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
	  TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32), m_PageAlloc(pAlloc, 1024*1024, 2)
	  {

	  }

	  ~TBPString()
	  {
		  DeleteNodes();
	  }

	  virtual TBTreeNode* CreateNode(int64 nAdd, bool bIsLeaf)
	  {
		  TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, nAdd, m_bMulti, bIsLeaf, m_bCheckCRC32,  m_InnerCompParams.get(),
			  m_LeafCompParams.get());
		  pNode->m_LeafNode.SetPageAlloc(&m_PageAlloc);
		return pNode;
	  }


	  bool insert(int64 nValue, const CommonLib::CString& sString)
	  {
		  embDB::sStringVal sValue;
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
		  return TBase::insert(nValue, sValue);

	  }
	private:
		CPageAlloc m_PageAlloc;
};

}

#endif