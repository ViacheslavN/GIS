#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_FEATURE_CLASS_H_


#include "GeoDatabase.h"
#include "ShapefileUtils.h"
#include "FeatureClassBase.h"
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

			//IDataset
		/*	virtual eDatasetType  GetDatasetType() const {return dtFeatureClass;}
			virtual IWorkspace*    GetWorkspace() const {return m_pWorkspace;}
			virtual const CommonLib::CString&   GetDatasetName() const {return m_sDatasetName;}
			virtual const CommonLib::CString&   GetDatasetViewName() const {return m_sDatasetViewName;}*/

			//ITable
		/*	virtual void                 AddField(IField* field);
			virtual void                 DeleteField(const CommonLib::CString& fieldName);
			virtual IFieldsPtr             GetFields() const;
			virtual bool                 HasOIDField() const;
			virtual const CommonLib::CString& GetOIDFieldName() const;*/
			virtual IRowPtr				  GetRow(int64 id);
			virtual ICursorPtr			  Search(IQueryFilter* filter, bool recycling);

			//IFeatureClass
			/*virtual CommonLib::eShapeType GetGeometryType() const;
			virtual const CommonLib::CString&         GetShapeFieldName() const;
			virtual GisGeometry::IEnvelopePtr			 GetExtent() const ;
			virtual GisGeometry::ISpatialReferencePtr GetSpatialReference() const;*/


			bool reload(bool write);
			void close();
			ShapefileUtils::SHPGuard* GetSHP();
			ShapefileUtils::DBFGuard* GetDBF();

			CommonLib::CString GetFullName();

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);

		private:
			//IWorkspace *m_pWorkSpace;
			IShapeFieldPtr m_pShapeField;
			IFieldPtr m_pOIDField;
			//IFieldsPtr m_FieldsPtr;

			CommonLib::CString m_sPath;
			/*CommonLib::CString m_sName;
			CommonLib::CString m_sViewName;
			CommonLib::CString m_sShapeFieldName;
			CommonLib::CString m_sOIDName;*/


			ShapefileUtils::SHPGuard m_shp;
			ShapefileUtils::DBFGuard m_dbf;
		/*	GisGeometry::IEnvelopePtr	 m_pExtent;
			CommonLib::eShapeType m_ShapeType;
			GisGeometry::ISpatialReferencePtr m_pSpatialReferencePtr;*/
			

	

		};
	}
}

#endif