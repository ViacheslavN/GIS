#ifndef _EMBEDDED_DATABASE_B_INDEX_H_
#define _EMBEDDED_DATABASE_B_INDEX_H_
#include "BaseBPMapv2.h"
#include "embDBInternal.h"
#include "DBFieldInfo.h"
#include "IndexIteratorBase.h"
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
		pVal->setVal(m_ParentIt.key());
		return true;
	}
	virtual int64 getRowID()
	{
		return m_ParentIt.value();
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
	  virtual bool load(int64 nAddr, eTransactionType type)
	  {

		  int64 m_nFieldInfoPage = nAddr;
		  FilePagePtr pPage = m_pDBTransactions->getFilePage(nAddr, MIN_PAGE_SIZE); //TO DO fix
		  if(!pPage.get())
			  return false;
		  CommonLib::FxMemoryReadStream stream;
		  stream.attach(pPage->getRowData(), pPage->getPageSize());
		  sFilePageHeader header(stream);
		  if(!header.isValid())
		  {
			  m_pDBTransactions->error(_T("IndexField: Page %I64d Error CRC for node page"), pPage->getAddr()); //TO DO log error
			  return false;
		  }
		  if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != INDEX_INFO_PAGE)
		  {
			  m_pDBTransactions->error(_T("IndexField: Page %I64d Not field info page"), pPage->getAddr()); //TO DO log error
			  return false;
		  }
		  stream.read(m_nBTreeRootPage);
		  m_tree.setRootPage(m_nBTreeRootPage);
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




class CIndexHandlerBase : public IDBIndexHandler
{
public:

	CIndexHandlerBase(IDBFieldHandler *pField, CommonLib::alloc_t* pAlloc,  indexTypes type, int64 nIndexPage, uint32 nNodePageSize) : m_pAlloc(pAlloc), m_IndexType(type),
		m_nIndexPage(nIndexPage), m_nBTreeRootPage(-1), m_nNodePageSize(nNodePageSize)
	{
		m_pField = pField;
	}
	~CIndexHandlerBase(){}

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
	IDBFieldHandlerPtr m_pField;
	int64 m_nIndexPage;
	int64 m_nBTreeRootPage;
	uint32 m_nNodePageSize;


};


}





#endif
