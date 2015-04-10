#ifndef _EMBEDDED_DATABASE_B_INDEX_H_
#define _EMBEDDED_DATABASE_B_INDEX_H_
#include "BaseBPMapv2.h"
#include "IDBField.h"
#include "DBFieldInfo.h"
namespace embDB

{

template<class TBTree>
class TIndexIterator: public IIndexIterator
{
public:
	typedef typename TBTree::iterator  iterator;

	TIndexIterator(iterator& it) : m_ParentIt(it){}
	virtual ~TIndexIterator(){}

	virtual bool next()
	{
		return m_ParentIt.next();
	}
	virtual bool isNull()
	{
		return m_ParentIt.isNull();
	}
	virtual bool isValid()
	{
		return !m_ParentIt.isNull();
	}
	virtual bool getKey(IFieldVariant* pVal)
	{
		return pVal->setVal(m_ParentIt.key());
	}
	virtual uint64 getRowID()
	{
		return m_ParentIt.value();
	}
	virtual int64 addr() const
	{
		return m_ParentIt.addr();
	}
	virtual int32 pos() const
	{
		return m_ParentIt.pos();
	}
	virtual bool copy(IIndexIterator *pIter)
	{
		return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
	}
	void set(iterator it)
	{
		m_ParentIt = it;
	}
	public:
	iterator m_ParentIt;

};


template<class _TBTree>
class CIndexBase 
{
public:
	CIndexBase( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
	  m_pDBTransactions(pTransactions),
		  m_tree(-1, pTransactions, pAlloc, 100), 
		  m_nBTreeRootPage(-1)
	  {

	  }

	  typedef _TBTree TBTree;
	  virtual bool save()
	  {
		  return m_tree.saveBTreeInfo();
	  }
	  virtual bool load(int64 nAddr, eTransactionsType type)
	  {

		  int64 m_nFieldInfoPage = nAddr;
		  FilePagePtr pPage = m_pDBTransactions->getFilePage(nAddr);
		  if(!pPage.get())
			  return false;
		  CommonLib::FxMemoryReadStream stream;
		  stream.attach(pPage->getRowData(), pPage->getPageSize());
		  sFilePageHeader header(stream);
		  if(!header.isValid())
		  {
			  CommonLib::str_t sMsg;
			  sMsg.format(_T("IndexField: Page %I64d Error CRC for node page"), pPage->getAddr());
			  m_pDBTransactions->error(sMsg); //TO DO log error
			  return false;
		  }
		  if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != INDEX_INFO_PAGE)
		  {
			  CommonLib::str_t sMsg;
			  sMsg.format(_T("IndexField: Page %I64d Not field info page"), pPage->getAddr());
			  m_pDBTransactions->error(sMsg); //TO DO log error
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
	IDBTransactions* m_pDBTransactions;
	TBTree m_tree;
	int64 m_nBTreeRootPage;
};


class IDBIndexHandler : IField
{
public:
	IDBIndexHandler(){}
	~IDBIndexHandler(){}
	virtual sFieldInfo* getFieldInfoType() = 0;
	virtual void setFieldInfoType(sFieldInfo& fi) = 0;
	virtual bool save(int64 nAddr, IDBTransactions *pTran) = 0;
	virtual bool load(int64 nAddr, IDBStorage *pStorage) = 0;
	virtual IndexFiled* getIndex(IDBTransactions* pTransactions, IDBStorage *pStorage) = 0;
	virtual bool release(IndexFiled* pField) = 0;

	virtual bool lock() =0;
	virtual bool unlock() =0;
	virtual bool isCanBeRemoving() = 0;

};




class CIndexHandlerBase : public IDBIndexHandler
{
public:

	CIndexHandlerBase(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc)
	{}
	~CIndexHandlerBase(){}

	template<class TField>
	bool save(int64 nAddr, IDBTransactions *pTran, CommonLib::alloc_t *pAlloc,  uint16 nObjectPageType, uint16 nSubObjectPageType  )
	{
		//m_nFieldInfoPage = nAddr;
		FilePagePtr pPage(pTran->getFilePage(nAddr));
		if(!pPage.get())
			return false;
		CommonLib::FxMemoryWriteStream stream;
		stream.attach(pPage->getRowData(), pPage->getPageSize());
		sFilePageHeader header(stream, nObjectPageType, nSubObjectPageType);
		int64 m_nBTreeRootPage = -1;
		FilePagePtr pRootPage(pTran->getNewPage());
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

	eDataTypes getType() const
	{
		return (eDataTypes)m_fi.m_nFieldDataType;
	}
	const CommonLib::str_t& getName() const
	{
		return m_fi.m_sFieldName;
	}

	virtual sFieldInfo* getFieldInfoType()
	{
		return &m_fi;
	}
	virtual void setFieldInfoType(sFieldInfo& fi)
	{
		m_fi = fi;
	}
	bool isCanBeRemoving()
	{
		return true;
	}
protected:
	sFieldInfo m_fi;
	CommonLib::alloc_t* m_pAlloc;


};


}





#endif