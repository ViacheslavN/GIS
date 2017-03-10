#ifndef _EMBEDDED_DATABASE_B_INDEX_H_
#define _EMBEDDED_DATABASE_B_INDEX_H_
#include "BaseBPMapv2.h"
#include "embDBInternal.h"
#include "DBFieldInfo.h"
#include "IndexIteratorBase.h"
 

#include "BaseInnerNodeDIffCompress.h"
#include "BaseLeafNodeCompDiff.h"
#include "BaseInnerNodeDIffCompress.h"
#include "BaseInnerNodeDIffCompress2.h"
#include "BaseValueDiffCompressor.h"
#include "SignedNumLenDiffCompress.h"
#include "BaseLeafNodeCompDiff2.h"
#include "BaseValueCompressor.h"
#include "EmptyValueDIffCompress.h"

namespace embDB

{

template<class Iterator>
class TIndexIterator: public TIndexIteratorBase<Iterator, IIndexIterator>
{
public:
	typedef TIndexIteratorBase<Iterator, IIndexIterator> TBase;
	typedef typename TBase::iterator  iterator;

	TIndexIterator(iterator& it, IndexFiled *pIndex) : TBase(it, pIndex)
	{
		
	}
	virtual bool getKey(CommonLib::CVariant* pVal)
	{
		pVal->setVal(this->m_ParentIt.key());
		return true;
	}
	virtual int64 getRowID()
	{
		return this->m_ParentIt.value();
	}
};


template<class _TBTree>
class CIndexBase 
{
public:
	CIndexBase( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize) :
	  m_pDBTransactions(pTransactions),
		  m_tree(-1, pTransactions, pAlloc, 100, nPageSize), //Temorary
		  m_nBTreeRootPage(-1)
	  {

	  }

	  typedef _TBTree TBTree;
	  typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
	  typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

	  virtual bool save()
	  {
		 return true;// return m_tree.saveBTreeInfo();
	  }
	  virtual bool load(int64 nAddr, eTransactionDataType type)
	  {
 
		  m_tree.setRootPage(nAddr);
		  return m_tree.loadBTreeInfo(); 
	  }


	  virtual bool init(int64 nBTreeRootPage, TInnerCompressorParams *pInnerComp = NULL, TLeafCompressorParams *pLeafComp = NULL)
	  {

		  m_nBTreeRootPage = nBTreeRootPage;
		 // m_tree.setRootPage(m_nBTreeRootPage);
		  return m_tree.init(m_nBTreeRootPage, pInnerComp, pLeafComp);
	  }

	  TBTree* getBTree() {return &m_tree;}
protected:
	IDBTransaction* m_pDBTransactions;
	TBTree m_tree;
	int64 m_nBTreeRootPage;
};




class CIndexHolderBase : public IDBIndexHolder
{
public:

	CIndexHolderBase(IDBFieldHolder *pField, CommonLib::alloc_t* pAlloc,  indexTypes type, int64 nIndexPage, const SIndexProp &ip) : m_pAlloc(pAlloc), m_IndexType(type),
		m_nIndexPage(nIndexPage), m_nBTreeRootPage(-1)
	{
		m_pField = pField;
		m_nNodePageSize = ip.m_nNodePageSize;
	}
	~CIndexHolderBase(){}

	template<class TField, class TInnerCompParams, class TLeafCompParams>
	bool save(CommonLib::IWriteStream* pStream, IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,  TInnerCompParams* pInnerParams = NULL, TLeafCompParams* pLeafParams = NULL  )
	{
		FilePagePtr pRootPage(pTran->getNewPage(MIN_PAGE_SIZE));
		if(!pRootPage.get())
			return false;
		m_nBTreeRootPage = pRootPage->getAddr();
		pStream->write(m_nNodePageSize);
		pStream->write(m_nBTreeRootPage) ;

		TField field(pTran, pAlloc, m_nNodePageSize);
		field.init(m_nBTreeRootPage, pInnerParams,  pLeafParams);
		return true;
	}
	virtual bool load(CommonLib::IReadStream *pStream, IDBStorage *pStorage)
	{
		m_nNodePageSize = pStream->readInt32();
		m_nBTreeRootPage = pStream->readInt64();
		return true;
	}

	virtual bool lock()
	{
		return true;
	}
	virtual bool unlock()
	{
		return true;
	}
	bool isCanBeRemoving()
	{
		return true;
	}

	virtual indexTypes GetType() const
	{
		return  m_IndexType;
	}

	virtual IFieldPtr GetField() const 
	{
		return IFieldPtr(m_pField.get());
	}

protected:
	indexTypes m_IndexType;
	CommonLib::alloc_t* m_pAlloc;
	IDBFieldHolderPtr m_pField;
	int64 m_nIndexPage;
	int64 m_nBTreeRootPage;
	uint32 m_nNodePageSize;

	CompressType m_CompressType;
	bool m_bOnlineCalcCompSize;
	uint32 m_nCompCalcError;


};


}





#endif
