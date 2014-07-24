#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_PAGE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_PAGE_H_
#include "FilePage.h"
#include "BTreeNode.h"
#include "BPTreeNode.h"
#include "BaseBPTree.h"
#include "BPInnerNodeSimpleCompressor.h"
#include "BPLeafNodeSimpleCompressor.h"
#include "TranStorage.h"
namespace embDB
{
	
	
	class CTranPage
	{

	public:
		CTranNewPage(CTranStorage* pStorage);
		bool insert(int64 nAddr, const sFileTranPageInfo& fi);
		sFileTranPageInfo* find(int64 nAddr) const;
	private:

		typedef RBMap<int64, sFileTranPageInfo> TPages; //все страницы
		TPages m_MapPages;
	
		CTranStorage* m_pStorage;
	};
}

#endif