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
		public:
			CTable(CDatabase* pDB, CFilePage* pFilePage, const CommonLib::CString& sTableName, CStorage* pTableStorage/*, int64 nTableID*/);
			CTable(CDatabase* pDB, int64 m_nPageAddr,  const CommonLib::CString& sTableName, CStorage* pTableStorage);
			CTable(CDatabase* pDB, int64 m_nPageAddr, CStorage* pTableStorage);
			~CTable();

			virtual IFieldsPtr getFields() const {return IFieldsPtr(); }


			virtual bool getOIDFieldName(CommonLib::CString& sOIDName);
			virtual bool setOIDFieldName(const CommonLib::CString& sOIDName) ;
			virtual const CommonLib::CString& getName() const ;
			virtual IFieldPtr getField(const CommonLib::CString& sName) const ;
			virtual size_t getFieldCnt() const;
			virtual IFieldPtr getField(size_t nIdx) const;
			virtual IFieldPtr createField(SFieldProp& sFP);
			virtual bool deleteField(IField* pField);
			virtual bool createIndex(const CommonLib::CString& , SIndexProp& ip);
			virtual bool createCompositeIndex(std::vector<CommonLib::CString>& vecFields, SIndexProp& ip);
			virtual IFieldPtr createShapeField(const wchar_t *pszFieldName, const wchar_t* pszAlias, CommonLib::eShapeType shapeType, const CommonLib::bbox& extent, eSpatialCoordinatesUnits CoordUnits, bool bCreateIndex = true);



			bool addIndex(const CommonLib::CString& , SIndexProp& ip, bool bNew);
			bool addIndex(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			bool addField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			 
			bool load();
			int64 getAddr();
			//int64 getID() const {return m_nTableID;}
			bool save(IDBTransaction *pTran);
			IDBFieldHandlerPtr getFieldHandler(const CommonLib::CString& name);

			bool delField(IDBFieldHandler *pField, IDBTransaction *Tran = NULL);
			bool delField(const CommonLib::CString& sFieldName, IDBTransaction *Tran = NULL);
			bool delField(int64 nID, IDBTransaction *Tran = NULL);
		


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
			bool ReadField(int64 nAddr, IDBTransaction *pTran);
			bool ReadIndex(int64 nAddr, IDBTransaction *pTran);
			bool readHeader(CommonLib::FxMemoryReadStream& stream);
			bool createValueField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			bool createSpatialIndexField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);
			bool createIndexField(sFieldInfo* fi, IDBTransaction *pTran, bool bNew);

	
			bool loadTableStorage(int64 nAddr);
			bool ReadIndices(int64 nAddr, IDBTransaction *pTran);
			bool BuildIndex(IDBIndexHandler* pIndexHandler, IDBFieldHandler *pFieldHandler, IDBTransaction* pTran);
			eDataTypes GetType(uint64 nMaxVal, bool isPoint);
			

			
	    private:
			typedef std::map<CommonLib::CString, IDBFieldHandlerPtr> TFieldByName;
			typedef std::map<int64, IDBFieldHandlerPtr> TFieldByID;

			typedef std::map<CommonLib::CString, IDBIndexHandlerPtr> TIndexByName;
			typedef std::map<int64, IDBIndexHandlerPtr> TIndexByID;


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
			CommonLib::CString m_sTableName;
			CDatabase* m_pDB;
			CStorage* m_pTableStorage;
			TFieldPages m_nFieldsAddr;
			TFieldPages m_nIndexAddr;
			//int64 m_nTableID;

	};


}

#endif