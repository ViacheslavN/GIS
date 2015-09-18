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
					case dtOid32:
					case dtOid64:
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

				size_t nCnt = pFields->GetFieldCount();
				if(nCnt > 0)
					sSQL += L" ( ";

				CommonLib::CString sType;

				for (size_t i = 0; i < nCnt; ++i)
				{
					IFieldPtr pField = pFields->GetField(i);
					const CommonLib::CVariant& defVal = pField->GetDefaultValue();

					SQLiteUtils::FieldType2SQLiteType(pField->GetType(), sType);
					if((pField->GetType() == dtOid32 ||pField->GetType() == dtOid64) && pOIDField)
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


			int BindToField(sqlite3_stmt* pStmt, int nCol, eDataTypes type, CommonLib::CVariant* pVal)
			{
				if(!pVal)
					return 0;
				int retVal = 0;
				switch(type)
				{
				case dtInteger8:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<int8>());
					break;
				case dtInteger16:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<int16>());
					break;
				case dtInteger32:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<int32>());
					break;
				case dtOid32:
					retVal = sqlite3_bind_int64(pStmt, nCol, pVal->Get<int32>());
					break;
				case dtOid64:
					retVal = sqlite3_bind_int64(pStmt, nCol, pVal->Get<int64>());
					break;
				case dtInteger64:
					retVal = sqlite3_bind_int64(pStmt, nCol, pVal->Get<int64>());
					break;
				case dtUInteger8:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<byte>());
					break;
				case dtUInteger16:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<uint16>());
					break;
				case dtUInteger32:
					retVal = sqlite3_bind_int(pStmt, nCol, pVal->Get<uint32>());
					break;
				case dtUInteger64:
					retVal = sqlite3_bind_int64(pStmt, nCol, pVal->Get<uint64>());
					break;
				case dtFloat:
					retVal = sqlite3_bind_double(pStmt, nCol, pVal->Get<float>());
					break;
				case dtDouble:
					retVal = sqlite3_bind_double(pStmt, nCol, pVal->Get<double>());
					break;
				case  dtBlob:
					{
						CommonLib::IRefObjectPtr pObj = pVal->Get<CommonLib::IRefObjectPtr>();
						if(pObj.get())
						{
							CommonLib::CBlob *pBlob = (CommonLib::CBlob*)pObj.get();
							if(pBlob)
							{
								retVal = sqlite3_bind_blob(pStmt, nCol, pBlob->buffer(), pBlob->size(), SQLITE_TRANSIENT);
							}
						}

					}
					 break;
				case  dtString:
					{
						CommonLib::CString sText = pVal->Get<CommonLib::CString>();
						if(!sText.isEmpty())
							retVal = sqlite3_bind_text(pStmt, nCol, sText.cstr(), sText.length(), SQLITE_TRANSIENT);
					}
					break;
				}

				return retVal;
			}
		}

	}
}