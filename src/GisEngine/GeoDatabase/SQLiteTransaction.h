#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_TRANSACTION_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_TRANSACTION_H_

#include "GeoDatabase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{
			class CSQLiteDB;
		}

		class CSQLiteTransaction : public ITransaction
		{
		public:
			CSQLiteTransaction(SQLiteUtils::CSQLiteDB *pDB);
			~CSQLiteTransaction();


			bool begin();

			virtual bool commit();
			virtual bool rolback();
			virtual void GetError(CommonLib::CString& sText);

			virtual IInsertCursorPtr CreateInsertCusor(ITable *pTable, IFieldSet *pFileds = 0);
			virtual IUpdateCursorPtr CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds = 0);
			
			virtual IDeleteCursorPtr CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds = 0);

		private:
			SQLiteUtils::CSQLiteDB *m_pDB;
			bool m_bCommit;
			bool m_bEnd;
			bool m_bBegin;
		};
	}
}

#endif