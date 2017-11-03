#include "stdafx.h"
#include   "../../EmbDB/storage/storage.h"
#include   "../../EmbDB/Utils/streams/WriteStreamPage.h"
#include   "../../EmbDB/Utils/streams/ReadStreamPage.h"



void TestPageStream()
{
	CommonLib::simple_alloc_t alloc;
	embDB::CStorage storage(&alloc, 1000);
	storage.AddRef();
	storage.open(L"d:\\db\\pagestorage.data", false, false, true, false);
	embDB::FilePagePtr pPage = storage.getNewPage(PAGE_SIZE_8K);
	storage.initStorage(pPage->getAddr());
 	storage.saveStorageInfo();
	bool bCheckCRC = false;
	short nType1 = 1;
	short nType2 = 2;
	int64 nAddr = storage.getNewPageAddr(PAGE_SIZE_8K);
	{
		embDB::TWriteStreamPage<embDB::IDBStorage> writeStream(&storage, PAGE_SIZE_8K, bCheckCRC, nType1, nType2);
	

		writeStream.open(nAddr, 0);


		for (int64 i = 0; i < 10000; ++i)
		{
			writeStream.write(i);
		}
		writeStream.Save();
	}
		
	{

		embDB::TWriteStreamPage<embDB::IDBStorage> writeStream(&storage, PAGE_SIZE_8K, bCheckCRC, nType1, nType2);

		writeStream.open(nAddr, 0, true);
		for (int64 i = 0; i <100000; ++i)
		{
			writeStream.write(i);
		}
		writeStream.Save();

	}


	{
		embDB::TReadStreamPage<embDB::IDBStorage> readStream(&storage, PAGE_SIZE_8K, bCheckCRC, nType1, nType2);
		readStream.open(nAddr, 0);
		for (int64 i = 0; i < 20000; ++i)
		{
			int64 d = readStream.readInt64();
			if (d != i)
			{
				d++;
				assert(false);
			}
		}

	}

}