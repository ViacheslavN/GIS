#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_UNDO_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_UNDO_PAGE_MANAGER_H_

#include <vector>
#include "TranStorage.h"
#include "embDBInternal.h"
#include "TranUndoPageManager.h"
#include "storage.h"
#include "utils/PageVectorLazySave.h"
namespace embDB
{
	




	class CTranUndoPageManager
	{
	public:
		CTranUndoPageManager(IDBTransaction *pTran, CTranStorage *pStorage, bool bCheckCRC);
		~CTranUndoPageManager();


		struct sUndoPageInfo
		{
			int64 nDBAddr;
			int64 nTranAddr;
			uint32 nFlags;
			uint32 nPageSize;

			sUndoPageInfo(int64 _nDBAddr = -1, int64 _nTranAddr = -1, uint32 _nFlags = 0, uint32 _nPageSize = 0) :
				nDBAddr(_nDBAddr), nTranAddr(_nTranAddr), nFlags(_nFlags), nPageSize(_nPageSize)
			{

			}
		};
		typedef std::vector<sUndoPageInfo> TUndoDBPages;
		
		void setFirstPage(int64 nPage)
		{
			m_undoPages.setRoot(nPage);
		}
		bool add(int64 nDBAddr, int64 nTranAddr, uint32 nFlags, uint32 nPageSize);
		bool undo(CTranStorage *pTranStorage, IDBStorage* pDBStorage);
		bool save();

		class TReaderWriter
		{
		public:
			void write(const sUndoPageInfo& value, CommonLib::FxMemoryWriteStream& stream)
			{
				stream.write(value.nDBAddr);
				stream.write(value.nTranAddr);
				stream.write(value.nFlags);
			}
			void read(sUndoPageInfo& value, CommonLib::FxMemoryReadStream& stream)
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

		typedef TPageVectorLazySave<sUndoPageInfo, TReaderWriter> TUndoPageList;
	private:
		IDBTransaction *m_pTran;
		CTranStorage *m_pStorage;
		int64 m_nCurPage;
		uint32 m_nLastPos;
		TUndoPageList m_undoPages;
	};
}
#endif