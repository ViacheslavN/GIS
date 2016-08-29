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
			CEmbDBTransaction(embDB::IConnection* pConnection, eTransactionType type);
			~CEmbDBTransaction();


			bool begin();

			virtual bool commit();
			virtual bool rollback();
			virtual void GetError(CommonLib::CString& sText);

			virtual IInsertCursorPtr CreateInsertCusor(ITable *pTable, IFieldSet *pFileds = 0);
			virtual IUpdateCursorPtr CreateUpdateCusor(ITable *pTable, IFieldSet *pFileds = 0);

			virtual IDeleteCursorPtr CreateDeleteCusor(ITable *pTable, IFieldSet *pFileds = 0);

		private:
			embDB::IConnectionPtr m_pConnection;
			embDB::ITransactionPtr m_pTran;
			bool m_bCommit;
			bool m_bEnd;
			bool m_bBegin;
		};
	}
}

#endif