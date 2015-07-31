#ifndef _EMBEDDED_DATABASE_TABLE_H_
#define _EMBEDDED_DATABASE_TABLE_H_

#include "CommonLibrary/str_t.h"
#include "Key.h"
#include "BaseBTree.h"
#include "VariantField.h"
#include "ValueField.h"
#include "DBFieldInfo.h"
#include <vector>
#include <map>
#include "CommonLibrary/FixedMemoryStream.h"
#include "IDBTransactions.h"
#include "PageVector.h"
#include "IDBTable.h"
namespace embDB
{
	class CDatabase;
	class CStorage;
	class IDBIndexHandler;

	class CTable : public IDBTable
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




			virtual bool getOIDFieldName(CommonLib::str_t& sOIDName);
			virtual bool setOIDFieldName(const CommonLib::str_t& sOIDName) ;
			virtual const CommonLib::str_t& getName() const ;
			virtual IField* getField(const CommonLib::str_t& sName) const ;
			virtual size_t getFieldCnt() const;
			virtual IField* getField(size_t nIdx) const;
			virtual IField* createField(SFieldProp& sFP);
			virtual bool deleteField(IField* pField);
			virtual bool createIndex(const CommonLib::str_t& , SIndexProp& ip);
			virtual bool createCompositeIndex(std::vector<CommonLib::str_t>& vecFields, SIndexProp& ip);


			bool addIndex(const CommonLib::str_t& , SIndexProp& ip, bool bNew);
			bool addIndex(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);
			bool addField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);
			bool addSpatialField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);
			bool load();
			int64 getAddr();
			//int64 getID() const {return m_nTableID;}
			bool save(IDBTransactions *pTran);
			IDBFieldHandler* getFieldHandler(const CommonLib::str_t& name);

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
			bool ReadField(int64 nAddr, IDBTransactions *pTran);
			bool ReadIndex(int64 nAddr, IDBTransactions *pTran);
			bool readHeader(CommonLib::FxMemoryReadStream& stream);
			bool createValueField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);
			bool createSpatialIndexField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);
			bool createIndexField(sFieldInfo& fi, IDBTransactions *pTran, bool bNew);

	
			bool loadTableStorage(int64 nAddr);
			bool ReadIndices(int64 nAddr, IDBTransactions *pTran);
			bool BuildIndex(IDBIndexHandler* pIndexHandler, IDBFieldHandler *pFieldHandler, IDBTransactions* pTran);
			
	    private:
			typedef std::map<CommonLib::str_t, IDBFieldHandler*> TFieldByName;
			typedef std::map<int64, IDBFieldHandler*> TFieldByID;

			typedef std::map<CommonLib::str_t, IDBIndexHandler*> TIndexByName;
			typedef std::map<int64, IDBIndexHandler*> TIndexByID;


			typedef TPageVector<int64> TFieldPages;
 
			TFieldByName m_FieldByName;
			TFieldByID m_FieldByID;

			TIndexByName m_IndexByName;
			TIndexByID m_IndexByID;


			int64 m_nTablePage;
			int64 m_nStoragePageID;
			int64 m_nFieldsPage;
			int64 m_nIndexsPage;
			int64 m_nRowIDPage;
			CStorage* m_pMainDBStorage;
			CommonLib::str_t m_sTableName;
			CDatabase* m_pDB;
			CStorage* m_pTableStorage;
			TFieldPages m_nFieldsAddr;
			TFieldPages m_nIndexAddr;
			//int64 m_nTableID;

	};


}

#endif