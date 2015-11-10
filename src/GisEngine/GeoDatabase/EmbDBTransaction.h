#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_TRANSACTION_H_
#define GIS_ENGINE_GEO_DATABASE_EMBDB_TRANSACTION_H_

#include "GeoDatabase.h"
#include "../../EmbDB/EmbDB/embDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{

		class CEmbDBTransaction : public ITransaction
		{
		public:
			CEmbDBTransaction(embDB::IDatabase* pDB);
			~CEmbDBTransaction();


			bool begin();

			virtual bool commit();
			virtual bool rolback();
			virtual void GetError(CommonLib::CString& sText);

			virtual IInsertCursorPtr CreateInsertCusor(ITable *pTable, IFieldSet *pFileds = 0);
			virtual IUpdateCursorPtr CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds = 0);

			virtual IDeleteCursorPtr CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds = 0);

		private:
			embDB::IDatabasePtr m_pDB;
			embDB::ITransactionPtr m_pTran;
			bool m_bCommit;
			bool m_bEnd;
			bool m_bBegin;
		};
	}
}

#endif