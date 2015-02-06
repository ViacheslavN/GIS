#ifndef _EMBEDDED_DATABASE_TABLE_H_
#define _EMBEDDED_DATABASE_TABLE_H_

#include "CommonLibrary/str_t.h"
#include "Key.h"
#include "BaseBTree.h"
#include "VariantField.h"
#include "OIDField.h"
#include "DBField.h"
#include <vector>
#include <map>
#include "CommonLibrary/FixedMemoryStream.h"
#include "IDBTransactions.h"
#include "PageVector.h"
namespace embDB
{
	class CDatabase;
	class CStorage;

	class CTable
	{

		enum eSectionTYpe
		{
			HEADER_SECTION = 1,
			FIELD_SECTION = 2
			
		};
		enum eFieldType
		{
			OIDFIELD_TYPE = 1,
			INDEXFIELD_TYPE =2

		};
		public:
			CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::str_t& sTableName, CStorage* pTableStorage/*, int64 nTableID*/);
			CTable(CDatabase* pDB, int64 m_nPageAddr,  const CommonLib::str_t& sTableName, CStorage* pTableStorage);
			CTable(CDatabase* pDB, int64 m_nPageAddr, CStorage* pTableStorage);
			~CTable();

			bool addField(sFieldInfo& fi, IDBTransactions *pTran);
			bool addSpatialField(sFieldInfo& fi, IDBTransactions *pTran);
			bool load();
			const CommonLib::str_t& getName() const;
			int64 getAddr();
			//int64 getID() const {return m_nTableID;}
			bool save(IDBTransactions *pTran);
			IDBFieldHandler* getField(const CommonLib::str_t& name);

			bool delField(IDBFieldHandler *pField, IDBTransactions *Tran = NULL);
			bool delField(const CommonLib::str_t& sFieldName, IDBTransactions *Tran = NULL);
			bool delField(int64 nID, IDBTransactions *Tran = NULL);
		



			virtual bool isLockWrite(){return true;}
			virtual bool lockWrite(){return true;}
			virtual bool try_lockWrite(){return true;}
			virtual bool unlockWrite(){return true;}

			virtual bool isLock(){return true;}
			virtual bool lock();
			virtual bool try_lock(){return true;}
			virtual bool unlock(){return true;}
			bool isCanBeRemoving();
			
			bool commit();

		private:
			bool CreateField(sFieldInfo& fi, IDBTransactions *pTran);
			bool readFields(CommonLib::FxMemoryReadStream& stream, IDBTransactions *pTran = NULL);
			bool readHeader(CommonLib::FxMemoryReadStream& stream);
			bool createValueField(sFieldInfo& fi, IDBTransactions *pTran);
			bool createSpatialIndexField(sFieldInfo& fi, IDBTransactions *pTran);
			bool createIndexField(sFieldInfo& fi, IDBTransactions *pTran);
			bool ReadField(int64 nAddr, IDBTransactions *pTran);
			bool saveFields(IDBTransactions *pTran);
			bool loadTableStorage(int64 nAddr);

			
	    private:
			typedef RBMap<CommonLib::str_t, IDBFieldHandler*> TFieldByName;
			typedef RBMap<int64, IDBFieldHandler*> TFieldByID;
			typedef TPageVector<int64> TFieldPages;
 
			TFieldByName m_OIDFieldByName;
			TFieldByID m_OIDFieldByID;

			int64 m_nSchemaPageAddr;
			int64 m_nTablePage;
			int64 m_nStoragePageID;
			int64 m_nFieldsPage;
			int64 m_nLastRecOID;
			CStorage* m_pMainDBStorage;
			CommonLib::str_t m_sTableName;
			CDatabase* m_pDB;
			CStorage* m_pTableStorage;
			TFieldPages m_nFieldsAddr;
			//int64 m_nTableID;

	};


}

#endif