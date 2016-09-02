#include "stdafx.h"
#include "TranStorage.h"
#include "embDBInternal.h"
namespace embDB
{




	CTranStorage::CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter, bool bCheckCRC ) : m_pAlloc(pAlloc), m_nLastAddr(-1),
		m_pCounter(pCounter), m_pPageCrypto(NULL), m_nPageSize(MIN_PAGE_SIZE), m_bCheckCRC(bCheckCRC)
	{

	}
	CTranStorage::~CTranStorage()
	{

	}
	bool CTranStorage::open(const CommonLib::CString& sTranName,/* uint32 nPageSize,*/ bool bNew)
	{
		CommonLib::enOpenFileMode nOpenMode = bNew ? CommonLib::ofmCreateAlways : CommonLib::ofmOpenExisting;
		CommonLib::enAccesRights nReadWrite = CommonLib::aeReadWrite;
		//m_nPageSize = nPageSize;
		bool bRet =  m_pFile.openFile(sTranName.cwstr(), nOpenMode, nReadWrite, CommonLib::smNoMode);
		m_sTranName = sTranName;
		if(bRet)
		{
			m_nLastAddr = m_pFile.getFileSize();
		}

		return bRet;
	}
	bool CTranStorage::close(bool bDelete)
	{
		bool bRet =  m_pFile.closeFile();
		if(!bRet)
			return false;
		if(bDelete)
			return CommonLib::FileSystem::deleteFile(m_sTranName.cwstr());
		return true;
	}
	int64 CTranStorage::saveFilePage(CFilePage* pPage, int64 nAddr )
	{
		if(nAddr == -1)
		{
			nAddr = m_nLastAddr;
			m_nLastAddr += 1;
		}
		bool bRet = m_pFile.setFilePos64(nAddr * pPage->getPageSize(), CommonLib::soFromBegin);
		assert(bRet);
		uint32 nCnt = 0;
		if(m_pPageCrypto && pPage->isNeedEncrypt())
		{
			m_pPageCrypto->encrypt(pPage, m_pBufPageCrypto->getRowData(),  (uint32)pPage->getPageSize());
			nCnt = m_pFile.writeFile((void*)m_pBufPageCrypto->getRowData(),  (uint32)pPage->getPageSize());
		}
		else
			nCnt = m_pFile.writeFile((void*)pPage->getRowData(),  pPage->getPageSize() );
		assert(nCnt != 0);
		m_pCounter->WriteTranPage();
		return nAddr;
	}
	FilePagePtr CTranStorage::getFilePage(int64 nAddr, uint32 nSize, bool bRead, bool bDecrypt)
	{
		if(nSize%m_nPageSize != 0)
			return FilePagePtr();
		if(nSize == 0)
			nSize = m_nPageSize;

		uint32 nCnt = nSize/m_nPageSize;

		FilePagePtr pPage(new CFilePage(m_pAlloc, nSize, nAddr));
		if(bRead)
		{
			bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
			assert(bRet);
			uint32 nWCnt = m_pFile.readFile((void*)pPage->getRowData(),  (uint32)nSize );
			assert(nWCnt != 0);

			if(m_pPageCrypto && bDecrypt)
			{
				m_pPageCrypto->decrypt(pPage.get());
			}
		}
	
		if(m_nLastAddr <= nAddr)
			m_nLastAddr =  nAddr + nCnt;
			m_pCounter->ReadTranPage();
		return pPage;
	}
	FilePagePtr CTranStorage::getNewPage(uint32 nSize)
	{
		if(nSize%m_nPageSize != 0)
			return FilePagePtr();
		if(nSize == 0)
			nSize = m_nPageSize;

		uint32 nCnt = nSize/m_nPageSize;

		FilePagePtr pPage(new CFilePage(m_pAlloc, nSize, m_nLastAddr));
		m_nLastAddr += nCnt;
		return pPage;
	}
	int64 CTranStorage::getNewPageAddr(uint32 nSize)
	{
		if(nSize%m_nPageSize != 0)
			return -1;
		if(nSize == 0)
			nSize = m_nPageSize;

		uint32 nCnt = nSize/m_nPageSize;

		int64 nAddr = m_nLastAddr;
		m_nLastAddr += nCnt;
		return nAddr;
	}
	bool CTranStorage::Flush()
	{
		return m_pFile.Flush();
	}
}