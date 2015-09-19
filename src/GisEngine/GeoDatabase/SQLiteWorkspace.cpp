#include "stdafx.h"
#include "SQLiteWorkspace.h"
extern "C" {
#include "sqlite3/sqlite3.h"
}
#include "CommonLibrary/File.h"
#include "SQLiteUtils.h"
#include "Fields.h"
#include "Field.h"
#include "GeoDatabaseUtils.h"
#include "SQLiteTable.h"
#include "SQLiteFeatureClass.h"
#include "SQLiteDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
 
		const CommonLib::CString CSQLiteWorkspace::m_sRTreePrefix = L"SPatialRTree";
		const CommonLib::CString CSQLiteWorkspace::m_sProjPrefix = L"PROJ";
		CSQLiteWorkspace::CSQLiteWorkspace() : TBase (wiSqlLite)
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
			m_sPath(pszPath)
		{
			m_sName = pszName;
		}
		bool CSQLiteWorkspace::IsConnect() const
		{
			return m_pDB.get() ? m_pDB->IsConnect() : false;
		}
		bool CSQLiteWorkspace::IsError() const
		{
			return m_pDB.get() ? m_pDB->IsError() : false;
		}
		uint32 CSQLiteWorkspace::GetErrorCode() const {return 0;}
		void CSQLiteWorkspace::GetErrorText( CommonLib::CString& sStr, uint32 nCode) 
		{
			if(m_pDB.get())
				sStr = m_pDB->GetError();
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

			if(!IsConnect())
				return ITablePtr();

			m_pDB->ClearError();
			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"empty table name");
				return ITablePtr();
			}

			ITablePtr pTable = GetTable(sName);
			if(pTable.get())
			{
				m_pDB->SetErrorText(L"table " + sName + L" is exist");
				return  ITablePtr();
			}

			CommonLib::CString sSQL;
			CommonLib::CString sOidField = sOIDName;
			SQLiteUtils::CreateSQLCreateTable(pFields, sName, sSQL, sOidField.isEmpty() ? &sOidField : NULL);
			

			 if(!m_pDB->execute(sSQL))
				 return ITablePtr();
			

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
			const CommonLib::CString& sAnnotationName)
		{
	
			if(!IsConnect())
				return IFeatureClassPtr();
			
			m_pDB->ClearError();

			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"empty FeatureClass name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
			{
				m_pDB->SetErrorText("FeatureClass " + sName + L" is exist");
				return  IFeatureClassPtr();
			}

			CommonLib::CString sSQL;
			CommonLib::CString sOidField = sOIDName;
			CommonLib::CString sShapeField = shapeFieldName;
			CommonLib::CString sAnno = sAnnotationName;
			CommonLib::eShapeType gtype = CommonLib::shape_type_null;
			GisGeometry::ISpatialReferencePtr pSPref;
			SQLiteUtils::CreateSQLCreateTable(pFields, sName, sSQL,
				sOidField.isEmpty() ? &sOidField : NULL,
				sShapeField.isEmpty() ? &sShapeField : NULL,
				sAnno.isEmpty() ? &sAnno : NULL, 
				 &gtype, &pSPref);
		 

			if(!m_pDB->execute(sSQL))
				return  IFeatureClassPtr();
			

			CommonLib::CString sRTreeSQL;
			
			sRTreeSQL.format(L"CREATE VIRTUAL TABLE %s_%s USING rtree(feature_id, minX, maxX, minY, maxY)", 
				sName.cwstr(), m_sRTreePrefix.cwstr());
		 
			if(!m_pDB->execute(sRTreeSQL))
				return  IFeatureClassPtr();
			
			if(pSPref.get())
			{
				CommonLib::CString sSPRefSQL;
				sSPRefSQL.format(L"CREATE TABLE %s_PROJ (PROJ TEXT)", sName.cwstr());
				if(!m_pDB->execute(sSPRefSQL))
					return  IFeatureClassPtr();
				 
				sSPRefSQL.format(L"INSERT INTO %s_PROJ (PROJ) VALUES('%s')", sName.cwstr(), pSPref->GetProjectionString().cwstr());
				if(!m_pDB->execute(sSPRefSQL))
					return  IFeatureClassPtr();
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

			SQLiteUtils::TSQLiteResultSetPtr pRS = m_pDB->prepare_query(sQuery);
			if (pRS.get())
			{
				pFields = new CFields();
				while(pRS->StepNext())
				{
					CommonLib::CString sName = pRS->ColumnText(1);
					CommonLib::CString sType = pRS->ColumnText(2);
					CommonLib::CString sNotnull = pRS->ColumnText(3);
					CommonLib::CString sDefValue = pRS->ColumnText(4);
					CommonLib::CString sPK = pRS->ColumnText(5);

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
			}

			return pFields;
		}

		ITablePtr CSQLiteWorkspace::OpenTable(const CommonLib::CString& sName)
		{
			
			if(!IsConnect())
			{
				return ITablePtr();
			}
			m_pDB->ClearError();

			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"Empty table Name");
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
	
			if(!IsConnect())
			{
				return IFeatureClassPtr();
			}
			m_pDB->ClearError();
			if(sName.isEmpty())
			{
				m_pDB->SetErrorText(L"Empty table Name");
				return IFeatureClassPtr();
			}

			IFeatureClassPtr pFC = GetFeatureClass(sName);
			if(pFC.get())
				return pFC;


			IFieldsPtr pFields = ReadFields(sName);
			if(!pFields.get())
				return pFC;

			if(pFields->GetFieldCount() == 0)
				return pFC;



			CSQLiteFeatureClass *pSQLiteFC = new CSQLiteFeatureClass(this, sName, sName, sName+ L"_" + m_sRTreePrefix);
			pSQLiteFC->open();

			pFC = (IFeatureClass*)pSQLiteFC;
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
			m_pDB.reset(new SQLiteUtils::CSQLiteDB(sFullName, true));
			return !m_pDB->IsError();
		}
		bool CSQLiteWorkspace::load(const CommonLib::CString& sFullName, bool bWrite, bool bOpenAll)
		{

			 m_pDB.reset(new SQLiteUtils::CSQLiteDB(sFullName, false));
			 if(m_pDB->IsError())
			 {
				 return false;
			 }

			 if(!bOpenAll)
				 return true;


			 CommonLib::CString sSQL = L"SELECT * FROM sqlite_master WHERE type='table' ORDER BY name";

			 SQLiteUtils::TSQLiteResultSetPtr pRS = m_pDB->prepare_query(sSQL);
			 if(!pRS.get())
			 {
				return false;
			 }

			 
			 std::vector<CommonLib::CString> tableNames;
			 while(pRS->StepNext())
			 {
				CommonLib::CString tableName = pRS->ColumnText(1);
				CommonLib::CString sCreateSQL = pRS->ColumnText(4);

				if(sCreateSQL.find(m_sRTreePrefix) == -1)
					 tableNames.push_back(tableName);
			 }
		 


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
			if(m_pDB.get())
			{
				m_pDB.reset();
			}
		}
		ITransactionPtr CSQLiteWorkspace::startTransaction()
		{
			return ITransactionPtr();
		}
	}
}