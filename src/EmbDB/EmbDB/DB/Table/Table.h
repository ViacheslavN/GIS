#ifndef _EMBEDDED_DATABASE_TABLE_H_
#define _EMBEDDED_DATABASE_TABLE_H_

#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "../Fields/ValueField.h"
#include "DBFieldInfo.h"
#include <vector>
#include <map>
#include "CommonLibrary/FixedMemoryStream.h"
#include "utils/PageVector.h"
#include "../Indexs/UniqueIndex/Index.h"

#include "Counter.h"
namespace embDB
{
	class CDatabase;
	class CStorage;
 

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

		static const uint32 nTableHeaderPageSize  = MIN_PAGE_SIZE;
		static const uint32 nTableFieldsPageSize = MIN_PAGE_SIZE;
		static const uint32 nTableIndexPageSize = MIN_PAGE_SIZE;
		static const uint32 nFieldInfoPageSize = MIN_PAGE_SIZE;
		static const uint32 nStatisticPageSize = MIN_PAGE_SIZE;

		static const uint32 nMaxFieldNameLen = 128;
		static const uint32 nMaxFieldAliasLen = 128;

		public:
			CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::CString& sTableName);
			CTable(CDatabase* pDB, int64 m_nPageAddr,  const CommonLib::CString& sTableName);
			CTable(CDatabase* pDB, int64 m_nPageAddr);
			~CTable();

	 


			virtual bool getOIDFieldName(CommonLib::CString& sOIDName);
			virtual bool setOIDFieldName(const CommonLib::CString& sOIDName) ;
			virtual const CommonLib::CString& getName() const ;
			
			virtual IFieldPtr createField(const SFieldProp& sFP, ITransaction *pTran);
			virtual IFieldPtr createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias, CommonLib::eShapeType shapeType, 
					const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, ITransaction *pTran, bool bCreateIndex = true, uint32 nPageSize = PAGE_SIZE_8K);
			virtual bool createIndex(const CommonLib::CString& sFieldName, SIndexProp& ip, ITransaction *pTran);
			virtual bool createStatistic(const CommonLib::CString& sFieldName, const SStatisticInfo& si, ITransaction *pTran);
			virtual bool UpdateStatistic(const CommonLib::CString& sName, ITransaction *pTran);

			

			virtual bool deleteField(IField* pField);
	 
			virtual bool createCompositeIndex(std::vector<CommonLib::CString>& vecFields, SIndexProp& ip);
		
			
			virtual IFieldPtr getField(const CommonLib::CString& sName) const ;
			virtual uint32 getFieldCnt() const;
			virtual IFieldPtr getField(uint32 nIdx) const;
			virtual IFieldsPtr getFields() const {return m_pFields;}
			

		
	 
			bool addField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			 
			bool load();
			int64 getAddr();
			//int64 getID() const {return m_nTableID;}
			bool save(IDBTransaction *pTran);
			//IDBFieldHolderPtr getFieldHolder(const CommonLib::CString& name);

			bool delField(IField *pField, IDBTransaction *Tran = NULL);
			bool delField(const CommonLib::CString& sFieldName, IDBTransaction *Tran = NULL);
			//bool delField(int64 nID, IDBTransaction *Tran = NULL);
		


			virtual bool isLockWrite(){return true;}
			virtual bool lockWrite(){return true;}
			virtual bool try_lockWrite(){return true;}
			virtual bool unlockWrite(){return true;}

			virtual bool isLock(){return true;}
			virtual bool lock();
			virtual bool try_lock(){return true;}
			virtual bool unlock(){return true;}
			bool isCanBeRemoving();

			virtual int64 GetNextOID();
			virtual bool commit(IDBTransaction *pTran);
		private:
			bool ReadField(int64 nAddr);
			bool ReadIndex(int64 nAddr);
			bool ReadStatistic(int64 nAddr);
			bool readHeader(CommonLib::FxMemoryReadStream& stream);
			bool createValueField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
	


			IDBIndexHolderPtr createIndexHolder(IDBFieldHolder *pField,  const SIndexProp& ip, int64 nPageAddr);
			IDBIndexHolderPtr createSpatialIndexField(IDBShapeFieldHolder* pField, int64 nPageAddr, const SIndexProp& ip);
	
			//bool loadTableStorage(int64 nAddr);
			bool ReadIndices(int64 nAddr, IDBTransaction *pTran);
			bool BuildIndex(IDBIndexHolder* pIndexHolder, IDBFieldHolder *pFieldHolder, IDBTransaction* pTran);
			eSpatialType GetSpatialType(uint64 nMaxVal, bool isPoint);
			
			bool BuildStatistic(IFieldStatisticHolder* pStatisticHolder, IDBFieldHolder *pFieldHolder, IDBTransaction* pTran);

			
	    private:
			typedef std::map<CommonLib::CString, IDBFieldHolderPtr> TFieldByName;
			typedef std::map<int64, IDBFieldHolderPtr> TFieldByID;

			typedef std::map<CommonLib::CString, IDBIndexHolderPtr> TIndexByName;
			typedef std::map<int64, IDBIndexHolderPtr> TIndexByID;

			typedef std::map<CommonLib::CString, IFieldStatisticHolderPtr> FieldStatisticByName;

			typedef std::vector<int64> TFieldPages;

			bool LoadAddrs(TFieldPages& vecAddrs, int64 nBeginPage, IFilePage *pTran);
			bool SaveAddrs(const TFieldPages& vecAddrs, int64 nBeginPage, IFilePage *pTran, bool bNew);
		private:
  

			TIndexByName m_IndexByName;
			FieldStatisticByName m_FieldStatisticByName;


			int64 m_nTablePage;
			int64 m_nFieldsPage;
			int64 m_nIndexsPage;
			int64 m_nStatisticsPage;

			IDBStoragePtr m_pDBStorage;
			CommonLib::CString m_sTableName;
			CDatabase* m_pDB;
			TFieldPages m_vecFieldsAddr;
			TFieldPages m_vecIndexAddr;
			TFieldPages m_vecStatisticsAddr;

			typedef TCounter<int64> TOIDCounter;
			TOIDCounter m_OIDCounter;
			IFieldsPtr m_pFields;
			//IFieldsPtr m_pIndexs;

	};


}

#endif