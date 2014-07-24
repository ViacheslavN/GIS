#include "stdafx.h"
#include "SimpleTransactions.h"
#include "DBConfig.h"
namespace embDB
{
	SimpleTransactions::SimpleTransactions(CommonLib::alloc_t *pAlloc, IDBStorage* pDBStorage) :
		m_pAlloc(pAlloc),
		//m_Storage(pAlloc), 
		m_bCommit(false),
		//m_PageChache(pAlloc, &m_Storage),
		m_pDBStorage(pDBStorage),
		m_nTranType(eTT_UNDEFINED),
		m_bError(false)
	{

	}
	SimpleTransactions::~SimpleTransactions()
	{
	
	}

	void SimpleTransactions::setType(eTransactionsType type)
	{
		m_nTranType = type;
	}

	bool SimpleTransactions::begin()
	{
		//return m_Storage.open("d:\\db1_tran.tpd", m_pDBStorage->getPageSize());
		return true;
	}
	bool SimpleTransactions::commit()
	{
		//m_PageChache.saveChange(m_pDBStorage);
		//m_Storage.close();
		//m_pDBStorage->commit();
		m_pDBStorage->saveChache();
		m_pDBStorage->commit();
		return true;
	}
	bool SimpleTransactions::rollback()
	{
		//m_Storage.close();
		return true;
	}
	//IStorage
	CFilePage* SimpleTransactions::getFilePage(int64 nAddr, bool bRead)
	{
		//if(m_nTranType == eTT_SELECT)
	//	{
			CFilePage *pFilePage = m_pDBStorage->getFilePage(nAddr, bRead);
			if(!pFilePage)
				return NULL;
			TPages::iterator it = m_pages.find(pFilePage->getAddr());
			if(it.isNull())
			{
				m_pages.insert(pFilePage->getAddr(), (int)eFP_CHANGE);
			}
			/*else
			{
				int32 nFlag = 
			}*/

			return pFilePage;
	/*	}
		CFilePage* pPage = m_PageChache.GetPage(nAddr);
		if(!pPage)
		{
			CFilePage* pStoragePage =  m_pDBStorage->getFilePage(nAddr);
			pPage = new CFilePage(m_pAlloc, pStoragePage->getRowData(), pStoragePage->getPageSize(), nAddr);
			m_PageChache.AddPage(nAddr, pPage);
		}
		
		return pPage;*/
	}
	void SimpleTransactions::dropFilePage(CFilePage* pPage)
	{
		//m_pStorage->dropFilePage(pPage);
		 
	}
	void SimpleTransactions::dropFilePage(int64 nAddr)
	{
		//m_pStorage->dropFilePage(nAddr);
	}
	CFilePage* SimpleTransactions::getNewPage()
	{
		//assert(m_nTranType != eTT_SELECT);
		/*if(!(m_nFakeAddr%m_Storage.getPageSize()))//что бы не пересеклись адреса
		{
			m_nFakeAddr++;
		}*/
		/*int64 nAddr = m_pDBStorage->getNewPageAddr();
		CFilePage *pFilePage = new CFilePage(m_pAlloc, m_Storage.getPageSize(), nAddr);
		pFilePage->setFlag(eFP_NEW, true);
		m_PageChache.AddPage(nAddr, pFilePage);*/
	
		//return pFilePage;
		CFilePage *pFilePage = m_pDBStorage->getNewPage();
		if(!pFilePage)
			return NULL;
		m_pages.insert(pFilePage->getAddr(), (int)eFP_NEW);
		return pFilePage;
	 
	}
	size_t SimpleTransactions::getPageSize() const
	{
		return m_pDBStorage->getPageSize();
	}
	void SimpleTransactions::saveFilePage(CFilePage* pPage)
	{
		/*assert(m_nTranType != eTT_SELECT);
		pPage->setFlag(eFP_CHANGE, true);
		m_PageChache.savePage(pPage); */
		m_pDBStorage->saveFilePage(pPage);
	}

	bool  SimpleTransactions::isError() const
	{
		return m_bError;
	}
	size_t  SimpleTransactions::getErrorMessageSize() const
	{
		return 0;
	}
	size_t  SimpleTransactions::getErroMessage(wchar_t * pBuf, size_t nSize) const
	{
		return 0;
	}
	void SimpleTransactions::error(const CommonLib::str_t& sError, uint32 nErrorID)
	{
		m_bError = true;
	}
	uint32 SimpleTransactions::getLogLevel() const
	{
		return 0;
	}
	void SimpleTransactions::log(uint32 nLevel, const CommonLib::str_t& sMessage)
	{

	}
	void  SimpleTransactions::addDBBTree(IDBBtree *pTree)
	{

	}
}