#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_UNDO_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_UNDO_PAGE_MANAGER_H_

#include <vector>
#include "TranStorage.h"
#include "IDBTransactions.h"
#include "TranUndoPageManager.h"
#include "storage.h"
#include "PageVectorLazySave.h"
namespace embDB
{
	struct sUndoPageInfo
	{
		int64 nDBAddr;
		int64 nTranAddr;
		int32 nFlags;
	};
	typedef std::vector<sUndoPageInfo> TUndoDBPages;




	class CTranUndoPageManager
	{
	public:
		CTranUndoPageManager(IDBTransactions *pTran, CTranStorage *pStorage);
		~CTranUndoPageManager();
		
		void setFirstPage(int64 nPage)
		{
			m_undoPages.setRoot(nPage);
		}
		bool add(const sUndoPageInfo& PageInfo);
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

			size_t rowSize()
			{
				return sizeof(sUndoPageInfo);
			}
		};

		typedef TPageVectorLazySave<sUndoPageInfo, TReaderWriter> TUndoPageList;
	private:
		IDBTransactions *m_pTran;
		CTranStorage *m_pStorage;
		int64 m_nCurPage;
		size_t m_nLastPos;
		TUndoPageList m_undoPages;
	};
}
#endif