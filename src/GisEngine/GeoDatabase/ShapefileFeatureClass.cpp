#include "stdafx.h"
#include "ShapefileFeatureClass.h"
#include "CommonLibrary/File.h"
#include "Fields.h"
#include "GisGeometry/SpatialReferenceProj4.h"
#include "GisGeometry/Envelope.h"
#include "GeometryDef.h"
#include "Field.h"
#include "ShapefileRowCursor.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CShapefileFeatureClass::CShapefileFeatureClass(IWorkspace *pWorkSpace, const CommonLib::CString& sPath, 
			const CommonLib::CString& sName, const CommonLib::CString& sViewName) : TBase(pWorkSpace),
			m_sPath(sPath)
		{

			m_sDatasetName = sName;
			m_sDatasetViewName = sViewName;
		}
		CShapefileFeatureClass::~CShapefileFeatureClass()
		{

		}
		IRowPtr	CShapefileFeatureClass::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr  CShapefileFeatureClass::Search(IQueryFilter* filter, bool recycling)
		{
			return  ICursorPtr((ICursor*)new CShapefileRowCursor(filter, recycling, this));
		}

		void CShapefileFeatureClass::close()
		{
			m_shp.clear();
			m_dbf.clear();
		}

		CommonLib::CString CShapefileFeatureClass::GetFullName()
		{
			CommonLib::CString sFullName = m_sPath + m_sDatasetViewName + L".shp";
			return sFullName;
		}

		bool CShapefileFeatureClass::reload(bool write)
		{
			if(m_sPath.isEmpty() || m_sDatasetViewName.isEmpty())
				return false; //TO DO Error log

			m_pFields->Clear();
			m_pExtent.release();
			m_pSpatialReference.release();
			m_bHashOID = false;

			CommonLib::CString filePathBase = m_sPath + m_sDatasetViewName;
			CommonLib::CString shpFilePath = filePathBase + L".shp";
			CommonLib::CString dbfFilePath = filePathBase + L".dbf";
			CommonLib::CString prjFileName = filePathBase + L".prj";
			CommonLib::CString shpIdxFilePath = filePathBase + L".shapeidx";

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
			  GisBoundingBox bounds;
		 
			 if(objectCount > 0)
			 {
				 bounds.type = CommonLib::bbox_type_normal;
				 bounds.xMin = minBounds[0];
				 bounds.xMax = maxBounds[0];
				 bounds.yMin = minBounds[1];
				 bounds.yMax = maxBounds[1];
				 bounds.zMin = minBounds[2];
				 bounds.zMax = maxBounds[2];
				 bounds.mMin = minBounds[3];
				 bounds.mMax = maxBounds[3];
			 }

			
			 m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(prjFileName, GisGeometry::eSPRefTypePRJFilePath);
			 if(!m_pSpatialReference->IsValid())
			 {
				 m_pSpatialReference = new GisGeometry::CSpatialReferenceProj4(bounds);
			 }

			 m_pExtent = new GisGeometry::CEnvelope(bounds, m_pSpatialReference.get());
			 bool hasZ;
			 bool hasM;
			 m_ShapeType = ShapefileUtils::SHPTypeToGeometryType(shapeType, &hasZ, &hasM);
			 IGeometryDefPtr pGeometryDef(new  CGeometryDef(m_ShapeType, hasZ, hasM));
			 pGeometryDef->SetSpatialReference(m_pSpatialReference.get());
			 pGeometryDef->SetBaseExtent(bounds);

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
				 pFieldPtr->SetName(CommonLib::CString(name));
				 pFieldPtr->SetIsEditable(true);
				 pFieldPtr->SetIsNullable(true);
				 pFieldPtr->SetIsRequired(false);
				 pFieldPtr->SetType(fieldType);
				 pFieldPtr->SetLength(length);
				 pFieldPtr->SetPrecision(precision);
				 pFieldPtr->SetScale(scale);
				 m_pFields->AddField(pFieldPtr.get());
			 }
			 m_pShapeField = new CField();
			 m_pShapeField->SetGeometryDef(pGeometryDef.get());
			 m_pShapeField->SetType(dtGeometry);

			 m_sShapeFieldName = L"Shape";
			 int i = 0;
			 while(m_pFields->FieldExists(m_sShapeFieldName))
				 m_sShapeFieldName.format(L"Shape%d", i++);
			
			 m_pShapeField->SetName(m_sShapeFieldName);
			 m_pFields->AddField(m_pShapeField.get());
			 m_sShapeFieldName = m_pShapeField->GetName();


			 IFieldPtr pOidField(new CField());
			 // oidField->SetName(L"ObjectID");
			 pOidField->SetIsEditable(false);
			 pOidField->SetIsNullable(false);
			 pOidField->SetIsRequired(true);;
			 pOidField->SetType(dtOid32);
			 m_sOIDFieldName = L"ObjectID";
			 m_bHashOID = true;
			 i = 0;
			 while(m_pFields->FieldExists(m_sOIDFieldName))
				 m_sOIDFieldName.format(L"ObjectID%d", i++);
			 pOidField->SetName(m_sOIDFieldName);
			 m_pFields->AddField(pOidField.get());

			 m_pShapeIndex.release();
			 m_pShapeIndex = DatasetLite::IShapeFileIndex::open(shpIdxFilePath, shapeType);
			 return true;
		}

		bool CShapefileFeatureClass::CreateShapeIndex(const CommonLib::CString& sIndexName)
		{
			CommonLib::CString sFileName = sIndexName;
			CommonLib::CString filePathBase = m_sPath + m_sDatasetViewName;
			CommonLib::CString shpFilePath = filePathBase + L".shp";
			if(sFileName.isEmpty())
			{
			
				sFileName = filePathBase + L".shapeidx";
			}
			m_pShapeIndex = DatasetLite::IShapeFileIndex::create(sFileName, 8192, shpFilePath);
			return m_pShapeIndex.get() != 0;
		}
		DatasetLite::IShapeFileIndexPtr CShapefileFeatureClass::GetShapeIndex()
		{
			return m_pShapeIndex;
		}
		ShapefileUtils::SHPGuard* CShapefileFeatureClass::GetSHP()
		{
			return &m_shp;
		}
		ShapefileUtils::DBFGuard* CShapefileFeatureClass::GetDBF()
		{
			return& m_dbf;
		}


		bool CShapefileFeatureClass::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return true;
		}
		bool CShapefileFeatureClass::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}

		bool CShapefileFeatureClass::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return true;
		}
		bool CShapefileFeatureClass::load(const GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
	}

	
}