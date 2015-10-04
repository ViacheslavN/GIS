#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/StringLeafNodeCompressor.h"
#include "CommonLibrary/DebugTime.h"

typedef embDB::BPInnerNodeSimpleCompressorV2<int64> TInnerCompressor;
typedef embDB::BPStringLeafNodeSimpleCompressor<int64> TLeafCompressor;

typedef embDB::BPTreeInnerNodeSetv2<int64, embDB::IDBTransactions, TInnerCompressor> TInnerNode;
typedef embDB::BPTreeLeafNodeMapv2<int64, CommonLib::CString, embDB::IDBTransactions, TLeafCompressor, embDB::TBPVector<int64>,
	embDB::TBPVectorNoPOD<CommonLib::CString> > TLeafNode;

typedef embDB::BPTreeNodeMapv2<int64, CommonLib::CString, embDB::IDBTransactions, TInnerCompressor, TLeafCompressor, TInnerNode, TLeafNode> TBPTreeNode;

typedef embDB::TBPMapV2 <int64,  CommonLib::CString, embDB::comp<int64>, embDB::IDBTransactions,
embDB::BPInnerNodeSimpleCompressorV2<int64>,
embDB::BPStringLeafNodeSimpleCompressor<int64>, TInnerNode, TLeafNode, TBPTreeNode> TBMapString;




template<class Tran>
void insertINBTreeMapString  (CommonLib::alloc_t* pAlloc, uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran)
{
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBMapString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CommonLib::CString sString;
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			sString.format(L"Строка_%I64d", i);
			if(!tree.insert(i, sString))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	else
	{
	
		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{
			sString.format(L"Строка_%I64d", i);
			if(!tree.insert(i, sString))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	tmInsert = time.stop();
	time.start();
	tree.commit();

	treeCom = time.stop();
	time.start();
	pTran->commit();
	tranCom = time.stop();
	nTreeRootPage = tree.getPageBTreeInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;

}



template<class Tran>
void searchINBTreeMapString  (CommonLib::alloc_t* pAlloc, uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBMapString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CommonLib::CString sString;
	int64 nNotFound = 0;
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			sString.format(L"Строка_%I64d", i);

			TBMapString::iterator it = tree.find(i);
			if(it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if( i != it.key())
			{
				std::cout << "Key not found " << i << std::endl;
				nNotFound++;
			}
			else if( sString != it.value())
			{
				CommonLib::CString str = it.value();
				std::cout << "String not found search string" << sString.cstr() << " found " << it.value().cstr() <<std::endl;
				nNotFound++;
			}
		
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	else
	{

		int64 nCount = nStart - nEndStart;
		for (__int64 i = nStart; i > nEndStart; --i)
		{
			sString.format(L"Строка_%I64d", i);
			TBMapString::iterator it = tree.find(i);
			if(it.isNull())
			{
				std::cout << "Not found " << i << std::endl;
				nNotFound++;
			}
			else if( i != it.key())
			{
				std::cout << "Key not found " << i << std::endl;
				nNotFound++;
			}
			else if( sString != it.value())
			{
				std::cout << "String not found " << i << std::endl;
				nNotFound++;
			}

			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
		}
	}
	tmInsert = time.stop();
	time.start();
	tree.commit();

	treeCom = time.stop();
	time.start();
	pTran->commit();
	tranCom = time.stop();
	nTreeRootPage = tree.getPageBTreeInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;

}




template<class TTransaction>
int64 CreateTree(CommonLib::alloc_t *pAlloc, const wchar_t *pszName, uint32 nPageSize, embDB::eStringCoding sc, uint32 nLen)
{
	embDB::CStorage storage( pAlloc, 10000);
	if(!storage.open(pszName, false, false,  true, false, nPageSize))
		return -1;
	embDB::FilePagePtr pPage = storage.getNewPage();
	int64 intnStorageInfoPage = pPage->getAddr();
	storage.initStorage(pPage->getAddr());
	storage.saveStorageInfo();

	TTransaction tran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\tran.data", &storage, 1);
	tran.begin();

	embDB::FilePagePtr pTreeRootPage = tran.getNewPage();

	embDB::FilePagePtr pLeafCompRootPage = tran.getNewPage();
	TLeafCompressor::TLeafCompressorParams compParams;
	compParams.setRootPage(pLeafCompRootPage->getAddr());
	compParams.SetStringLen(nLen);
	compParams.setStringCoding(sc);
	compParams.save(&tran);




	TBMapString tree(pTreeRootPage->getAddr(), &tran, pAlloc, 100);
	tree.setCompPageInfo(-1, pLeafCompRootPage->getAddr());
	tree.saveBTreeInfo(); 
	tree.commit();
	tran.commit();

	return pTreeRootPage->getAddr();
}

void TestBPStringTree()
{

	CommonLib::CString str(L"Строка_1");
	CommonLib::CBlob blob;

	int nLen = str.calcUTF8Length() + 1;
	blob.reserve(nLen);
	
	str.exportToUTF8((char*)blob.buffer(), nLen);

	CommonLib::CString str2;
	str2.loadFromUTF8((char*)blob.buffer());

	CommonLib::simple_alloc_t alloc;
	int64 nRootTreePage = CreateTree<embDB::CDirectTransactions>(&alloc, L"d:\\db\\BPTreeString.data", 8192, embDB::scUTF8, 100);

	{
		embDB::CStorage storage( &alloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		embDB::CDirectTransactions InsertTran(&alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		insertINBTreeMapString<embDB::CDirectTransactions>(&alloc, 100, 0, 100000, 1, nRootTreePage, &InsertTran);
	}

	{
		embDB::CStorage storage( &alloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		embDB::CDirectTransactions InsertTran(&alloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		searchINBTreeMapString<embDB::CDirectTransactions>(&alloc, 100, 0, 10000, 1, nRootTreePage, &InsertTran);
	}
	
}