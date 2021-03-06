#include "stdafx.h"
#include "TranStorage.h"
#include "../../embDBInternal.h"
namespace embDB
{




	CTranStorage::CTranStorage(CommonLib::alloc_t *pAlloc, CTranPerfCounter *pCounter, bool bCheckCRC,
		CPageCipher* pPageCrypto ) : m_pAlloc(pAlloc), m_nLastAddr(-1),
		m_pCounter(pCounter), m_pPageChiper(pPageCrypto), m_nPageSize(MIN_PAGE_SIZE), m_bCheckCRC(bCheckCRC)
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

	bool CTranStorage::saveFilePage(CFilePage* pPage, uint32 nDataSize)
	{
		uint32 nSize = pPage->getPageSize();
		if (nSize%m_nPageSize != 0)
			return false;
		uint32 nCnt = nSize / m_nPageSize;

		bool bRet = m_pFile.setFilePos64(pPage->getAddr() * m_nPageSize, CommonLib::soFromBegin);
		assert(bRet);
		uint32 nWCnt = 0;
		if (m_pPageChiper && pPage->isNeedEncrypt())
		{
			m_BufForChiper.reserve(pPage->getPageSize());
			m_pPageChiper->encrypt(pPage, m_BufForChiper.buffer(), pPage->getPageSize());
			nWCnt = m_pFile.writeFile((void*)m_BufForChiper.buffer(), pPage->getPageSize());
		}
		else
			nWCnt = m_pFile.writeFile((void*)pPage->getRowData(), pPage->getPageSize());
		assert(nWCnt != 0);
		m_pCounter->WriteTranPage();
		return true;

	}
	bool CTranStorage::saveFilePage(FilePagePtr pPage, uint32 nDataSize)
	{
		return saveFilePage(pPage.get(), nDataSize);
	}

	int64 CTranStorage::saveFilePageWithRetAddr(CFilePage* pPage, int64 nAddr )
	{
		uint32 nSize = pPage->getPageSize();
		if(nSize%m_nPageSize != 0)
			return -1;
		if(nSize == 0)
			nSize = m_nPageSize;

		uint32 nCnt = nSize/m_nPageSize;


		if(nAddr == -1)
		{
			nAddr = m_nLastAddr;
			m_nLastAddr += nCnt;
		}
		bool bRet = m_pFile.setFilePos64(nAddr * m_nPageSize, CommonLib::soFromBegin);
		assert(bRet);
		uint32 nWCnt = 0;
		if(m_pPageChiper && pPage->isNeedEncrypt())
		{
			m_BufForChiper.reserve(pPage->getPageSize());
			m_pPageChiper->encrypt(pPage, m_BufForChiper.buffer(), pPage->getPageSize());
			nWCnt = m_pFile.writeFile((void*) m_BufForChiper.buffer(), pPage->getPageSize());
		}
		else
			nWCnt = m_pFile.writeFile((void*)pPage->getRowData(),  pPage->getPageSize() );
		assert(nWCnt != 0);
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

			if(m_pPageChiper && bDecrypt)
			{
				m_pPageChiper->decrypt(pPage.get());
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