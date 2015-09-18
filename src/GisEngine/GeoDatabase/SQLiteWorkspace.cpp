#include "stdafx.h"
#include "SQLiteWorkspace.h"
#include "sqlite3/sqlite3.h"
#include "CommonLibrary/File.h"
#include "SQLiteUtils.h"
#include "Fields.h"
#include "Field.h"
#include "GeoDatabaseUtils.h"
#include "SQLiteTable.h"
#include "SQLiteFeatureClass.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
 
		const CommonLib::CString CSQLiteWorkspace::m_sRTreePrefix = L"SPatialRTree";
		CSQLiteWorkspace::CSQLiteWorkspace() : TBase (wiSqlLite), m_pConn(0), m_precordSet(0), m_bError(false)
		{
			m_WorkspaceType = wiSqlLite;
		//	sqlite3_initialize();
		}
		CSQLiteWorkspace::~CSQLiteWorkspace()
		{
			close();
		//	sqlite3_shutdown();
		}
		CSQLiteWorkspace::CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath) : TBase (wiSqlLite),
			m_sPath(pszPath), m_pConn(0), m_precordSet(0), m_bError(false)
		{
			m_sName = pszName;
		}
		bool CSQLiteWorkspace::IsConnect() const
		{
			return m_pConn != NULL;
		}
		IWorkspacePtr CSQLiteWorkspace::Create(const wchar_t *pszName, const wchar_t *pszPath)
		{
			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != _T('/') || sFullName[sFullName.length() - 1] != _T('\\'))
				{
					sFullName += _T('\\');
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wiSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return IWorkspacePtr();
			}

			if(CommonLib::FileSystem::isFileExisit(sFullName.cwstr()))
			{
				//TO DO Error
				return IWorkspacePtr();
			}

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath);
			if(!pSQLiteWks->create(sFullName))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}
			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks, pszPath);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}

		IWorkspacePtr CSQLiteWorkspace::Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite, bool bOpenAll)
		{

			CommonLib::CString sFullName = pszPath;

			if(!sFullName.isEmpty())
			{
				if(sFullName[sFullName.length() - 1] != _T('/') || sFullName[sFullName.length() - 1] != _T('\\'))
				{
					sFullName += _T('\\');
				}
			}

			sFullName += pszName;
			if(sFullName.isEmpty())
			{
				//TO DO Error
				return IWorkspacePtr();
			}
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wiSqlLite, sFullName);
			if(pWks.get())
			{
				//TO DO Error
				return pWks;
			}

			CSQLiteWorkspace *pSQLiteWks = new  CSQLiteWorkspace(pszName, pszPath);
			if(!pSQLiteWks->load(sFullName, bWrite, bOpenAll))
			{
				//TO DO Error
				delete pSQLiteWks;
				return IWorkspacePtr();
			}

			CWorkspaceHolder::AddWorkspace((IWorkspace*)pSQLiteWks, pszPath);
			return IWorkspacePtr((IWorkspace*)pSQLiteWks);
		}
		IWorkspacePtr CSQLiteWorkspace::Open(CommonLib::IReadStream* pSteram, bool bOpenAll)
		{
			return IWorkspacePtr();
		}
		IWorkspacePtr CSQLiteWorkspace::Open(GisCommon::IXMLNode *pNode, bool bOpenAll)
		{
			return IWorkspacePtr();
		}

		
		ITablePtr  CSQLiteWorkspace::CreateTable(const CommonLib::CString& sName, IFields* pFields, const CommonLib::CString& sOIDName )
		{
			m_bError = false;
			if(!IsConnect())
				return ITablePtr();

			if(sName.isEmpty())
			{
				m_bError = true;
				m_sErrorMessage = L"empty table name";
				return ITablePtr();
			}

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
			{
				m_bError = true;
				m_sErrorMessage = L"table " + sName + L" is exist";
				return  ITablePtr();
			}

			CommonLib::CString sSQL;
			CommonLib::CString sOidField = sOIDName;
			SQLiteUtils::CreateSQLCreateTable(pFields, sName, sSQL, sOidField.isEmpty() ? &sOidField : NULL);
			


			 int retVal = sqlite3_exec(m_pConn, sSQL.cstr(), 0, 0, 0);

			 if(retVal != SQLITE_OK)
			 {
				 m_bError = true;
				 m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
				 return ITablePtr();
			 }

			pTable = OpenTable(sName);
			if(pTable.get())
			{
				pTable->SetHasOIDField(true);
				pTable->SetOIDFieldName(sOidField);
			}
			return pTable;
		}
		IFeatureClassPtr CSQLiteWorkspace::CreateFeatureClass(const CommonLib::CString& sName,
			IFields* pFields, const CommonLib::CString& sOIDName,  
			const CommonLib::CString& shapeFieldName,
			const CommonLib::CString& sAnnotationName,
			CommonLib::eShapeType geomtype)
		{
			m_bError = false;
			if(!IsConnect())
				return IFeatureClassPtr();

			if(sName.isEmpty())
			{
				m_bError = true;
				m_sErrorMessage = L"empty FeatureClass name";
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
			{
				m_bError = true;
				m_sErrorMessage = L"FeatureClass " + sName + L" is exist";
				return  IFeatureClassPtr();
			}

			CommonLib::CString sSQL;
			CommonLib::CString sOidField = sOIDName;
			CommonLib::CString sShapeField = shapeFieldName;
			CommonLib::CString sAnno = sAnnotationName;
			CommonLib::eShapeType gtype = geomtype;
			SQLiteUtils::CreateSQLCreateTable(pFields, sName, sSQL,
				sOidField.isEmpty() ? &sOidField : NULL,
				sShapeField.isEmpty() ? &sShapeField : NULL,
				sAnno.isEmpty() ? &sAnno : NULL, 
				gtype == CommonLib::shape_type_null ? &gtype : NULL);
		 


			int retVal = sqlite3_exec(m_pConn, sSQL.cstr(), 0, 0, 0);

			if(retVal != SQLITE_OK)
			{
				m_bError = true;
				m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
				return IFeatureClassPtr();
			}

			CommonLib::CString sRTreeSQL;
			
			sRTreeSQL.format(L"CREATE VIRTUAL TABLE %s_%s USING rtree(%s, minX, maxX, minY, maxY)", 
				sName.cwstr(), m_sRTreePrefix.cwstr(), sOidField.cwstr());
		 
	
			retVal = sqlite3_exec(m_pConn, sRTreeSQL.cstr(), NULL, 0, NULL);
			if(retVal != SQLITE_OK)
			{
				m_bError = true;
				m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
				return IFeatureClassPtr();
			}

			pFC =  OpenFeatureClass(sName);
		 
			if(pFC.get())
			{
				pFC->SetHasOIDField(true);
				pFC->SetOIDFieldName(sOidField);
				pFC->SetShapeFieldName(shapeFieldName.isEmpty() ? sShapeField : shapeFieldName);
				if(!shapeFieldName.isEmpty())
				{
					int nIDx = pFC->GetFields()->FindField(shapeFieldName);
					if(nIDx != -1)
					{
						IFieldPtr pShapeField = pFC->GetFields()->GetField(nIDx);
						if(pShapeField.get())
							pShapeField->SetType(dtGeometry);
					}

				}
				if(!sAnno.isEmpty())
				{
					pFC->SetIsAnnoClass(true);
					pFC->SetAnnoFieldName(sAnno);
					int nIDx = pFC->GetFields()->FindField(sAnno);
					if(nIDx != -1)
					{
						IFieldPtr pAnnoField = pFC->GetFields()->GetField(nIDx);
						if(pAnnoField.get())
							pAnnoField->SetType(dtAnnotation);
					}
				}
				pFC->SetGeometryType(gtype);
			
			}
			return pFC;
		}

		IFieldsPtr CSQLiteWorkspace::ReadFields(const CommonLib::CString& sName)
		{

			IFieldsPtr pFields;
			CommonLib::CString sQuery;
			sQuery.format(L"pragma table_info ('%s')", sName.cwstr());
			int rc = sqlite3_prepare_v2(m_pConn, sQuery.cstr(), -1, &m_precordSet, NULL);
 
			if (rc == SQLITE_OK)
			{
				pFields = new CFields();
				while(sqlite3_step(m_precordSet) == SQLITE_ROW)
				{
					CommonLib::CString sName = (char*)sqlite3_column_text(m_precordSet, 1);
					CommonLib::CString sType = (char*)sqlite3_column_text(m_precordSet, 2);
					CommonLib::CString sNotnull = (char*)sqlite3_column_text(m_precordSet, 3);
					CommonLib::CString sDefValue = (char*)sqlite3_column_text(m_precordSet, 4);
					CommonLib::CString sPK = (char*)sqlite3_column_text(m_precordSet, 5);

					eDataTypes type = SQLiteUtils::SQLiteType2FieldType(sType);
					IFieldPtr pField(new CField());
					pField->SetType(type);
					pField->SetName(sName);
					pField->SetIsNullable(sType != L"1");
					if(!sDefValue.isEmpty())
					{
						CommonLib::CVariant var = GeoDatabaseUtils::GetVariantFromString(type, sDefValue);
						pField->SetIsDefault(var);
					}
					pField->SetIsPrimaryKey(sPK == L"1");
					pFields->AddField(pField.get());
				}
				sqlite3_finalize(m_precordSet);
			}

			return pFields;
		}

		ITablePtr CSQLiteWorkspace::OpenTable(const CommonLib::CString& sName)
		{
			m_bError = false;
			if(!IsConnect())
			{
				m_bError = true;
				m_sErrorMessage = L"Not connections";
				return ITablePtr();
			}

			if(sName.isEmpty())
			{
				m_bError = true;
				m_sErrorMessage = L"Empty table Name";
				return ITablePtr();
			}

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
				return pTable;
		
		 
			IFieldsPtr pFields = ReadFields(sName);
			if(!pFields.get())
				return pTable;


			pTable = (ITable*)new  CSQLiteTable(this, sName, sName);
			pTable->SetFields(pFields.get());
			AddDataset(pTable.get());
	 
			return pTable;
		}
		IFeatureClassPtr CSQLiteWorkspace::OpenFeatureClass(const CommonLib::CString& sName)
		{
			m_bError = false;
			if(!IsConnect())
			{
				m_bError = true;
				m_sErrorMessage = L"Not connections";
				return IFeatureClassPtr();
			}

			if(sName.isEmpty())
			{
				m_bError = true;
				m_sErrorMessage = L"Empty table Name";
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
				return pFC;


			IFieldsPtr pFields = ReadFields(sName);
			if(!pFields.get())
				return pFC;


			pFC = (IFeatureClass*)new  CSQLiteFeatureClass(this, sName, sName, sName+ L"_" + m_sRTreePrefix);
			pFC->SetFields(pFields.get());
			AddDataset(pFC.get());

			return pFC;
		}

		
		bool CSQLiteWorkspace::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CSQLiteWorkspace::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}
		bool CSQLiteWorkspace::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CSQLiteWorkspace::load(GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
		bool  CSQLiteWorkspace::create(const CommonLib::CString& sFullName)
		{
			int retVal = sqlite3_open_v2(sFullName.cstr(), &m_pConn, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
			if(retVal != SQLITE_OK)
			{
				m_bError = true;
				m_sErrorMessage = L"Error create DB file " + sFullName;
				close();
				return false;
			}

			return true;
		}
		bool CSQLiteWorkspace::load(const CommonLib::CString& sFullName, bool bWrite, bool bOpenAll)
		{
			 int flags = bWrite ? SQLITE_OPEN_READWRITE:  SQLITE_OPEN_READONLY;

			 int retVal = sqlite3_open_v2(sFullName.cstr(), &m_pConn, flags, 0);
			 if(retVal != SQLITE_OK)
			 {
				 m_bError = true;
				 m_sErrorMessage = L"Error open DB file " + sFullName;
				 close();
				 return false;
			 }

			 if(!bOpenAll)
				 return true;


			 std::string sSQL = "SELECT * FROM sqlite_master WHERE type='table' ORDER BY name";
			 int retValue = sqlite3_prepare_v2(m_pConn, sSQL.c_str(), -1, &m_precordSet, 0);
			 if(retValue != SQLITE_OK)
			 {
			
				m_bError = true;
				 m_sErrorMessage = CommonLib::CString(sqlite3_errmsg(m_pConn));
				close();
				return false;
			 }

			 retValue = sqlite3_step(m_precordSet);
			 std::vector<CommonLib::CString> tableNames;
			 while(retValue == SQLITE_ROW)
			 {

		
				CommonLib::CString tableName = (char*)sqlite3_column_text(m_precordSet, 1);
				CommonLib::CString sCreateSQL = (char*)sqlite3_column_text(m_precordSet, 4);

				if(sCreateSQL.find(m_sRTreePrefix) == -1)
					 tableNames.push_back(tableName);


				 retValue = sqlite3_step(m_precordSet);
			 }
		 
			 sqlite3_finalize(m_precordSet);


			 for (size_t i = 0, sz = tableNames.size(); i < sz; ++i)
			 {
				 if(!OpenTable(tableNames[i]))
				 {
					 return false;
				 }
			 }

			 return true;
		}
		void CSQLiteWorkspace::close()
		{
			if(m_pConn != 0)
			{
				int result = sqlite3_close(m_pConn);
				if(result != SQLITE_OK)
				{
					//TO DO Error
				}
				m_pConn = 0;
			}
		}
		ITransactionPtr CSQLiteWorkspace::startTransaction()
		{
			return ITransactionPtr();
		}
	}
}