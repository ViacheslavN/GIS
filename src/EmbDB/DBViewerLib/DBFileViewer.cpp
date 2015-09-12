#include "stdafx.h"
#include "DBFileViewer.h"
#include "../EmbDB/storage.h"

namespace DbFileViewerLib
{
	CDBFileViewer::CDBFileViewer() : m_bOpen(false), m_bInnerStorage(false),
		m_pStorage(NULL), m_FileType(eNotDef)
	{
	}
	CDBFileViewer::~CDBFileViewer()
	{
		close();
	}

	bool CDBFileViewer::close()
	{
		if(!m_bOpen)
			return false;
		m_bOpen = false;
		if(m_bInnerStorage || !m_pStorage)
			return true;

		m_pStorage->close();
		delete m_pStorage;
		m_pStorage = NULL;
		return true;
	}

	bool CDBFileViewer::isOpen() const
	{
		return m_bOpen;
	}
	bool CDBFileViewer::open(const CommonLib::CString& sDbName,const CommonLib::CString& sWorkingPath, const CommonLib::CString& sPassword)
	{
		if(!isOpen())
			return false;
		m_pStorage= new embDB::CStorage(&m_alloc);
		bool bOpen =  m_pStorage->open(sDbName, false, false, false, false, 8192);
		if(!bOpen)
		{
			delete m_pStorage;
			m_pStorage = NULL;
			return false;
		}
		return ReadHeader();
			
	}
	bool CDBFileViewer::open(embDB::CStorage* pStorage)
	{
		if(!isOpen())
			return false;
		m_pStorage = pStorage;
		return ReadHeader();
	}
	bool CDBFileViewer::ReadHeader()
	{
		embDB::FilePagePtr pPage = m_pStorage->getFilePage(0);
		if(!pPage.get())
			return false;
		return true;
	}
}