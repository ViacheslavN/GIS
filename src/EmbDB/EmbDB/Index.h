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
	CIndexBase( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) :
	  m_pDBTransactions(pTransactions),
		  m_tree(-1, pTransactions, pAlloc, 100, 8192), //Temorary
		  m_nBTreeRootPage(-1)
	  {

	  }

	  typedef _TBTree TBTree;
	  virtual bool save()
	  {
		  return m_tree.saveBTreeInfo();
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


	  virtual bool init(int64 nBTreeRootPage)
	  {

		  m_nBTreeRootPage = nBTreeRootPage;
		  m_tree.setRootPage(m_nBTreeRootPage);
		  return m_tree.init();
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

	CIndexHandlerBase(CommonLib::alloc_t* pAlloc,  indexTypes type) : m_pAlloc(pAlloc), m_IndexType(type)
	{
	}
	~CIndexHandlerBase(){}

	template<class TField>
	bool save(int64 nAddr, IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,  uint16 nObjectPageType, uint16 nSubObjectPageType  )
	{
		//m_nFieldInfoPage = nAddr;
		FilePagePtr pPage(pTran->getFilePage(nAddr, MIN_PAGE_SIZE));
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, nObjectPageType, nSubObjectPageType);
		int64 m_nBTreeRootPage = -1;
		FilePagePtr pRootPage(pTran->getNewPage(8192));
		if(!pRootPage.get())
			return false;
		m_nBTreeRootPage = pRootPage->getAddr();
		stream.write(m_nBTreeRootPage);
		header.writeCRC32(stream);
		pPage->setFlag(eFP_CHANGE, true);
		pTran->saveFilePage(pPage);

		TField field(pTran, pAlloc);
		field.init(m_nBTreeRootPage);
		return field.save();
	}
	virtual bool load(int64 nAddr, IDBStorage *pStorage)
	{
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
		return m_pField;
	}

protected:
	indexTypes m_IndexType;
	CommonLib::alloc_t* m_pAlloc;
	IFieldPtr m_pField;


};


}





#endif
