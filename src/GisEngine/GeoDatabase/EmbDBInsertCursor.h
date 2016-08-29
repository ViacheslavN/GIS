#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_INSERT_CURSOR_BASE_H
#define GIS_ENGINE_GEO_DATABASE_EMBDB_INSERT_CURSOR_BASE_H
#include "InsertCursorBase.h"
#include "CommonLibrary/MemoryStream.h"
#include "../../EmbDB/EmbDB/embDB.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		 
			class CEmbDBInsertCursor : public IInsertCursorBase<IInsertCursor>
			{
				public:
					typedef IInsertCursorBase<IInsertCursor> TBase;

					CEmbDBInsertCursor(ITable* pTable, IFieldSet *pFileds, embDB::IConnection *pConnection ,
						embDB::ITransaction* pTran);
					virtual ~CEmbDBInsertCursor();

					virtual int64 InsertRow(IRow* pRow);
				private:
					void init();
					void close();
				private:
					std::vector<eDataTypes> m_vecTypes;
					embDB::IConnectionPtr m_pConnection;
					embDB::IInsertCursorPtr m_pCursor;
					embDB::IRowPtr m_pDBRow;
					embDB::ITransactionPtr m_pTran;
					bool m_bValidCursor;
					bool m_bInit;
					CommonLib::CString m_sErrorMessage;
				
			};
	}
}

#endif