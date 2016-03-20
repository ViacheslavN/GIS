#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_REDO_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_REDO_PAGE_MANAGER_H_

#include <vector>
#include "TranStorage.h"
#include "embDBInternal.h"
#include "TranUndoPageManager.h"
#include "storage.h"
#include "PageVectorLazySave.h"
namespace embDB
{
	




	class CTranRedoPageManager
	{

		struct sRedoPageInfo
		{

			sRedoPageInfo(int64 _nDBAddr = -1, int64 _nTranAddr = -1, uint32 _nFlags = 0, uint32 _nPageSize = 0) :
				nDBAddr(_nDBAddr), nTranAddr(_nTranAddr), nFlags(_nFlags), nPageSize(_nPageSize)
			{}
			int64 nDBAddr;
			int64 nTranAddr;
			uint32 nFlags;
			uint32 nPageSize;
		};


		struct sRedoHeader
		{
			int64 nUndoBlock;
			int64 nRedoBlock;
		};
		typedef std::vector<sRedoPageInfo> TRedoDBPages;

	public:
		CTranRedoPageManager(IDBTransaction *pTran, CTranStorage *pStorage);
		~CTranRedoPageManager();

		void setFirstPage(int64 nPage, bool bCreate);
		bool add_undo(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize);
		bool add(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize);
		bool redo(CTranStorage *pTranStorage, IDBStorage* pDBStorage);
		bool save();

		class TReaderWriter
		{
		public:
			void write(const sRedoPageInfo& value, CommonLib::FxMemoryWriteStream& stream)
			{
				stream.write(value.nDBAddr);
				stream.write(value.nTranAddr);
				stream.write(value.nFlags);
			}
			void read(sRedoPageInfo& value, CommonLib::FxMemoryReadStream& stream)
			{
				stream.read(value.nDBAddr);
				stream.read(value.nTranAddr);
				stream.read(value.nFlags);
			}

			uint32 rowSize()
			{
				return 2 * sizeof(int64) + sizeof(uint32);
			}
		};

		typedef TPageVectorLazySave<sRedoPageInfo, TReaderWriter> TRedoPageList;
	private:
		IDBTransaction *m_pTran;
		CTranStorage *m_pStorage;
		uint32 m_nLastPos;
		TRedoPageList m_RedoPages;
		TRedoPageList m_UndoPages;
		int64 m_nRootPage;
		sRedoHeader m_Header;
	};
}
#endif