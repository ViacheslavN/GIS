#include "stdafx.h"
#include "ShapefileFeatureClass.h"
#include "CommonLibrary/File.h"
#include "Fields.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "GeometryDef.h"
#include "Field.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		CShapefileFeatureClass::CShapefileFeatureClass(IWorkspace *pWorkSpace, const CommonLib::str_t& sPath, 
			const CommonLib::str_t& sName, const CommonLib::str_t& sViewName) :
			m_pWorkSpace(pWorkSpace), m_sPath(sPath), m_sName(sName), m_sViewName(sViewName), m_ShapeType(CommonLib::shape_type_null)
		{
		/*	if(m_sName.isEmpty())
			{
				m_sPath = CommonLib::FileSystem::FindFilePath(m_sPath);
				m_sName = CommonLib::FileSystem::FindFileName(m_sName);
			}*/

			m_FieldsPtr = new CFields();
		}
		CShapefileFeatureClass::~CShapefileFeatureClass()
		{

		}

		void CShapefileFeatureClass::AddField(IField* pField)
		{
			
		}
		void  CShapefileFeatureClass::DeleteField(const CommonLib::str_t& fieldName)
		{
			
		}
		IFieldsPtr CShapefileFeatureClass::GetFields() const
		{
			return m_FieldsPtr;
		}
		bool   CShapefileFeatureClass::HasOIDField() const
		{
			return true;
		}
		const CommonLib::str_t& CShapefileFeatureClass::GetOIDFieldName() const
		{
			return m_sOIDName;
		}
		IRowPtr	CShapefileFeatureClass::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr  CShapefileFeatureClass::Search(IQueryFilter* filter, bool recycling)
		{
			return ICursorPtr();
		}

		//IFeatureClass
		CommonLib::eShapeType CShapefileFeatureClass::GetGeometryType() const
		{
			return m_ShapeType;
		}
		const CommonLib::str_t&  CShapefileFeatureClass::GetShapeFieldName() const
		{
			return m_sShapeFieldName;
		}
		const GisBoundingBox& CShapefileFeatureClass::GetExtent() const
		{
			return m_Extent;
		}
		Geometry::ISpatialReferencePtr CShapefileFeatureClass::GetSpatialReference() const
		{
			return m_pSpatialReferencePtr;
		}


		bool CShapefileFeatureClass::reload(bool write)
		{
			if(!m_sPath.isEmpty() || !m_sViewName.isEmpty())
				return false; //TO DO Error log

			m_FieldsPtr->Clear();
			m_pSpatialReferencePtr.release();
			m_Extent = GisBoundingBox();

			CommonLib::str_t filePathBase = m_sPath + m_sViewName;
			CommonLib::str_t shpFilePath = filePathBase + L".shp";
			CommonLib::str_t dbfFilePath = filePathBase + L".dbf";
			CommonLib::str_t prjFileName = filePathBase + L".prj";

			  const char* szAccess = write ? "r+b" : "rb";
			 m_shp.file = ShapeLib::SHPOpen(shpFilePath.cstr(), szAccess);
			 if(!m_shp.file)
				return false; //TO DO Error log
			 m_dbf.file = ShapeLib::DBFOpen(dbfFilePath.cstr(), szAccess);
			 if(!m_dbf.file)
				 return false; //TO DO Error log


			 int objectCount;
			 int shapeType;
			 double minBounds[4];
			 double maxBounds[4];
			 SHPGetInfo(m_shp.file, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);

		 
			 if(objectCount > 0)
			 {
				 m_Extent.type = CommonLib::bbox_type_normal;
				 m_Extent.xMin = minBounds[0];
				 m_Extent.xMax = maxBounds[0];
				 m_Extent.yMin = minBounds[1];
				 m_Extent.yMax = maxBounds[1];
				 m_Extent.zMin = minBounds[2];
				 m_Extent.zMax = maxBounds[2];
				 m_Extent.mMin = minBounds[3];
				 m_Extent.mMax = maxBounds[3];
			 }

			 m_pSpatialReferencePtr = new Geometry::CSpatialReferenceProj4(prjFileName, Geometry::eSPRefTypePRJFilePath);
			 if(!m_pSpatialReferencePtr->IsValid())
			 {
				 m_pSpatialReferencePtr = new Geometry::CSpatialReferenceProj4(m_Extent);
			 }
			 bool hasZ;
			 bool hasM;
			 CommonLib::eShapeType geomType = ShapefileUtils::SHPTypeToGeometryType(shapeType, &hasZ, &hasM);
			 IGeometryDefPtr pGeometryDefPtr(new  CGeometryDef(geomType, hasZ, hasM));

			 int fieldCount = ShapeLib::DBFGetFieldCount(m_dbf.file);
			 for(int fieldNum = 0; fieldNum < fieldCount; ++fieldNum)
			 {
				 char name[33];
				 int width;
				 int dec;
				 ShapeLib::DBFFieldType shpFieldType = ShapeLib::DBFGetFieldInfo(m_dbf.file, fieldNum, name, &width, &dec);

				 eDataTypes fieldType;
				 int length;
				 int precision;
				 int scale;
				 fieldType = ShapefileUtils::SHPFieldInfoToFieldInfo(shpFieldType, width, dec, &length, &precision, &scale);

				 IFieldPtr pFieldPtr(new CField());
				 pFieldPtr->SetName(CommonLib::str_t(name));
				 pFieldPtr->SetIsEditable(true);
				 pFieldPtr->SetIsNullable(false);
				 pFieldPtr->SetIsRequired(false);
				 pFieldPtr->SetType(fieldType);
				 pFieldPtr->SetLength(length);
				 pFieldPtr->SetPrecision(precision);
				 pFieldPtr->SetScale(scale);
				 m_FieldsPtr->AddField(pFieldPtr.get());
			 }
			 m_pShapeField = new CField();
			 m_pShapeField->SetGeometryDef(pGeometryDefPtr.get());

			 m_sShapeFieldName = L"Shape";
			 int i = 0;
			 while(m_FieldsPtr->FieldExists(m_sShapeFieldName))
				 m_sShapeFieldName.format(L"Shape%d", i++);
			
			 m_pShapeField->SetName(m_sShapeFieldName);
			 m_FieldsPtr->AddField(m_pShapeField.get());
			 m_sShapeFieldName = m_pShapeField->GetName();


			 IFieldPtr pOidField(new CField());
			 // oidField->SetName(L"ObjectID");
			 pOidField->SetIsEditable(false);
			 pOidField->SetIsNullable(false);
			 pOidField->SetIsRequired(true);;
			 pOidField->SetType(dtOid);
			 m_sOIDName = L"ObjectID";
			 i = 0;
			 while(m_FieldsPtr->FieldExists(m_sOIDName))
				 m_sOIDName.format(L"ObjectID%d", i++);
			 pOidField->SetName(m_sOIDName);
			 m_FieldsPtr->AddField(pOidField.get());


			 return true;
		}
	}
}