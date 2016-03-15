#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/FixedStringBPLeafNode.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/FixedStringTree.h"
#include "../../EmbDB/StringTree.h"
#include "../../EmbDB/FixedStringACCompressor.h"
#include "../../EmbDB/BaseInnerNodeDIffCompress.h"
#include "../../EmbDB/BaseLeafNodeCompDiff.h"
#include "../../EmbDB/BaseInnerNodeDIffCompress2.h"
#include "../../EmbDB/BaseValueDiffCompressor.h"
#include "../../EmbDB/SignedNumLenDiffCompress.h"


//typedef embDB::BPInnerNodeSimpleCompressorV2<int64> TInnerCompressor;
typedef embDB::BPFixedStringLeafNodeCompressor<int64> TLeafCompressor;

typedef embDB::TBaseValueDiffCompress<int64, embDB::SignedDiffNumLenCompressor64i> TInnerLinkCompress;
typedef embDB::TBPBaseInnerNodeDiffCompressor2<int64, embDB::OIDCompressor, TInnerLinkCompress> TInnerCompressor;
	 

typedef embDB::BPTreeInnerNodeSetv2<int64, embDB::IDBTransaction, TInnerCompressor> TInnerNode;
typedef embDB::BPTreeLeafNodeMapv2<int64, embDB::sFixedStringVal, embDB::IDBTransaction, TLeafCompressor> TLeafNode;

typedef embDB::BPTreeNodeMapv2<int64, embDB::sFixedStringVal, embDB::IDBTransaction, TInnerCompressor, TLeafCompressor, TInnerNode, TLeafNode> TBPTreeNode;

typedef embDB::TBPMapV2 <int64,  embDB::sFixedStringVal, embDB::comp<int64>, embDB::IDBTransaction,
embDB::BPInnerNodeSimpleCompressorV2<int64>,
embDB::BPFixedStringLeafNodeCompressor<int64>, TInnerNode, TLeafNode, TBPTreeNode> TBFixedMapString;


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
	
	typedef embDB::TBPFixedString<int64, Tran> TBPString;
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBPString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.SetMinSplit(true);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CBufAlloc alloc;
	 
	embDB::CBPTreeStatistics statInfo;
	tree.SetBPTreeStatistics(&statInfo);

	CommonLib::CString sString(&alloc);
/*	CommonLib::CString sBigString;

	sBigString += L"begin";
	for (size_t i =0; i < 10000; ++i)
	{

		sBigString += _T("ST");
	}*/
	

	
	if(nStart < nEndStart)
	{
		
		int64 nCount = nEndStart - nStart;
		CommonLib::CWriteFileStream stream;
		stream.open(L"D:\\test\\files\\1str.txt", CommonLib::ofmCreateAlways, CommonLib::arWrite, CommonLib::smNoMode);

		char mUtf8Buf[2148];
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			
			sString.format(L"Cтрока_Строка_Строка__%I64d", i);

		/*	if(i%10000 == 0)
			{
				sBigString += sString;
			}
			*/
			uint32 nUft8Len = sString.calcUTF8Length()  + 1;
			sString.exportToUTF8(mUtf8Buf, nUft8Len);
			stream.write((byte*)mUtf8Buf, nUft8Len);

			if(!tree.insert(i, /*i%10000 == 0 ? sBigString : */sString))
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

	std::cout << "Create Inner Nodes: " << statInfo.GetCreateNode(false) << " Create Leaf Nodes: " <<  statInfo.GetCreateNode(true) <<	std::endl;
	std::cout << "Load Inner Nodes: " << statInfo.GetLoadNode(false) << " Load Leaf Nodes: " <<  statInfo.GetLoadNode(true) <<	std::endl;
	std::cout << "Save Inner Nodes: " << statInfo.GetSaveNode(false) << " Save Leaf Nodes: " <<  statInfo.GetSaveNode(true) <<	std::endl;


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
	typedef embDB::TBPFixedString<int64, Tran> TBPString;
	TBPString tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize, 8192);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	CommonLib::CString sString(pAlloc);
	int64 nNotFound = 0;	
	if(nStart < nEndStart)
	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			sString.format(L"Cтрока_Строка_Строка__%I64d", i);

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
					const embDB::sFixedStringVal& val = it.value();	
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
	//tree.commit();

	treeCom = time.stop();
	time.start();
	pTran->commit();
	tranCom = time.stop();
	nTreeRootPage = tree.getPageBTreeInfo();


	std::cout << "Insert end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
}




template<class TTransaction>
int64 CreateTree(CommonLib::alloc_t *pAlloc, const wchar_t *pszName, uint32 nPageSize, embDB::eStringCoding sc, uint32 nLen)
{
 
	embDB::CStorage storage( pAlloc, 10000);
	if(!storage.open(pszName, false, false,  true, false))
		return -1;
	embDB::FilePagePtr pPage = storage.getNewPage(nPageSize);
	int64 intnStorageInfoPage = pPage->getAddr();
	storage.initStorage(pPage->getAddr());
	storage.saveStorageInfo();

	storage.AddRef();


	TTransaction tran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\tran.data", &storage, 1);
	tran.begin();

	embDB::FilePagePtr pTreeRootPage = tran.getNewPage(256);

	uint64 nTreeRootPage = pTreeRootPage->getAddr();

	

	embDB::FilePagePtr pLeafCompRootPage = tran.getNewPage(256);
	TLeafCompressor::TLeafCompressorParams compParams;
//	compParams.setRootPage(pLeafCompRootPage->getAddr());
	compParams.SetStringLen(nLen);
	compParams.setStringCoding(sc);
	//compParams.SetMaxPageStringSize(400);
	//compParams.save(&tran);

	typedef embDB::TBPFixedString<int64, TTransaction> TBPString;
	TBFixedMapString tree(-1, (embDB::IDBTransaction*)&tran, pAlloc, 100, 8192);
	
//	tree.saveBTreeInfo(); 
	tree.init(nTreeRootPage, NULL, &compParams); 
	tree.commit();

	pTreeRootPage.release();
	pLeafCompRootPage.release();
	tran.commit();


	return nTreeRootPage;
}
template<class Transactions>
void TestBPStringTreeImpl(CommonLib::alloc_t *pAlloc, int64 nBegin, int64 nEnd, uint32 nBPCache,embDB::eStringCoding coding )
{
	 
	int64  nStep = (nEnd - nBegin)/100;
 
	int64 nRootTreePage = CreateTree<Transactions>(pAlloc, L"d:\\db\\BPTreeString.data", 8192, coding, 128);

	{
		embDB::CStorage storage( pAlloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false))
			return;

		storage.AddRef();
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_MODIFY, L"d:\\db\\inserttran.data", &storage, 1);
		InsertTran.begin();

		insertINBTreeMapString<embDB::IDBTransaction>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran);

		std::cout << "File Size " << storage.getFileSize() <<	std::endl;
	}

	{
		embDB::CStorage storage(pAlloc, 10000);
		storage.AddRef();
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_SELECT, L"d:\\db\\inserttran.data", &storage, 1);


		searchINBTreeMapString<embDB::IDBTransaction>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran, coding);
	}
	
}


void TestBPFixedStringTree()
{
		CommonLib::simple_alloc_t alloc;
		TestBPStringTreeImpl<embDB::CTransaction>(&alloc, 0, 1000000, 10, embDB::scUTF8);
};
