#include "stdafx.h"
#include "FilePage.h"

namespace embDB
{
CFilePage::CFilePage(CommonLib::alloc_t *m_pAlloc, uint32 nSize, int64 nAddr) :
	m_nSize(nSize)
	,m_nAddr(nAddr)
	,m_pAlloc(m_pAlloc)
	,m_nFlags(0)
	,m_bValid(false)
	,m_bCheck(false)
	,m_bIsNeedEncrypt(true)
	,m_bIgnoreOffset(false)
{
	m_pData =  (byte*)m_pAlloc->alloc(sizeof(byte) * m_nSize);

}
CFilePage::CFilePage(CommonLib::alloc_t *m_pAlloc, byte *pData, uint32 nSize, int64 nAddr) :
	m_nSize(nSize)
	,m_nAddr(nAddr)
	,m_pAlloc(m_pAlloc)
	,m_nFlags(0)
	,m_bValid(false)
	,m_bCheck(false)
	,m_bIsNeedEncrypt(true)
	,m_bIgnoreOffset(false)
{
	m_pData =  (byte*)m_pAlloc->alloc(sizeof(byte) * m_nSize);
	memcpy(m_pData, pData, nSize);
}

	CFilePage::CFilePage(byte *pData, uint32 nSize, int64 nAddr) :
	m_nSize(nSize)
	,m_nAddr(nAddr)
	,m_pAlloc(0)
	,m_nFlags(0)
	,m_bValid(false)
	,m_bCheck(false)
	,m_bIsNeedEncrypt(true)
	,m_bIgnoreOffset(false)
{
	m_pData =  pData;

}

CFilePage::~CFilePage()
{
	if(m_pAlloc)
		m_pAlloc->free(m_pData);
}
byte* CFilePage::getRowData() const
{
	return m_pData;
}
uint32 CFilePage::getPageSize() const
{
	return m_nSize;
}
int64 CFilePage::getAddr() const
{
	return m_nAddr;
}
bool CFilePage::IsFree()
{
	return RefCounter::isRemovable();
}
uint32 CFilePage::getFlags() const
{
	return m_nFlags;
}
void CFilePage::setFlag(uint32 nFlag, bool bSet)
{
	if(bSet)
		m_nFlags |= nFlag;
	else
		m_nFlags &= ~nFlag;
}
bool CFilePage::copyFrom(CFilePage *pPage)
{
	assert(pPage->getPageSize() == getPageSize());
	memcpy(m_pData, pPage->getRowData(), getPageSize());
	return true;
}
void CFilePage::setAddr(int64 nAddr)
{
	m_nAddr = nAddr;
}

bool CFilePage::isValid() const
{
	return m_bValid;
}
void CFilePage::setValid(bool bValid)
{
	m_bValid = bValid;
}
/*bool CFilePage::isCheck() const
{
	return m_bCheck;
}
void CFilePage::setCheck(bool bCheck)
{
	m_bCheck = bCheck;
}*/
bool CFilePage::isNeedEncrypt() const
{
	return m_bIsNeedEncrypt;
}
void CFilePage::setNeedEncrypt(bool bEncrypt)
{
	m_bIsNeedEncrypt = bEncrypt;
}
/*
bool CFilePage::isIgnoreOffset() const
{
	return m_bIgnoreOffset;
}
void CFilePage::setIgnoreOffset(bool bIgnore)
{
	m_bIgnoreOffset = bIgnore;
}
*/
}
