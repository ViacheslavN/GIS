#include "stdafx.h"
#include "ShapefileWorkspace.h"
#include "CommonLibrary/BaseVariant.h"
#include "Common/PropertySet.h"
#include "ShapefileUtils.h"
#include "CommonLibrary/File.h"
#include "ShapefileFeatureClass.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		const wchar_t CShapefileWorkspace::c_PropertyName[] = L"NAME";
		const wchar_t CShapefileWorkspace::c_PropertyPath[] = L"PATH";

		CShapefileWorkspace::CShapefileWorkspace(Common::IPropertySetPtr& protSetPtr) : m_bLoad(false)
		{
			m_ConnectProp = protSetPtr;
			CommonLib::IVariant *pVarName = m_ConnectProp->GetProperty(c_PropertyName);
			CommonLib::IVariant *pVarPath = m_ConnectProp->GetProperty(c_PropertyPath);
			if(pVarPath && pVarPath->getType() == CommonLib::dtString)
			{
				pVarPath->getVal(m_sPath);
				m_sPath = ShapefileUtils::NormalizePath(m_sPath);
			}
			if(pVarName && pVarName->getType() == CommonLib::dtString)
			{
				pVarName->getVal(m_sName);
			}
			load();
		}
		CShapefileWorkspace::CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath) : m_bLoad(false),  m_sName(pszName)
		{

			m_sPath = ShapefileUtils::NormalizePath(pszPath);

			m_ConnectProp = new Common::CPropertySet();
			CommonLib::TVarString varName(m_sName);
			CommonLib::TVarString varPath(m_sPath);

			m_ConnectProp->SetProperty(c_PropertyName, &varName);
			m_ConnectProp->SetProperty(c_PropertyPath, &varPath);
			load();
		}
		 

		const CommonLib::str_t& CShapefileWorkspace::GetWorkspaceName() const
		{
			return m_sPath;
		}
		Common::IPropertySetPtr  CShapefileWorkspace::GetConnectionProperties() const
		{
			return m_ConnectProp;
		}
		eWorkspaceID CShapefileWorkspace::GetWorkspaceID() const
		{
			return wiShapeFile;
		}
		/*IDatasetContainer* CShapefileWorkspace::GetDatasetContainer()
		{
			return &m_DatasetContainer;
		}*/

		uint32 CShapefileWorkspace::GetDatasetCount() const
		{
			return m_vecDatasets.size();
		}
		IDataset* CShapefileWorkspace::GetDataset(uint32 nIdx) const
		{
			return m_vecDatasets[nIdx];
		}
		void CShapefileWorkspace::RemoveDataset(uint32 nIdx)
		{
			assert(nIdx < m_vecDatasets.size());
			m_vecDatasets.erase(m_vecDatasets.begin() + nIdx);
		}
		void CShapefileWorkspace::RemoveDataset(IDataset *pDataset)
		{
			TVecDataset::iterator it = std::find(m_vecDatasets.begin(), m_vecDatasets.end(), pDataset);
			if(it != m_vecDatasets.end())
				m_vecDatasets.erase(it);
		}
	 
		void CShapefileWorkspace::load()
		{
			std::vector<CommonLib::str_t> vecFiles;
			CommonLib::FileSystem::getFiles(m_sPath + L"*.shp", vecFiles);

			for (size_t i = 0, sz = vecFiles.size(); i < sz; ++i)
			{
				IFeatureClass* pFClass = OpenFeatureClass(vecFiles[i]);
				if(pFClass)
					m_vecDatasets.push_back(pFClass);
			}
			


		}
		void CShapefileWorkspace::clear()
		{
			for (size_t i = 0, sz = m_vecDatasets.size(); i < sz; ++i)
			{
				delete m_vecDatasets[i];
			}
			m_vecDatasets.clear();
		}
		ITable*  CShapefileWorkspace::OpenTable(const CommonLib::str_t& sName)
		{
			return NULL;
		}
		IFeatureClass* CShapefileWorkspace::OpenFeatureClass(const CommonLib::str_t& sName)
		{
			CommonLib::str_t sViewName = CommonLib::FileSystem::FindOnlyFileName(sName);
			CShapefileFeatureClass *pFeatureClass = new CShapefileFeatureClass(this, m_sPath, sViewName + L".shp", sViewName);
			bool bLoad = pFeatureClass->load();
			if(!bLoad)
			{
				delete pFeatureClass;
			}
			return pFeatureClass;
		}
 
		ITable*  CShapefileWorkspace::CreateTable(const CommonLib::str_t& name, IFields* fields)
		{
			return NULL;
		}
		IFeatureClass* CShapefileWorkspace::CreateFeatureClass(const CommonLib::str_t& sName, IFields* pFields, const CommonLib::str_t& sShapeFieldName)
		{
			if(sName.isEmpty())
				return NULL; //TO DO Error
			  if(!pFields)
				 return NULL; //TO DO Error

			  int shapeType = CommonLib::shape_type_null;
			  int fieldCount = pFields->GetFieldCount();
			  Geometry::ISpatialReferencePtr pSprefPtr;

			  for(int i = 0; i < fieldCount; ++i)
			  {
				  IField* pField = pFields->GetField(i);
				  switch(pField->GetType())
				  {
				  case CommonLib::dtInteger8:
				  case CommonLib::dtUInteger8:
				  case CommonLib::dtInteger16:
				  case CommonLib::dtUInteger16:
				  case CommonLib::dtInteger32:
				  case CommonLib::dtUInteger32:
				  case CommonLib::dtInteger64:
				  case CommonLib::dtUInteger64:
				  case CommonLib::dtString:
				  case CommonLib::dtDate:
				  case CommonLib::dtOid:
					  break;
				  case CommonLib::dtGeometry:
					  {
						  IShapeField* pShpField = (IShapeField*)pField;
						  CommonLib::eShapeType gtype = pShpField->GetGeometryDef()->GetGeometryType();
						  if(gtype == CommonLib::shape_type_point )
							  shapeType = shapelib::SHPT_MULTIPOINT;
						  else if(gtype == CommonLib::shape_type_polyline)
						  {
							  shapeType = shapelib::SHPT_ARC;
						  }
						  if(gtype == CommonLib::shape_type_polygon)
						  {
							  shapeType = shapelib::SHPT_POLYGON;
						  }
						  if(gtype == CommonLib::shape_type_multipatch)
						  {
							  shapeType = shapelib::SHPT_MULTIPATCH;
						  }
						  pSprefPtr = pShpField->GetGeometryDef()->GetSpatialReference();
					  }
					  break;
				  default:
					 return NULL; //to do error
				  }
			  }

			  if(shapeType == CommonLib::shape_type_null)
				   return NULL; //to do error


			CommonLib::str_t filePathBase = m_sPath + sName;
			if(sName.right(4).equals(L".shp", false))
			  filePathBase += sName.left(filePathBase.length() - 4);
			 CommonLib::str_t shpFilePath = filePathBase + L".shp";
			 CommonLib::str_t dbfFilePath = filePathBase + L".dbf";
			 CommonLib::str_t prjFileName = filePathBase + L".prj";


			 shapelib::SHPHandle shp = shapelib::SHPCreate(shpFilePath.cwstr(), shapeType);
			 if(!shp)
				return NULL;  //to do error
			 shapelib::SHPClose(shp);

			 shapelib::DBFHandle dbf = shapelib::DBFCreate(dbfFilePath.cwstr());
			 if(!dbf)
			 {
				 CommonLib::FileSystem::deleteFile(shpFilePath.cwstr());
				 return NULL;  //to do error
			 }
			 if(pSprefPtr.get())
			 {
				 FILE* pFile = _wfopen(prjFileName.cwstr(), L"wt");
				 fputs(pSprefPtr->GetProjectionString().cstr(), pFile);
				 fclose(pFile);
			 }

			 for(int i = 0; i < fieldCount; ++i)
			 {
				 IField* pField = pFields->GetField(i);
				 switch(pField->GetType())
				 {
					 case CommonLib::dtInteger8:
					case CommonLib::dtInteger16:
					case CommonLib::dtInteger32:
					case CommonLib::dtInteger64:
					case CommonLib::dtUInteger8:
					case CommonLib::dtUInteger16:
					case CommonLib::dtUInteger32:
					case CommonLib::dtUInteger64:
				
						shapelib::DBFAddField(dbf, pField->GetName().cstr(), shapelib::FTInteger, pField->GetPrecision(), pField->GetScale());
						break;
					case CommonLib::dtFloat:
					case CommonLib::dtDouble:
						shapelib::DBFAddField(dbf, pField->GetName().cstr(), shapelib::FTDouble, pField->GetPrecision(), pField->GetScale());
						break;
					case CommonLib::dtString:
						shapelib::DBFAddField(dbf, pField->GetName().cstr(), shapelib::FTString, pField->GetLength(), 0);
						break;
					case CommonLib::dtDate:
						shapelib::DBFAddField(dbf, pField->GetName().cstr(), shapelib::FTDate, pField->GetLength(), 0);
						break;
					case CommonLib::dtOid:
						break;
					case CommonLib::dtGeometry:
						break;
			 
				 default:
					 return NULL;
				 }
			 }

			 shapelib::DBFClose(dbf);
			 return OpenFeatureClass(sName);

		}

	}
}