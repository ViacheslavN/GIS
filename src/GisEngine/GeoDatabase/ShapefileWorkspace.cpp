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




		CShapefileWorkspace::CShapefileWorkspace(GisCommon::IPropertySetPtr& protSetPtr) : TBase(wiShapeFile),
			m_bLoad(false)
		{
			m_WorkspaceType = wiShapeFile;
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
		CShapefileWorkspace::CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath) : TBase(wiShapeFile), 
			m_bLoad(false)
		{
			m_sName = pszName;
			m_sPath = ShapefileUtils::NormalizePath(pszPath);

			m_ConnectProp = new GisCommon::CPropertySet();
			CommonLib::CVariant varName(m_sName);
			CommonLib::CVariant varPath(m_sPath);

			m_ConnectProp->SetProperty(c_PropertyName, varName);
			m_ConnectProp->SetProperty(c_PropertyPath, varPath);
			load();
		}
		 
		CShapefileWorkspace::~CShapefileWorkspace()
		{

		}
	
	 
		void CShapefileWorkspace::load()
		{
			CommonLib::CSSection::scoped_lock lock (m_mutex);
			std::vector<CommonLib::CString> vecFiles;
			CommonLib::FileSystem::getFiles(m_sPath + L"*.shp", vecFiles);

			for (size_t i = 0, sz = vecFiles.size(); i < sz; ++i)
			{
				IFeatureClassPtr pFClass = OpenFeatureClass(vecFiles[i]);
				if(pFClass.get())
				{
					m_vecDatasets.push_back(IDatasetPtr((IDataset*)pFClass.get()));
					m_DataSetMap.insert(std::make_pair(pFClass->GetDatasetViewName(), pFClass.get()));
				}
			}
			


		}
		void CShapefileWorkspace::clear()
		{

		}
		ITablePtr  CShapefileWorkspace::OpenTable(const CommonLib::CString& sName)
		{
			return ITablePtr();
		}
		IFeatureClassPtr CShapefileWorkspace::OpenFeatureClass(const CommonLib::CString& sName)
		{
			CommonLib::CSSection::scoped_lock lock (m_mutex);
			CommonLib::CString sViewName = CommonLib::FileSystem::FindOnlyFileName(sName);
			CShapefileFeatureClass * pFeatureClass = new CShapefileFeatureClass(this, m_sPath, sViewName + L".shp", sViewName);
			bool bLoad = pFeatureClass->reload(false);
			if(!bLoad)
			{
				delete pFeatureClass;
				return IFeatureClassPtr();
			}
			IFeatureClassPtr pFeaturePtr((IFeatureClass*)pFeatureClass);
	

			return pFeaturePtr;
		}
 
		ITablePtr  CShapefileWorkspace::CreateTable(const CommonLib::CString& name, IFields* fields)
		{
			return ITablePtr();
		}
		IFeatureClassPtr CShapefileWorkspace::CreateFeatureClass(const CommonLib::CString& sName, IFields* pFields, const CommonLib::CString& sShapeFieldName)
		{
		
			TDatasetMap::iterator it = m_DataSetMap.find(sName);
			if(it != m_DataSetMap.end())
				return IFeatureClassPtr((IFeatureClass*)it->second.get());


			if(sName.isEmpty())
				return IFeatureClassPtr(); //TO DO Error
			  if(!pFields)
				 return IFeatureClassPtr(); //TO DO Error

			  int shapeType = CommonLib::shape_type_null;
			  int fieldCount = pFields->GetFieldCount();
			  GisGeometry::ISpatialReferencePtr pSprefPtr;

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


			  CommonLib::CSSection::scoped_lock lock (m_mutex);
			  CommonLib::CString filePathBase = m_sPath + sName;
			  if(sName.right(4).equals(L".shp", false))
				  filePathBase += sName.left(filePathBase.length() - 4);
			  CommonLib::CString shpFilePath = filePathBase + L".shp";
			  CommonLib::CString dbfFilePath = filePathBase + L".dbf";
			  CommonLib::CString prjFileName = filePathBase + L".prj";



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

			 IFeatureClassPtr pFClass = OpenFeatureClass(sName);
			 if(pFClass.get())
			 {
				 m_vecDatasets.push_back(IDatasetPtr((IDataset*)pFClass.get()));
				 m_DataSetMap.insert(std::make_pair(pFClass->GetDatasetViewName(), pFClass.get()));
			 }
			 return pFClass;
		}


		ITablePtr CShapefileWorkspace::GetTable(const CommonLib::CString& name)
		{
			return ITablePtr();
		}
	 


		IWorkspacePtr CShapefileWorkspace::Open(const wchar_t *pszName, const wchar_t *pszPath)
		{
		
			IWorkspacePtr pWks = CWorkspaceHolder::GetWorkspace(wiShapeFile, pszPath);
			if(pWks.get())
				return pWks;

			CShapefileWorkspace* pShapeWks = new CShapefileWorkspace(pszName, pszPath);
			CWorkspaceHolder::AddWorkspace((IWorkspace*)pShapeWks, pszPath);
			return IWorkspacePtr((IWorkspace*)pShapeWks);
		}
		IWorkspacePtr CShapefileWorkspace::Open(CommonLib::IReadStream* pSteram)
		{

			CommonLib::CString sName;
			CommonLib::CString sPath;

			if(!pSteram->checkRead(4))
				return IWorkspacePtr();

			pSteram->read(sName);

			if(!pSteram->checkRead(4))
				return IWorkspacePtr();

			pSteram->read(sPath);

			return Open(sName.cwstr(), sPath.cwstr());
		}
		IWorkspacePtr CShapefileWorkspace::Open(GisCommon::IXMLNode *pNode)
		{

			CommonLib::CString sName = pNode->GetPropertyString("name", sName);
			CommonLib::CString sPath = pNode->GetPropertyString("path", sPath);

			return Open(sName.cwstr(), sPath.cwstr());
		}


		bool CShapefileWorkspace::save(CommonLib::IWriteStream *pWriteStream) const
		{
			pWriteStream->write(uint32(GetWorkspaceType()));
			pWriteStream->write(m_sName);
			pWriteStream->write(m_sPath);
			return true;
		}
		bool CShapefileWorkspace::load(CommonLib::IReadStream* pReadStream)
		{
			SAFE_READ_RES_EX(pReadStream, m_sName, 1);
			SAFE_READ_RES_EX(pReadStream, m_sPath, 1);
			load();
			return true;
		}

		bool CShapefileWorkspace::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			pXmlNode->AddPropertyInt32U("ID", uint32(GetWorkspaceType()));
			pXmlNode->AddPropertyString("name", m_sName);
			pXmlNode->AddPropertyString("path", m_sPath);
			return true;
		}
		bool CShapefileWorkspace::load(GisCommon::IXMLNode* pXmlNode)
		{
			m_sName = pXmlNode->GetPropertyString("name", m_sName);
			m_sPath = pXmlNode->GetPropertyString("path", m_sPath);
			load();
			return true;
		}

		ITransactionPtr CShapefileWorkspace::startTransaction()
		{
			return ITransactionPtr();
		}

	}
}