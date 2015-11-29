#ifndef _EMBEDDED_DATABASE_TABLE_H_
#define _EMBEDDED_DATABASE_TABLE_H_

#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "Key.h"
#include "BaseBPSetv2.h"
//#include "VariantField.h"
#include "ValueField.h"
#include "DBFieldInfo.h"
#include <vector>
#include <map>
#include "CommonLibrary/FixedMemoryStream.h"
#include "PageVector.h"
#include "Index.h"
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
			
			virtual IFieldPtr createField(const SFieldProp& sFP, ITransaction *pTran = NULL);
			virtual IFieldPtr createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias, CommonLib::eShapeType shapeType, 
					const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, bool bCreateIndex = true, uint32 nPageSize = 8192, ITransaction *pTran = NULL);
			virtual bool createIndex(const CommonLib::CString& sFieldName, SIndexProp& ip, ITransaction *pTran = NULL);
			
			
			virtual bool deleteField(IField* pField);
	 
			virtual bool createCompositeIndex(std::vector<CommonLib::CString>& vecFields, SIndexProp& ip);
		
			
			virtual IFieldPtr getField(const CommonLib::CString& sName) const ;
			virtual size_t getFieldCnt() const;
			virtual IFieldPtr getField(size_t nIdx) const;
			virtual IFieldsPtr getFields() const {return m_pFields;}
			

		
	 
			bool addField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			 
			bool load();
			int64 getAddr();
			//int64 getID() const {return m_nTableID;}
			bool save(IDBTransaction *pTran);
			//IDBFieldHandlerPtr getFieldHandler(const CommonLib::CString& name);

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
			bool readHeader(CommonLib::FxMemoryReadStream& stream);
			bool createValueField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			
			IDBIndexHandlerPtr createIndexHandler(IDBFieldHandler *pField,  SIndexProp* ip, int64 nPageAddr);
			IDBIndexHandlerPtr createSpatialIndexField(IDBShapeFieldHandler* pField, int64 nPageAddr, uint32 nPageNodeSize);
	
			//bool loadTableStorage(int64 nAddr);
			bool ReadIndices(int64 nAddr, IDBTransaction *pTran);
			bool BuildIndex(IDBIndexHandler* pIndexHandler, IDBFieldHandler *pFieldHandler, IDBTransaction* pTran);
			eSpatialType GetSpatialType(uint64 nMaxVal, bool isPoint);
			

			
	    private:
			typedef std::map<CommonLib::CString, IDBFieldHandlerPtr> TFieldByName;
			typedef std::map<int64, IDBFieldHandlerPtr> TFieldByID;

			typedef std::map<CommonLib::CString, IDBIndexHandlerPtr> TIndexByName;
			typedef std::map<int64, IDBIndexHandlerPtr> TIndexByID;


			typedef TPageVector<int64> TFieldPages;
 
			//TFieldByName m_FieldByName;
			//TFieldByID m_FieldByID;

			TIndexByName m_IndexByName;
			//TIndexByID m_IndexByID;


			int64 m_nTablePage;
			int64 m_nFieldsPage;
			int64 m_nIndexsPage;
			IDBStoragePtr m_pDBStorage;
			CommonLib::CString m_sTableName;
			CDatabase* m_pDB;
			TFieldPages m_nFieldsAddr;
			TFieldPages m_nIndexAddr;
			typedef TCounter<int64> TOIDCounter;
			TOIDCounter m_OIDCounter;
			IFieldsPtr m_pFields;
			//IFieldsPtr m_pIndexs;

	};


}

#endif