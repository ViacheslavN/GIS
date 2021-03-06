#ifndef _EMBEDDED_DATABASE_FREE_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_FREE_PAGE_MANAGER_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../utils/simple_stack.h"
 
#include "../utils/BitMap.h"
#include <map>
#include "../utils/PageVector.h"
#include "../utils/PageVectorLazySave.h"
#include "embDBInternal.h"
namespace embDB
{
	class CStorage;

//	typedef embDB::TBPlusTreeSet<int64, int64, embDB::comp<int64>, embDB::IDBTransaction> TBTreeSet;
	class CFreePageManager
	{
		public:
			CFreePageManager(CStorage* pStorage, CommonLib::alloc_t * pAlloc, bool bCheckCRC);
			~CFreePageManager();
			bool init(int64 nRootAddr, bool bNew);
			bool load();
			bool save();
			bool addPage(int64 nAddr);
			int64 getFreePage(bool bSave = false);
			bool removeFromFreePage(int64 nAddr);
			int64 getRoot(){return m_nRootPage;};
			bool saveForUndoState(IDBTransaction *pTran);
			bool undo(IDBTransaction *pTran, int64 nPageBegin);

		
	    private:

			bool AddFreeMap(int64 nAddr, uint64 nBlockNum,  bool bNew);
			typedef TSimpleStack<int64> TFreePages;

			struct FileFreeMap
			{
				FileFreeMap(uint32 nAddrLen, bool bCheckCRC) : m_bChange(false), m_nAddr(-1), m_nBlockNum(0),
					m_nBeginAddr(0), m_nEndAddr(0), m_nAddrLen(nAddrLen), m_bCheckCRC(bCheckCRC)
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
 
				TFreePages m_FreePages;
				CBitMap		m_BitMap;
				bool m_bChange;
				uint64 m_nBlockNum;
				int64 m_nBeginAddr;
				int64 m_nEndAddr;
				uint32 m_nAddrLen;
				bool m_bCheckCRC;
				bool load(CFilePage* pPage )
				{
					CommonLib::FxMemoryReadStream stream;
					stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
					sFilePageHeader header (stream, pPage->getPageSize(), m_bCheckCRC);
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
					for (uint32 i = 0; i < m_nAddrLen/8; ++i )
					{

						stream.read(nbyte);
						if(nbyte == 0)
						{
							nPageAddr += 8;
							continue;
						}
						for (uint32 b = 0; b < 8; ++b)
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
					stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
					sFilePageHeader header (stream, STORAGE_PAGE, STORAGE_FREE_MAP_PAGE, pPage->getPageSize(), m_bCheckCRC);
					stream.write(m_nBlockNum);
				//	m_BitMap.setBits(stream.buffer() + stream.pos(), stream.size() - stream.pos());
					stream.write(m_BitMap.getBits(), m_BitMap.size());
					header.writeCRC32(stream);
					return true;
				}




				//bitstream
			};

			typedef std::map<uint64, FileFreeMap*> TMapFreeMaps;
			struct sUndoPageInfo
			{
				int64 m_BitMapAddr;
				int64 m_nBitMapAddInTran;
			};
 
			class UndoPageInfoReaderWriter
			{
			public:
			 
				void write(const sUndoPageInfo& undoPageInfo, CommonLib::FxMemoryWriteStream& stream)
				{
					stream.write(undoPageInfo.m_BitMapAddr);
					stream.write(undoPageInfo.m_nBitMapAddInTran);
				}
				void read(sUndoPageInfo& undoPageInfo, CommonLib::FxMemoryReadStream& stream)
				{
					stream.read(undoPageInfo.m_BitMapAddr);
					stream.read(undoPageInfo.m_nBitMapAddInTran);
				}

				uint32 rowSize()
				{
					return 2*sizeof(int64);
				}
			};
			typedef TPageVectorLazySave<sUndoPageInfo, UndoPageInfoReaderWriter> TUndoVector;


			bool SaveUndoPage(int64 nPage, IDBTransaction *pTran, TUndoVector& vec);
		private:
		
			

			std::vector<int64> m_vecNewFreeMaps;
			typedef TPageVector<int64> TFreeMapLists;
			TFreeMapLists m_ListFreeMaps;
			CStorage* m_pStorage;
			CommonLib::alloc_t * m_pAlloc;

			int64 m_nRootPage;
			int64 m_nFreeMapLists;

			TMapFreeMaps m_FreeMaps;
			uint32 m_nAddrLen;
			uint32 m_nPageSize;
			bool m_bCheckCRC;

	
	};
}
#endif