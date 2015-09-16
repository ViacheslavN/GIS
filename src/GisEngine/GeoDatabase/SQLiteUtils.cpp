#include "stdafx.h"
#include "SQLiteUtils.h"
#include "sqlite3/sqlite3.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{
			void FieldType2SQLiteType(eDataTypes type, CommonLib::CString& sSQLiteType)
			{
				switch(type)
				{
					case dtInteger8:
					case dtInteger16:
					case dtInteger32:
					case dtInteger64:
					case dtUInteger8:
					case dtUInteger16:
					case dtUInteger32:
					case dtUInteger64:
					case dtOid:
						sSQLiteType = L"INTEGER";
						break;
					case dtFloat:
					case dtDouble:
						sSQLiteType = L"REAL";
						break;
					case  dtBlob:
					case  dtGeometry:
						sSQLiteType = L"BLOB";
						break;
					case  dtString:
						sSQLiteType = L"TEXT";
						break;
				}
			}

			eDataTypes  SQLiteType2FieldType(CommonLib::CString& sSQLiteType)
			{
				if(sSQLiteType == L"INTEGER")
					return dtInteger64;
				if(sSQLiteType == L"REAL")
					return dtDouble;
				if(sSQLiteType == L"BLOB")
					return dtBlob;
				if(sSQLiteType == L"TEXT")
					return dtString;

				return dtUnknown;
			}
			eDataTypes SQLiteType2FieldType(int nSQLiteFieldType)
			{
				switch(nSQLiteFieldType)
				{
				case SQLITE_INTEGER: 
					return dtInteger64;
					break;
				case SQLITE_FLOAT: 
					return dtDouble;
					break;
				case SQLITE_BLOB: 
					return dtBlob;
					break;
				case SQLITE_TEXT: 
					return dtString;
					break;
				default:
					return dtUnknown;
					break;
				}
			}

			void CreateSQLCreateTable(IFields *pFields, const CommonLib::CString& sTableName, 
				CommonLib::CString& sSQL, CommonLib::CString* pOIDField, CommonLib::CString* pShapeField, 
				CommonLib::CString* pAnno, CommonLib::eShapeType *pSPType)
			{

				if(pOIDField)
				 *pOIDField = L"rowid";
				std::vector<CommonLib::CString> vecPrimaryKey;
				sSQL = L"CREATE TABLE "; 
				sSQL += sTableName;

				int nCnt = pFields->GetFieldCount();
				if(nCnt > 0)
					sSQL += L" ( ";

				CommonLib::CString sType;

				for (size_t i = 0; i < nCnt; ++i)
				{
					IFieldPtr pField = pFields->GetField(i);
					const CommonLib::CVariant& defVal = pField->GetDefaultValue();

					SQLiteUtils::FieldType2SQLiteType(pField->GetType(), sType);
					if(pField->GetType() == dtOid && pOIDField)
						*pOIDField = pField->GetName();
					else if(pField->GetType() == dtGeometry )
					{
						if(pShapeField)
							*pShapeField = pField->GetName();
						if(pSPType)
						{
							IShapeField *pShapeField = (IShapeField*)pField.get();
							if(pShapeField)
							{
								*pSPType = pShapeField->GetGeometryDef()->GetGeometryType();
							}
						}
					}
					else if(pField->GetType() == dtAnnotation && pAnno)
					{
						*pAnno = pField->GetName();
						if(pSPType)
						{
							IShapeField *pShapeField = (IShapeField*)pField.get();
							if(pShapeField)
							{
								*pSPType = pShapeField->GetGeometryDef()->GetGeometryType();
							}
						}
					}

					if(pField->GetIsPrimaryKey())
					{
						vecPrimaryKey.push_back(pField->GetName());
					}

					if(i != 0)
						sSQL += L", ";

					sSQL += pField->GetName() + L" ";

					if(!defVal.IsNull())
					{
						CommonLib::ToStringVisitor vis;
						sType += L" DEFAULT '" + CommonLib::apply_visitor<CommonLib::ToStringVisitor>(defVal, vis) + "' ";
					}


					if(!pField->GetIsNullable())
						sType += L" NOT NULL ";
					sSQL += sType;
				}



				if(!vecPrimaryKey.empty())
				{
					sSQL += L", PRIMARY KEY( ";

					for (size_t i = 0, sz = vecPrimaryKey.size(); i < sz; ++i)
					{
						if(i != 0)
							sSQL += L", ";

						sSQL += vecPrimaryKey[i];
					}

					sSQL += L")";
				}
				sSQL += L");";

			}
		}
	}
}