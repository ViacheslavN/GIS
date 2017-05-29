#ifndef _EMBEDDED_DATABASE_C_DIRECT_TRAN_UNDO_MANAGER_H_
#define _EMBEDDED_DATABASE_C_DIRECT_TRAN_UNDO_MANAGER_H_

#include "CommonLibrary/File.h"
#include "FilePage.h"
#include "utils/PageVectorLazySave.h"

namespace embDB
{
	class CTranStorage;

	class CDirectTranUndoManager
	{
		public:
			CDirectTranUndoManager();
			~CDirectTranUndoManager();

			void init(int64 nRootPage, CTranStorage *pStorage);
			void load(int64 nRootPage, CTranStorage *pStorage);

			void AddUndoPage(CFilePage *pPage);

		private:
			CTranStorage *m_pStorage;
			int64 m_nRootPage;
			typedef std::map<int64, int64> TPages;

			TPages m_pages;
			
			struct sUndoPageInfo
			{
				sUndoPageInfo() : nTranAddr(-1), nDBAddr(-1)
				{

				}

				int64 nTranAddr;
				int64 nDBAddr;
			};



			class TReaderWriter
			{
			public:
				void write(const sUndoPageInfo& value, CommonLib::FxMemoryWriteStream& stream)
				{
					stream.write(value.nDBAddr);
					stream.write(value.nTranAddr);
				}
				void read(sUndoPageInfo& value, CommonLib::FxMemoryReadStream& stream)
				{
					stream.read(value.nDBAddr);
					stream.read(value.nTranAddr);
				}

				uint32 rowSize()
				{
					return 2 * sizeof(int64) + sizeof(uint32);
				}
			};

			typedef TPageVectorLazySave<sUndoPageInfo, TReaderWriter> TUndoPageList;


	};


}

#endif