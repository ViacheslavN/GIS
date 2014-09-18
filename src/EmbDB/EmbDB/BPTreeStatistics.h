#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_STATISTICS_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_STATISTICS_H_

#include "IDBTransactions.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "BTVector.h"
namespace embDB
{
	template <typename _TLink, class _Transaction, typename _TKey>
	class BPTreeStatistics
	{
	public:
		typedef _TLink TLink;
		typedef _TKey Tkey;
		BPTreeStatistics(bool bCheckCRC32) : m_nRootPage(-1), m_nInnerNodeCounts(0), m_nKeyCounts(0), m_nLeafNodeCounts(0),
			m_bCheckCRC32(bCheckCRC32)
		{

		}
		~BPTreeStatistics()
		{

		}

		void setPage(TLink nPage)
		{
			m_nRootPage = nPage;
		}
		bool Load(_Transaction* pTransaction)
		{
			FilePagePtr pFilePage =  pTransaction->getFilePage(m_nRootPage);
			if(!pFilePage.get())
				return false; 
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header(stream, m_bCheckCRC32 && !pFilePage->isCheck());
 
			if(!pFilePage->isCheck() && !header.isValid())
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Page %I64d Error CRC for static btree page"), pFilePage->getAddr());
				pTransaction->error(sMsg);
				return false;
			}
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_STATIC_PAGE)
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Page %I64d is not BTreeStaticPage"), (int64)m_nRootPage);
				pTransaction->error(sMsg);
				return false;
			}
			pFilePage->setCheck(true);
			stream.read(m_nInnerNodeCounts);
			stream.read(m_nLeafNodeCounts);
			stream.read(m_nKeyCounts);

			assert(m_nLeafNodeCounts >= 0);
			assert(m_nInnerNodeCounts >= 0);

			return true;
		}
		bool Save(_Transaction* pTransaction)
		{
			FilePagePtr pFilePage =  pTransaction->getFilePage(m_nRootPage, false);
			if(!pFilePage.get())
				return false; 
			CommonLib::FxMemoryWriteStream stream;
			
			stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_STATIC_PAGE);
			stream.write(m_nInnerNodeCounts);
			stream.write(m_nLeafNodeCounts);
			stream.write(m_nKeyCounts);
			header.writeCRC32(stream);
			pTransaction->saveFilePage(pFilePage);
			return true;
		}


		void AddKey(int nKey)
		{
			m_nKeyCounts += nKey;
		}
		void AddNode(int nNode, bool bLeaf)
		{
			if(bLeaf)
				m_nLeafNodeCounts += nNode;
			else
				m_nInnerNodeCounts += nNode;

			assert(m_nLeafNodeCounts >= 0);
			assert(m_nInnerNodeCounts >= 0);
		}
		void clear()
		{
			m_nKeyCounts = 0;
			m_nLeafNodeCounts = 1;
			m_nInnerNodeCounts = 0;
		}

	public:
		TLink m_nRootPage;
		TLink m_nLeafNodeCounts;
		TLink m_nInnerNodeCounts;
		TLink m_nKeyCounts;
		bool m_bCheckCRC32;


	};
}

#endif