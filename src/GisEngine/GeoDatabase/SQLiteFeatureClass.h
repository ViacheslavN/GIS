#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_FEATURE_CLASS_H_

#include "FeatureClassBase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{
			class CSQLiteDB;
		}
		class CSQLiteWorkspace;
		class CSQLiteFeatureClass : public IFeatureClassBase<IFeatureClass>
		{
		public:
			typedef  IFeatureClassBase<IFeatureClass> TBase;
			CSQLiteFeatureClass(CSQLiteWorkspace *pWorkspace, 
				const CommonLib::CString& sName, 
				const CommonLib::CString& sViewName);
			~CSQLiteFeatureClass();

			virtual IRowPtr		GetRow(int64 id);
			virtual ICursorPtr	Search(IQueryFilter* filter, bool recycling);

			const CommonLib::CString& GetRTReeIndexName() const { return m_sSpatialIndexName;}
 


		 

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);

			bool open();
			bool saveFeatureInfo();
			bool CreateFeatureClass(IFields* pFields);
			static bool IsFeatureClass(CommonLib::CString& sName, SQLiteUtils::CSQLiteDB *pDB);

			
		private:
			CSQLiteWorkspace *m_pSQLiteWorkspace;
			CommonLib::CString m_sSpatialIndexName;
			CommonLib::CString m_sPropTableName;
			eDataTypes m_nOIDType;
		};
	}
}

#endif