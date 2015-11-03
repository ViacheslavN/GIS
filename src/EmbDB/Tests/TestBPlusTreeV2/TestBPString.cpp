#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/StringLeafNodeCompressor.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/FixedStringTree.h"
#include "../../EmbDB/StringTree.h"


typedef embDB::BPInnerNodeSimpleCompressorV2<int64> TInnerCompressor;
typedef embDB::BPFixedStringLeafNodeCompressor<int64> TLeafCompressor;

typedef embDB::BPTreeInnerNodeSetv2<int64, embDB::IDBTransaction, TInnerCompressor> TInnerNode;
typedef embDB::BPTreeLeafNodeMapv2<int64, embDB::sFixedStringVal, embDB::IDBTransaction, TLeafCompressor> TLeafNode;

typedef embDB::BPTreeNodeMapv2<int64, embDB::sFixedStringVal, embDB::IDBTransaction, TInnerCompressor, TLeafCompressor, TInnerNode, TLeafNode> TBPTreeNode;

typedef embDB::TBPMapV2 <int64,  embDB::sFixedStringVal, embDB::comp<int64>, embDB::IDBTransaction,
embDB::BPInnerNodeSimpleCompressorV2<int64>,
embDB::BPFixedStringLeafNodeCompressor<int64>, TInnerNode, TLeafNode, TBPTreeNode> TBMapString;


/*class TBPString : public TBMapString
{
	public:

		TBPString(int64 nPageBTreeInfo, embDB::IDBTransactions* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBMapString(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32)
			{

			}

		bool insert(int64 nValue, const CommonLib::CString& sString)
		{
			embDB::sStringVal sValue;
			if(m_LeafCompParams->GetStringCoding() == embDB::scASCII)
			{
				sValue.m_nLen = sString.length() + 1;
				sValue.m_pBuf = (byte*)m_pAlloc->alloc(sValue.m_nLen);
				strcpy((char*)sValue.m_pBuf, sString.cstr());
				sValue.m_pBuf[sValue.m_nLen] = 0;
			}
			else if(m_LeafCompParams->GetStringCoding() == embDB::scUTF8)
			{
				sValue.m_nLen  = sString.calcUTF8Length() + 1;
				sValue.m_pBuf = (byte*)m_pAlloc->alloc(sValue.m_nLen);
				sString.exportToUTF8((char*)sValue.m_pBuf, sValue.m_nLen);
			}
			return TBMapString::insert(nValue, sValue);

		}
};*/

class CBufAlloc: public CommonLib::alloc_t
{
public:
	CBufAlloc() : pos(0)
	{

	}
	virtual void* alloc(size_t size)
	{
		byte *pBuf =  buf + pos;
		pos += size;
		return pBuf;
	}
	virtual void  free(void* buf)
	{

	}
	void clear()
	{
		pos = 0;
	}
private:
	byte buf[1024];
	int pos;
};

template<class Tran>
void insertINBTreeMapString  (CommonLib::alloc_t* pAlloc, uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran)
{

	typedef embDB::TBPStringTree<int64, Tran> TBPString;
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBPString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.setOneSplit(true);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CBufAlloc alloc;
	 
	CommonLib::CString sString(&alloc);
	CommonLib::CString sBigString;

	sBigString += L"begin";
	for (size_t i =0; i < 10000; ++i)
	{

		sBigString += _T("ST");
	}
	

	
	if(nStart < nEndStart)
	{
		
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			
			sString.format(L"Строка_Строка_Строка_Строка_%I64d", i);

			if(i%10000 == 0)
			{
				sBigString += sString;
			}

			if(!tree.insert(i, i%10000 == 0 ? sBigString : sString))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
			}
			alloc.clear();
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
void searchINBTreeMapString  (CommonLib::alloc_t* pAlloc, 
	uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran, embDB::eStringCoding sCode)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	typedef embDB::TBPStringTree<int64, Tran> TBPString;
	TBPString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CommonLib::CString sString(pAlloc);
	int64 nNotFound = 0;

	CommonLib::CString sBigString;

	sBigString += L"begin";
	for (size_t i =0; i < 10000; ++i)
	{

		sBigString += _T("ST");
	}
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			sString.format(L"Строка_Строка_Строка_Строка_%I64d", i);

			TBPString::iterator it = tree.find(i);
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
			else /*if( sString != it.value())*/
			{
				const embDB::sStringVal& val = it.value();
				if(i%10000 == 0)
				{
					if(2990000 == i)
					{
						int d = 0;
						d++;
					}
					sBigString += sString;
					embDB::ReadStreamPage readStream(pTran, 1024*1024);
					readStream.open(val.m_nPage, val.m_nPos);
					CommonLib::CBlob blob(val.m_nLen);
					readStream.read(blob.buffer(), val.m_nLen);
					CommonLib::CString sFoundStr;
					if(sCode == embDB::scASCII)
						sFoundStr.loadFromASCII((const char*)blob.buffer());
					else
						sFoundStr.loadFromUTF8((const char*)blob.buffer());

					if(sBigString != sFoundStr)
					{
						std::cout << "String not found search string" << sString.cstr() << " found " << sFoundStr.cstr() <<std::endl;
						nNotFound++;
					}

				}
				else
				{
				
					CommonLib::CString sFoundStr;
					if(sCode == embDB::scASCII)
						sFoundStr.loadFromASCII((const char*)val.m_pBuf);
					else
						sFoundStr.loadFromUTF8((const char*)val.m_pBuf);

					if(sString != sFoundStr)
					{
						std::cout << "String not found search string" << sString.cstr() << " found " << sFoundStr.cstr() <<std::endl;
						nNotFound++;
					}
				}

			
			
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
			sString.format(L"Строка_Строка_Строка_Строка_%I64d", i);
			TBPString::iterator it = tree.find(i);
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
			/*else if( sString != it.value())
			{
				std::cout << "String not found " << i << std::endl;
				nNotFound++;
			}*/

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
	compParams.SetMaxPageStringSize(400);
	compParams.save(&tran);




	TBMapString tree(pTreeRootPage->getAddr(), &tran, pAlloc, 100);
	tree.setCompPageInfo(-1, pLeafCompRootPage->getAddr());
	tree.saveBTreeInfo(); 
	tree.commit();
	tran.commit();

	return pTreeRootPage->getAddr();
}
template<class Transactions>
void TestBPStringTreeImpl(CommonLib::alloc_t *pAlloc, int64 nBegin, int64 nEnd, uint32 nBPCache,embDB::eStringCoding coding )
{
	 
	int64  nStep = (nEnd - nBegin)/100;
 
	int64 nRootTreePage = CreateTree<Transactions>(pAlloc, L"d:\\db\\BPTreeString.data", 8192, coding, 100);

	{
		embDB::CStorage storage( pAlloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		insertINBTreeMapString<embDB::IDBTransaction>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran);
	}

	{
		embDB::CStorage storage(pAlloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		searchINBTreeMapString<embDB::IDBTransaction>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran, coding);
	}
	
}


void TestBPStringTree()
{
		CommonLib::simple_alloc_t alloc;
		TestBPStringTreeImpl<embDB::CDirectTransaction>(&alloc, 0, 3000000, 10, embDB::scUTF8);
};
