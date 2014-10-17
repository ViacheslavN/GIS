#include "stdafx.h"
#include "TranPerfCounter.h"
#include <iostream>
namespace embDB
{
	CTranPerfCounter::CTranPerfCounter()
	{
		clear();
	}
	CTranPerfCounter::~CTranPerfCounter()
	{

	}

	void CTranPerfCounter::clear()
	{
		m_nPageReadFromDBStorage = 0;
		m_nPageWriteToDBStorage = 0;
		m_nPageWriteToDBStorageNew = 0;
		m_nRemovePageFromDBStorage = 0;
		m_nPageReadFromTranStorage = 0;
		m_nPageWriteToTranStorage = 0;
		m_nPageFromChache = 0;
		m_nAddUndoAddr = 0;
	}


	void  CTranPerfCounter::OutDebugInfo()
	{

	
		std::cout <<"Read DB Page: " << m_nPageReadFromDBStorage << " Save DB Page: " << m_nPageWriteToDBStorage
			<< " Write new DB Page: " <<m_nPageWriteToDBStorageNew	<< " Remove from DB: " << m_nRemovePageFromDBStorage << std::endl;
		std::cout << "Read Tran Page:" << m_nPageReadFromTranStorage << " Save Tran Page  " << m_nPageWriteToTranStorage << std::endl;
		std::cout <<"Read Page from Chache: " << m_nPageFromChache <<" Save undo addr: " << m_nAddUndoAddr << std::endl;
	}
}