#ifndef _EMBEDDED_DATABASE_B_INDEX_H_
#define _EMBEDDED_DATABASE_B_INDEX_H_
#include "BaseBPMapv2.h"
#include "IDBField.h"
#include "DBFieldInfo.h"
namespace embDB

{

template<class TBTree>
class IndexIterator: public IIndexIterator
{
public:
	typedef typename TBTree::iterator  iterator;

	IndexIterator(iterator& it) : m_ParentIt(it){}
	virtual ~IndexIterator(){}

	virtual bool isValid()
	{
		return !m_ParentIt.isNull();
	}
	virtual bool next() 
	{
		return m_ParentIt.next();
	}
	virtual bool back() 
	{
		return m_ParentIt.back();
	}
	virtual bool isNull()
	{
		return m_ParentIt.isNull();
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

	virtual bool copy(IndexIterator *pIter)
	{
		return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
	}

	void set(iterator it)
	{
		m_ParentIt = it;
	}

private:
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
	  virtual bool load(int64 nAddr)
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

/*


template<class _TIndexType, class _TBTree, int FieldDataType>
class CIndexField : public CIndexBase<_TBTree>, public  IndexFiled
{
public:


	typedef IndexIterator<_TBTree> TFieldIterator;

	CIndexField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
	CIndexBase(pTransactions, pAlloc)
	{}
	~CIndexField(){}
	typedef OIDFieldBase<_TBTree> TBase;
	typedef typename TBase::TBTree TBTree;
	typedef _TIndexType TIndexType;

	virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID, IIndexIterator* pIter = NULL)
	{
		TIndexType val;
		pIndexKey->getVal(val);
		if(pIter)
		else
		return m_tree.insert(val, nOID);

	}
	virtual uint64 insert (IFieldVariant* pFieldVal)
	{
		FType val;
		pFieldVal->getVal(val);
		uint64 nOID;
		if(!m_tree.insertLast(TOIDIncFunctor(), val, &nOID))
			return  0;
		return nOID;
	}
	virtual bool update (uint64 nOID, IFieldVariant* pFieldVal)
	{
		FType val;
		pFieldVal->getVal(val);
		return  m_tree.update(nOID, val);
	}
	virtual bool remove(uint64 nOID)
	{
		return m_tree.remove(nOID);
	}
	virtual bool find(uint64 nOID, IFieldVariant* pFieldVal)
	{
		TBTree::iterator it = m_tree.find(nOID);
		if(it.isNull())
			return false;
		return pFieldVal->setVal(it.value());
	}
	FieldIteratorPtr find(uint64 nOID)
	{
		TBTree::iterator it = m_tree.find(nOID);
		TFieldIterator *pFiledIterator = new TFieldIterator(it);
		return FieldIteratorPtr(pFiledIterator);
	}
	virtual bool commit()
	{
		return m_tree.commit();
	}

};*/






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




}





#endif
