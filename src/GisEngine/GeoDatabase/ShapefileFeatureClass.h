#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_


#include "GeoDatabase.h"
#include "ShapefileUtils.h"
#include "FeatureClassBase.h"
#include "../../EmbDB/DatasetLite/SpatialDataset.h"
 
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileFeatureClass : public IFeatureClassBase<IFeatureClass>
		{
		public:
			typedef  IFeatureClassBase<IFeatureClass> TBase;
			CShapefileFeatureClass(IWorkspace *pWorkSpace, const CommonLib::CString& sPath, const CommonLib::CString& sName, const CommonLib::CString& sViewName);
			~CShapefileFeatureClass();

		
			virtual IRowPtr				  GetRow(int64 id);
			virtual ICursorPtr			  Search(IQueryFilter* filter, bool recycling);


			bool reload(bool write);
			void close();
			ShapefileUtils::SHPGuard* GetSHP();
			ShapefileUtils::DBFGuard* GetDBF();
			DatasetLite::IShapeFileIndexPtr GetShapeIndex();
			bool CreateShapeIndex(const CommonLib::CString& sIndexName = L"");


			CommonLib::CString GetFullName();

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

		private:
		 
			IShapeFieldPtr m_pShapeField;
			IFieldPtr m_pOIDField;
		 

			CommonLib::CString m_sPath;
		 
			ShapefileUtils::SHPGuard m_shp;
			ShapefileUtils::DBFGuard m_dbf;
			DatasetLite::IShapeFileIndexPtr m_pShapeIndex;
		};
	}
}

#endif