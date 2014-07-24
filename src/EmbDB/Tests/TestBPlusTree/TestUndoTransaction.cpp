#include "stdafx.h"
#include "TestBPTree.h"
#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBMuitiSet.h"
#include "../../EmbDB/storage.h"
#include "../../EmbDB/BPTreeNode.h"
#include "../../EmbDB/BaseBPMap.h"
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/MemoryStream.h"
#include "../../EmbDB/Key.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/BTVector.h"
#include "../../EmbDB/OIDField.h"
#include "../../EmbDB/BaseBPTreeRO.h"

#include "../../EmbDB/BPInnerNodeSimpleCompressor.h"
#include "../../EmbDB/BPLeafNodeSimpleCompressor.h"
/*
typedef embDB::BPInnerNodeSimpleCompressor<int64, int64, embDB::comp<int64> > TInnerCompressor;
typedef embDB::BPLeafNodeSimpleCompressor<int64, int64, embDB::comp<int64> > TLeafCompressor;

typedef embDB::TBPlusTreeMap<int64, int64, int64, embDB::comp<int64>, 
	embDB::IDBTransactions, TInnerCompressor, TLeafCompressor > TBTreePlus;
typedef embDB::TBaseBPlusTreeRO<int64, int64, int64, embDB::comp<int64>, embDB::IDBTransactions, TInnerCompressor, TLeafCompressor  > TBTreePlusRO;

void testUndoTransactionBPTree ()
{
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();
	{
		embDB::CStorage storage( alloc);
		storage.open("d:\\dbplus.data", false, false,  true, false, 8192);
		embDB::CTransactions tran(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo.data", &storage, 1);
		//embDB::CFilePage* pPage = storage.getNewPage();
		TBTreePlus tree(-1, &tran, alloc, 50);
		tran.begin();

		int nCount = 1000000;
		//for (__int64 i = 0; i < nCount; ++i)
		for (__int64 i = nCount; i >=0; --i)
		{			
			tree.insert(i, i);
		}
		tree.commit();
		tran.commit();


		embDB::CTransactions tran1(alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\tranUndo1.data", &storage, 1);
		tran1.begin();
		TBTreePlus tree1(tree.getPageBTreeInfo(), &tran1, alloc, 50);
		for (__int64 i = 2* nCount; i >= nCount + 1; --i)
		{			
			tree1.insert(i, i);
		}
		tree1.insert(1000000, 1000000);
		tree1.commit();
		tran1.commit();



//		tran.setType(embDB::eTT_SELECT);

		embDB::CTransactions ReadTran(alloc, embDB::rtUndo, embDB::eTT_SELECT, "d:\\tranUndo2.data", &storage, 1);
		TBTreePlusRO treeRO(tree1.getPageBTreeInfo(), &ReadTran, alloc, 50);

		int64 *pPtr = NULL;
		for (__int64 i = 0; i < 2*nCount; ++i)
		{
			pPtr = treeRO.search(i);
			if(!pPtr || *pPtr != i)
			{
				std::cout << "Not found " << i << std::endl;
			}
			pPtr = NULL;
		}

		TBTreePlusRO::iterator it = treeRO.begin();
		int64 nVal = -1;
		while(!it.isNull())
		{
			if(nVal == -1)
				nVal = it.value();
			else
			{
				if((it.value() - nVal) > 1)
				{
					std::cout << "Order error "  << std::endl;
				}
				nVal = it.value();

			}
			it.next();
		}
	}
};*/