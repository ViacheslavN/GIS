#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_UTILS_H_
#define GIS_ENGINE_GEO_DATABASE_EMBDB_UTILS_H_

#include "GeoDatabase.h"
#include "../../EmbDB/EmbDB/embDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace embDBUtils
		{
			embDB::eSpatialCoordinatesUnits GeometryUnits2EmbDB(GisEngine::GisCommon::Units units);
			embDB::eDataTypes FieldType2EmbDBType(eDataTypes type, CommonLib::CString* sStringType = NULL);
			eDataTypes EmbDBType2FieldType(embDB::eDataTypes);
			IFieldsPtr EmbDBFields2Fields(embDB::IFields *pFields);
			IFieldPtr EmbDBField2Field(embDB::IField *pField);

			bool CreateTable(const wchar_t* pszTableName, IFields* pFields, embDB::IConnection *pConn, CommonLib::CString* pOIDField = NULL, CommonLib::CString* pShapeField = NULL, 
				CommonLib::CString* pAnno = NULL, CommonLib::eShapeType *pSPType = NULL, GisGeometry::ISpatialReferencePtr* pSPref = NULL);
		
			embDB::eTransactionDataType  TranType2EmbDbTranType(eTransactionType type);

		}
	}
}

#endif