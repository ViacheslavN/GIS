#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_RESULT_SET_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_RESULT_SET_H_

#include "CommonLibrary/String.h"
#include "CommonLibrary/blob.h"
#include "CommonLibrary/GeoShape.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "SQLiteResultSetBase.h" 

namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{

			class CSQLiteResultSetRead : public CSQLiteResultSetBase
			{
			public:

				CSQLiteResultSetRead (sqlite3_stmt* stmt) : CSQLiteResultSetBase(stmt)
				{
				}

				~CSQLiteResultSetRead ()
				{}


				


	
			};

		}
	}
}

#endif