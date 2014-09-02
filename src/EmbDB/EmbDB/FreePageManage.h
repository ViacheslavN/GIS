#ifndef _EMBEDDED_DATABASE_FREE_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_FREE_PAGE_MANAGER_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "simple_stack.h"
#include "BaseBPSet.h"
#include "DirectTransactions.h"
#include "BTVector.h"
#include "BitMap.h"
#include <map>
namespace embDB
{
	class CStorage;

	typedef embDB::TBPlusTreeSet<int64, int64, embDB::comp<int64>, embDB::IDBTransactions> TBTreeSet;
	class CFreePageManager
	{
		public:
			CFreePageManager(CStorage* pStorage, CommonLib::alloc_t * pAlloc);
			~CFreePageManager();
			bool init(int64 nRootAddr, bool bNew);
			bool load();
			bool save();
			bool addPage(int64 nAddr);
			int64 getFreePage();
			bool removeFromFreePage(int64 nAddr);
			
			int64 getRoot(){return m_nRootPage;};
			
	    private:

			bool AddFreeMap(int64 nAddr, uint64 nBlockNum,  bool bNew);
			typedef TSimpleStack<int64> TFreePages;

			struct FileFreeMap
			{
				FileFreeMap(uint32 nAddrLen) : m_bChange(false), m_nAddr(-1), m_nBlockNum(0), m_nBeginAddr(0), m_nEndAddr(0), m_nAddrLen(nAddrLen)
				{

				}

				bool getBit(int64 nBit)
				{
					assert(nBit >= m_nBeginAddr && nBit <= m_nEndAddr);
					return m_BitMap.getBit(uint32(nBit - m_nBeginAddr));
				}
				bool setBit(int64 nBit, bool bSet)
				{
					assert(nBit >= m_nBeginAddr && nBit <= m_nEndAddr);
					return m_BitMap.setBit(uint32(nBit - m_nBeginAddr), bSet);
				}
				int64 m_nAddr;
				/*

				a[i] = i * Len
				b[i] =  i * Len - 1


				
				*/

				TFreePages m_FreePages;
				CBitMap		m_BitMap;
				bool m_bChange;
				uint64 m_nBlockNum;
				int64 m_nBeginAddr;
				int64 m_nEndAddr;
				uint32 m_nAddrLen;
				bool load(CFilePage* pPage )
				{
					CommonLib::FxMemoryReadStream stream;
					stream.attach(pPage->getRowData(), pPage->getPageSize());
					sFilePageHeader header (stream);
					if(!header.isValid())
					{
						//TO DO Logs
						return false;
					}
					if(header.m_nObjectPageType != STORAGE_PAGE || header.m_nSubObjectPageType != STORAGE_FREE_MAP_PAGE )
					{
						//TO DO Logs
						return false;
					}
		
					byte nbyte;
					m_nBlockNum = stream.readIntu64();

					m_nAddr = pPage->getAddr();
					m_nBeginAddr = m_nBlockNum * m_nAddrLen;
					m_nEndAddr = m_nBeginAddr +  m_nAddrLen - 1;

				//	m_BitMap.setBits(stream.buffer() + stream.pos(), stream.size() - stream.pos());
					m_BitMap.setBits(stream.buffer() + stream.pos(), m_nAddrLen/8);
					//assert(m_nAddrLen == m_BitMap.getBitSize());
					uint64 nPageAddr = m_nBlockNum * m_nAddrLen;
					for (size_t i = 0; i < m_nAddrLen/8; ++i )
					{

						stream.read(nbyte);
						if(nbyte == 0)
						{
							nPageAddr += 8;
							continue;
						}
						for (size_t b = 0; b < 8; ++b)
						{
							if(nbyte & (0x01 << b))
								m_FreePages.push(nPageAddr);
							++nPageAddr;
						}

					}

					return true;
				}
				bool save(CFilePage* pPage )
				{
					CommonLib::FxMemoryWriteStream stream;
					stream.attach(pPage->getRowData(), pPage->getPageSize());
					sFilePageHeader header (stream, STORAGE_PAGE, STORAGE_FREE_MAP_PAGE);
					stream.write(m_nBlockNum);
				//	m_BitMap.setBits(stream.buffer() + stream.pos(), stream.size() - stream.pos());
					stream.write(m_BitMap.getBits(), m_BitMap.size());
					header.writeCRC32(stream);
					return true;
				}




				//bitstream
			};

			typedef std::map<uint64, FileFreeMap*> TMapFreeMaps;
		private:
			
			CStorage* m_pStorage;
			CommonLib::alloc_t * m_pAlloc;

			int64 m_nRootPage;
			int64 m_nFreeMapLists;

			TMapFreeMaps m_FreeMaps;
			uint32 m_nAddrLen;

			bool m_bNewFreeMap;

	
	};
}
#endif