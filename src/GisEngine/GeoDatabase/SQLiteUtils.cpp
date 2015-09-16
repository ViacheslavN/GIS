#include "stdafx.h"
#include "SQLiteUtils.h"



namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLitefUtils
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
						sSQLiteType = L"BLOB";
						break;
					case  dtString:
						sSQLiteType = L"TEXT";
						break;
				}
			}
		}
	}
}