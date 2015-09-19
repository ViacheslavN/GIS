#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_UTILS_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_UTILS_H_


#include "GeoDatabase.h"
struct sqlite3_stmt;
namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{
			void FieldType2SQLiteType(eDataTypes type, CommonLib::CString& sSQLiteType);
			eDataTypes SQLiteType2FieldType(CommonLib::CString& sSQLiteType);
			eDataTypes SQLiteType2FieldType(int nSQLiteFieldType);
			void CreateSQLCreateTable(IFields *pFields, const CommonLib::CString& sTableName, 
				CommonLib::CString& sSQL, CommonLib::CString* pOIDField = NULL, CommonLib::CString* pShapeField = NULL, 
				CommonLib::CString* pAnno = NULL, CommonLib::eShapeType *pSPType = NULL, GisGeometry::ISpatialReferencePtr* pSPref = NULL);

			int BindToField(sqlite3_stmt* pStmt, int nCol, eDataTypes type, CommonLib::CVariant* pVal);
			
		}
	}
}
#endif