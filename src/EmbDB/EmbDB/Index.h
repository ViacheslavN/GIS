#ifndef _EMBEDDED_DATABASE_B_INDEX_H_
#define _EMBEDDED_DATABASE_B_INDEX_H_
#include "BaseBPMapv2.h"
#include "IField.h"
namespace embDB

{


class IndexIterator : public RefCounter
{
public:
	IndexIterator();
	virtual ~IndexIterator();
	virtual bool isValid() = 0;
	virtual bool next() = 0;
	virtual bool isNull() = 0;
	virtual bool getVal(IFieldVariant* pIndexKey) = 0;
	virtual uint64 getObjectID() = 0;
};

typedef IRefCntPtr<IndexIterator> TIndexIterator;

class IndexFiled  
{
public:
	IndexFiled() {}
	virtual ~IndexFiled() {}
	virtual bool insert (IFieldVariant* pIndexKey, uint64 nOID, IndexIterator* pIter = NULL) = 0;
	virtual bool update (IFieldVariant* pIndexKey, uint64 nOID, IndexIterator* pIter = NULL) = 0;
	virtual bool remove (IFieldVariant* pIndexKey, IndexIterator* pIter = NULL) = 0;
	virtual bool remove (IndexIterator* pIter ) = 0;
	virtual TIndexIterator find(IFieldVariant* pIndexKey) = 0;
	virtual TIndexIterator lower_bound(IFieldVariant* pIndexKey) = 0;
	virtual TIndexIterator upper_bound(IFieldVariant* pIndexKey) = 0;
	virtual bool commit() = 0;
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
		  return true;
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


}





#endif
