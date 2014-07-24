// TestBTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../EmbDB/RBSet.h"
#include "../../EmbDB/RBMuitiSet.h"
#include "../../EmbDB/storage.h"
#include "../../EmbDB/BTreeNode.h"
#include "../../EmbDB/BaseBTree.h"
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/MemoryStream.h"
#include "../../EmbDB/Key.h"
#include <set>
#include "../../EmbDB/SimpleTransactions.h"
#include "../../EmbDB/BTVector.h"
#include "../../EmbDB/OIDField.h"
#include "../../EmbDB/BTreeNodeRO.h"
#include "../../EmbDB/BaseBTreeRO.h"
/*class TKey
	{
	public:
		 int64 m_nOID;
		 int64 m_nLink;
		 int64 val;
	};*/

typedef embDB::TBaseNodeElem<int64, int64> TElem;
typedef embDB::TBNodeComp<TElem> TComp;
 
typedef embDB::RBSet<TElem, TComp> TTree;
typedef embDB::TBaseBTree<TElem, TComp > TBTree;
typedef embDB::TBaseBTreeRO<TElem, TComp > TBTreeRO;

int _tmain(int argc, _TCHAR* argv[])
{

	/*std::set<int> sInt;
	sInt.insert(1);
	sInt.insert(2);
	sInt.insert(3);
	sInt.insert(4);
	sInt.insert(5);
	sInt.insert(6);
	sInt.insert(7);
	sInt.insert(8);

	for(std::set<int>::iterator sIt = sInt.begin(); sIt != sInt.end(); sIt++)
	{
		int i = 0;
		i++;
	}*/
	CommonLib::alloc_t *alloc = new CommonLib::simple_alloc_t();

	/*embDB::TFieldINT32 int32key;
	embDB::BNodeVector<TElem> vec(alloc);
	TElem vkey;
	vec.reserve(5);
	vkey.m_key = 1;
	vec.push_back(vkey);

	vkey.m_key = 10;
	vec.push_back(vkey);

	vkey.m_key = 50;
	vec.push_back(vkey);

	vkey.m_key = 100;
	vec.push_back(vkey);

	vkey.m_key = 150;
	vec.push_back(vkey);

	short nType = -1;
	vkey.m_key = 30;
	int32 nnIndex = vec.search_or_less_index(vkey, nType);

	vkey.m_key = 40;
	nnIndex = vec.search_or_less_index(vkey, nType);

	vkey.m_key = 100;
	nnIndex = vec.search_or_less_index(vkey, nType);

	vkey.m_key = 200;
	nnIndex = vec.search_or_less_index(vkey, nType);

	vkey.m_key = -1;
	nnIndex = vec.search_or_less_index(vkey, nType);*/

	
		int nCount = 1000000;
	{


	
	

		__int64 nRootPageIofoTree = -1;
		{
			embDB::CStorage storage1( alloc);
			storage1.open("d:\\db1.data", false, false,  true,false,  65536);
			embDB::SimpleTransactions tran(alloc, &storage1);
			embDB::CFilePage* pPage = storage1.getNewPage();
		TBTree tree(1000, pPage->getAddr(), &tran, alloc, 10);
		tran.begin();
	
		for (__int64 i = 0; i < nCount; ++i)
		{
			TElem key;
			key.m_key = i;
			key.m_val = i;
			key.m_nLink = -1;
			tree.insert(key);
		}
		tree.commit();
		tran.commit();
		TElem *pFindkey = NULL;
		
		tran.setType(embDB::eTT_SELECT);
		for (__int64 i = 0; i < nCount; ++i)
		{
			TElem key;
			key.m_key = i;
 
			pFindkey = tree.search(key);
			if(!pFindkey || pFindkey->m_key != i)
				std::cout << "Not found " << i << std::endl;
		}

		nRootPageIofoTree = tree.getRootPageIofoTree();
		}

		{
			embDB::CStorage storage1( alloc);
			storage1.open("d:\\db1.data", false, false,  false, false, 65536);
			embDB::SimpleTransactions rtran(alloc, &storage1);
			rtran.setType(embDB::eTT_SELECT);
			TBTreeRO treeRO(nRootPageIofoTree, &rtran, alloc, 10);
			TElem *pFindkey = NULL;
			for (__int64 i = 0; i < nCount; ++i)
			{
				TElem key;
				key.m_key = i;

				pFindkey = treeRO.search(key);
				if(!pFindkey || pFindkey->m_key != i)
					std::cout << "Not found " << i << std::endl;
			}
		}
	
	}
	return 0;
}

