#include "stdafx.h"
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/stream.h"
#include "CommonLibrary/blob.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "../../EmbDB/BaseBPMapv2.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "CommonLibrary/DebugTime.h"
#include "../../EmbDB/BlobTree.h"
#include "../../EmbDB/BlobLeafNodeCompressor.h"
 


 

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
void insertINBTreeMapBlob  (CommonLib::alloc_t* pAlloc, uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran)
{

	typedef embDB::TBPBlobTree<int64, Tran> TBlobTree;
	std::cout << "Insert Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	TBlobTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	//tree.setOneSplit(true);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
	

	CommonLib::CBlob MinBlob(300);
	CommonLib::CBlob MaxBlob(300000);
 
	CommonLib::FxMemoryWriteStream stream;
	 

	
 
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
			
		

			if(i%10000 == 0)
			{

				stream.attach(MaxBlob.buffer(), MaxBlob.size());
			}
			else
			{
				stream.attach(MinBlob.buffer(), MinBlob.size());
			}

			stream.write(i);
			stream.seek(sizeof(int64), CommonLib::soFromEnd);

			stream.write(i);

			if(!tree.insert(i, i%10000 == 0 ? MaxBlob : MinBlob))
			{
				std::cout   << "Error Insert key:  " << i << std::endl;
			}
			n++;
			if(i%nStep == 0)
			{
				std::cout << n  << "  " << (n* 100)/nCount << " %" << '\r';
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
void searchINBTreeMapBlob   (CommonLib::alloc_t* pAlloc, 
	uint32 nCacheBPTreeSize, int64 nStart, int64 nEndStart, int64 nStep, int64& nTreeRootPage, Tran* pTran, embDB::eStringCoding sCode)
{
	std::cout << "Search Test"  << std::endl;
	CommonLib::TimeUtils::CDebugTime time;
	double tmInsert = 0;
	double treeCom = 0;
	double tranCom  = 0;
	typedef embDB::TBPBlobTree<int64, Tran> TBlobTree;
	TBlobTree tree(nTreeRootPage, pTran, pAlloc, nCacheBPTreeSize);
	tree.loadBTreeInfo(); 
	time.start();
	int64 n = 0;
 
	int64 nNotFound = 0;



	CommonLib::CBlob MinBlob(300);
	CommonLib::CBlob MaxBlob(300000);
	CommonLib::FxMemoryReadStream stream;

	CommonLib::CBlob CacheBlob(300000);

	{
		int64 nCount = nEndStart - nStart;
		for (__int64 i = nStart; i < nEndStart; ++i)
		{
	 

			TBlobTree::iterator it = tree.find(i);
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
				const embDB::sBlobVal& BlobVal = it.value();
				int64 nBeginKey = -1, nEndKey = -1;


				if(i%10000 == 0)
				{
					if(BlobVal.m_nSize != MaxBlob.size())
					{
						std::cout << "Error Blob Size  key: " << i << " Must " << MaxBlob.size() <<" Fact " <<  BlobVal.m_nSize <<std::endl;
						nNotFound++;
						continue;
					
					}

				   	embDB::ReadStreamPage readStream(pTran, 1024*1024);
					readStream.open(BlobVal.m_nPage, BlobVal.m_nBeginPos);
					readStream.read(CacheBlob.buffer(), BlobVal.m_nSize);
					stream.attach(CacheBlob.buffer(), BlobVal.m_nSize);
				}
				else
				{
				
					if(BlobVal.m_nSize != MinBlob.size())
					{
						std::cout << "Error Blob Size  key: " << i << " Must " << MinBlob.size() <<" Fact " <<  BlobVal.m_nSize <<std::endl;
						nNotFound++;
						continue;

					}
					stream.attach(BlobVal.m_pBuf, BlobVal.m_nSize);
				}

			
				nBeginKey = stream.readInt64();
				stream.seek(sizeof(int64), CommonLib::soFromEnd);
				
				nEndKey = stream.readInt64();

				if(i != nBeginKey || i != nEndKey)
				{
					std::cout << "Error Blob   key: " << i << " Begin " << nBeginKey <<" End " <<  nEndKey <<std::endl;
				}
			
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


	std::cout << "Search end key start: " << nStart << " key end: " << nEndStart << " Total time: " << (tmInsert + treeCom + tranCom) <<
		" time insert: " << tmInsert << " time tree commit: " << treeCom << " Tran commit: " << tranCom <<	std::endl;
	std::cout << "Tree inner node : " << tree.m_BTreeInfo.m_nInnerNodeCounts<< " Tree leaf node : " << tree.m_BTreeInfo.m_nLeafNodeCounts <<	std::endl;

}




template<class TTransaction>
int64 CreateTree(CommonLib::alloc_t *pAlloc, const wchar_t *pszName, uint32 nPageSize, uint32 nLen)
{
	embDB::CStorage storage( pAlloc, 10000);
	if(!storage.open(pszName, false, false,  true, false, nPageSize))
		return -1;

	typedef embDB::TBPBlobTree<int64,  embDB::IDBTransactions> TBlobTree;

	embDB::FilePagePtr pPage = storage.getNewPage();
	int64 intnStorageInfoPage = pPage->getAddr();
	storage.initStorage(pPage->getAddr());
	storage.saveStorageInfo();

	TTransaction tran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, "d:\\db\\tran.data", &storage, 1);
	tran.begin();

	embDB::FilePagePtr pTreeRootPage = tran.getNewPage();

	embDB::FilePagePtr pLeafCompRootPage = tran.getNewPage();
	embDB::BlobLeafNodeCompressor<int64, TTransaction>::TLeafCompressorParams compParams;
	compParams.setRootPage(pLeafCompRootPage->getAddr());
	compParams.SetMaxPageBlobSize(400);
	compParams.save(&tran);




	TBlobTree tree(pTreeRootPage->getAddr(), ( embDB::IDBTransactions*)&tran, pAlloc, 100);
	tree.setCompPageInfo(-1, pLeafCompRootPage->getAddr());
	tree.saveBTreeInfo(); 
	tree.commit();
	tran.commit();

	return pTreeRootPage->getAddr();
}
template<class Transactions>
void TestTreeBlobImpl(CommonLib::alloc_t *pAlloc, int64 nBegin, int64 nEnd, uint32 nBPCache,embDB::eStringCoding coding )
{
	 
	int64  nStep = (nEnd - nBegin)/100;
 
	int64 nRootTreePage = CreateTree<Transactions>(pAlloc, L"d:\\db\\BPTreeString.data", 8192, 400);

	{
		embDB::CStorage storage( pAlloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		insertINBTreeMapBlob<embDB::IDBTransactions>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran);
	}

	{
		embDB::CStorage storage(pAlloc, 10000);
		if(!storage.open(L"d:\\db\\BPTreeString.data", false, false,  false, false, 8192))
			return;
		storage.setStoragePageInfo(0);
		storage.loadStorageInfo();
		Transactions InsertTran(pAlloc, embDB::rtUndo, embDB::eTT_UNDEFINED, L"d:\\db\\inserttran.data", &storage, 1);


		searchINBTreeMapBlob<embDB::IDBTransactions>(pAlloc, nBPCache, nBegin, nEnd, nStep, nRootTreePage, &InsertTran, coding);
	}
	
}


void TestTreeBlob()
{
		CommonLib::simple_alloc_t alloc;
		TestTreeBlobImpl<embDB::CDirectTransactions>(&alloc, 0, 3000000, 10, embDB::scUTF8);
};
