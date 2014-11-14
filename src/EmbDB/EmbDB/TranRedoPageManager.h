#ifndef _EMBEDDED_DATABASE_I_TRANSACTIONS_REDO_PAGE_MANAGER_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_REDO_PAGE_MANAGER_H_

#include <vector>
#include "TranStorage.h"
#include "IDBTransactions.h"
#include "TranUndoPageManager.h"
#include "storage.h"
#include "PageVectorLazySave.h"
namespace embDB
{
	




	class CTranRedoPageManager
	{

		struct sUndoPageInfo
		{
			int64 nDBAddr;
			int64 nTranAddr;
			int32 nFlags;
		};
		typedef std::vector<sUndoPageInfo> TUndoDBPages;

	public:
		CTranRedoPageManager(IDBTransactions *pTran, CTranStorage *pStorage);
		~CTranRedoPageManager();

		void setFirstPage(int64 nPage)
		{
			m_undoPages.setRoot(nPage);
		}
		bool add(const sUndoPageInfo& PageInfo);
		bool redo(CTranStorage *pTranStorage, IDBStorage* pDBStorage);
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
				return 2 * sizeof(int64) + sizeof(uint32);
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