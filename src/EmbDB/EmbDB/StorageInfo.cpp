#include "stdafx.h"
#include "StorageInfo.h"

namespace embDB
{
	CStorageInfo::CStorageInfo(CStorage * pStorage) : m_pStorage(pStorage)
		,m_nBeginSize(0)
		,m_bDirty(false)
	{
	}
	CStorageInfo::~CStorageInfo()
	{

	}

	void CStorageInfo::BeginWriteTransaction(const CommonLib::str_t& sTranFileName)
	{
		m_bDirty = true;
		m_sTranName = sTranFileName;
		//m_nBeginSize = m_pStorage->getFileSize();
		//Save();
	}
	void CStorageInfo::EndWriteTransaction(const CommonLib::str_t& sTranFileName)
	{
		assert(!m_bDirty);
		assert(m_sTranName == sTranFileName);
		m_bDirty = false;
		//Save();
	}
	bool CStorageInfo::isDitry() const
	{
		return m_bDirty;
	}
	const CommonLib::str_t& CStorageInfo::getWriteTransaction() const
	{
		return m_sTranName;
	}
	bool CStorageInfo::save()
	{
		return true;
	}
}