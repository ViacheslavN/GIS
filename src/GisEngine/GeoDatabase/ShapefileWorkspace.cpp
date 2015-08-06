#include "stdafx.h"
#include "ShapefileWorkspace.h"
#include "CommonLibrary/Variant.h"
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
			const CommonLib::CVariant *pVarName = m_ConnectProp->GetProperty(c_PropertyName);
			const CommonLib::CVariant *pVarPath = m_ConnectProp->GetProperty(c_PropertyPath);
			if(pVarPath)
			{
				CommonLib::ToStringVisitor vis;
				m_sPath = ShapefileUtils::NormalizePath(CommonLib::apply_visitor<CommonLib::ToStringVisitor>(*pVarPath, vis));
			}
			if(pVarName)
			{
				m_sName = ShapefileUtils::NormalizePath(CommonLib::apply_visitor<CommonLib::ToStringVisitor>(*pVarName, CommonLib::ToStringVisitor()));
			}
			load();
		}
		CShapefileWorkspace::CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath) : m_bLoad(false),  m_sName(pszName)
		{

			m_sPath = ShapefileUtils::NormalizePath(pszPath);

			m_ConnectProp = new Common::CPropertySet();
			CommonLib::CVariant varName(m_sName);
			CommonLib::CVariant varPath(m_sPath);

			m_ConnectProp->SetProperty(c_PropertyName, varName);
			m_ConnectProp->SetProperty(c_PropertyPath, varPath);
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
		IDatasetPtr CShapefileWorkspace::GetDataset(uint32 nIdx) const
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
				IFeatureClassPtr pFClass = OpenFeatureClass(vecFiles[i]);
				if(pFClass.get())
					m_vecDatasets.push_back(IDatasetPtr((IDataset*)pFClass.get()));
			}
			


		}
		void CShapefileWorkspace::clear()
		{
			/*for (size_t i = 0, sz = m_vecDatasets.size(); i < sz; ++i)
			{
				delete m_vecDatasets[i];
			}
			m_vecDatasets.clear();*/
		}
		ITablePtr  CShapefileWorkspace::OpenTable(const CommonLib::str_t& sName)
		{
			return ITablePtr();
		}
		IFeatureClassPtr CShapefileWorkspace::OpenFeatureClass(const CommonLib::str_t& sName)
		{
			CommonLib::str_t sViewName = CommonLib::FileSystem::FindOnlyFileName(sName);
			CShapefileFeatureClass * pFeatureClass =new CShapefileFeatureClass(this, m_sPath, sViewName + L".shp", sViewName);
			bool bLoad = pFeatureClass->reload(false);
			if(!bLoad)
			{
				delete pFeatureClass;
			}
			IFeatureClassPtr pFeaturePtr((IFeatureClass*)pFeatureClass);
			return pFeaturePtr;
		}
 
		ITablePtr  CShapefileWorkspace::CreateTable(const CommonLib::str_t& name, IFields* fields)
		{
			return ITablePtr();
		}
		IFeatureClassPtr CShapefileWorkspace::CreateFeatureClass(const CommonLib::str_t& sName, IFields* pFields, const CommonLib::str_t& sShapeFieldName)
		{
			if(sName.isEmpty())
				return IFeatureClassPtr(); //TO DO Error
			  if(!pFields)
				 return IFeatureClassPtr(); //TO DO Error

			  int shapeType = CommonLib::shape_type_null;
			  int fieldCount = pFields->GetFieldCount();
			  Geometry::ISpatialReferencePtr pSprefPtr;

			  for(int i = 0; i < fieldCount; ++i)
			  {
				  IFieldPtr pField = pFields->GetField(i);
				  switch(pField->GetType())
				  {
				  case dtInteger8:
				  case dtUInteger8:
				  case dtInteger16:
				  case dtUInteger16:
				  case dtInteger32:
				  case dtUInteger32:
				  case dtInteger64:
				  case dtUInteger64:
				  case dtString:
				  case dtDate:
				  case dtOid:
					  break;
				  case dtGeometry:
					  {
						  IShapeField* pShpField = (IShapeField*)pField.get();
						  CommonLib::eShapeType gtype = pShpField->GetGeometryDef()->GetGeometryType();
						  if(gtype == CommonLib::shape_type_point )
							  shapeType = SHPT_MULTIPOINT;
						  else if(gtype == CommonLib::shape_type_polyline)
						  {
							  shapeType = SHPT_ARC;
						  }
						  if(gtype == CommonLib::shape_type_polygon)
						  {
							  shapeType = SHPT_POLYGON;
						  }
						  if(gtype == CommonLib::shape_type_multipatch)
						  {
							  shapeType = SHPT_MULTIPATCH;
						  }
						  pSprefPtr = pShpField->GetGeometryDef()->GetSpatialReference();
					  }
					  break;
				  default:
					 return IFeatureClassPtr(); //to do error
				  }
			  }

			  if(shapeType == CommonLib::shape_type_null)
				   return IFeatureClassPtr(); //to do error


			CommonLib::str_t filePathBase = m_sPath + sName;
			if(sName.right(4).equals(L".shp", false))
			  filePathBase += sName.left(filePathBase.length() - 4);
			 CommonLib::str_t shpFilePath = filePathBase + L".shp";
			 CommonLib::str_t dbfFilePath = filePathBase + L".dbf";
			 CommonLib::str_t prjFileName = filePathBase + L".prj";


			 ShapeLib::SHPHandle shp = ShapeLib::SHPCreate(shpFilePath.cstr(), shapeType);
			 if(!shp)
				return IFeatureClassPtr();  //to do error
			 SHPClose(shp);

			 ShapeLib::DBFHandle dbf = ShapeLib::DBFCreate(dbfFilePath.cstr());
			 if(!dbf)
			 {
				 CommonLib::FileSystem::deleteFile(shpFilePath.cwstr());
				 return IFeatureClassPtr();  //to do error
			 }
			 if(pSprefPtr.get())
			 {
				 FILE* pFile = _wfopen(prjFileName.cwstr(), L"wt");
				 fputs(pSprefPtr->GetProjectionString().cstr(), pFile);
				 fclose(pFile);
			 }

			 for(int i = 0; i < fieldCount; ++i)
			 {
				 IFieldPtr pField = pFields->GetField(i);
				 switch(pField->GetType())
				 {
					case dtInteger8:
					case dtInteger16:
					case dtInteger32:
					case dtInteger64:
					case dtUInteger8:
					case dtUInteger16:
					case dtUInteger32:
					case dtUInteger64:
				
						ShapeLib::DBFAddField(dbf, pField->GetName().cstr(), ShapeLib::FTInteger, pField->GetPrecision(), pField->GetScale());
						break;
					case dtFloat:
					case dtDouble:
						ShapeLib::DBFAddField(dbf, pField->GetName().cstr(), ShapeLib::FTDouble, pField->GetPrecision(), pField->GetScale());
						break;
					case dtString:
						ShapeLib::DBFAddField(dbf, pField->GetName().cstr(), ShapeLib::FTString, pField->GetLength(), 0);
						break;
					case dtDate:
						ShapeLib::DBFAddField(dbf, pField->GetName().cstr(), ShapeLib::FTDate, pField->GetLength(), 0);
						break;
					case dtOid:
						break;
					case dtGeometry:
						break;
			 
				 default:
					 return IFeatureClassPtr();
				 }
			 }

			 DBFClose(dbf);
			 return OpenFeatureClass(sName);

		}

	}
}