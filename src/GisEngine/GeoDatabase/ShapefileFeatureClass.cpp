#include "stdafx.h"
#include "ShapefileFeatureClass.h"
#include "CommonLibrary/File.h"
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
		}
		CShapefileFeatureClass::~CShapefileFeatureClass()
		{

		}

		bool CShapefileFeatureClass::load(bool write)
		{
			if(!m_sPath.isEmpty() || !m_sViewName.isEmpty())
				return false; //TO DO Error log

			CommonLib::str_t filePathBase = m_sPath + m_sViewName;
			CommonLib::str_t shpFilePath = filePathBase + L".shp";
			CommonLib::str_t dbfFilePath = filePathBase + L".dbf";
			CommonLib::str_t prjFileName = filePathBase + L".prj";

			  const wchar_t* mode = write ? L"r+b" : L"rb";
			 m_shp.file = shapelib::SHPOpen(shpFilePath.cwstr(), mode);
			 if(!m_shp.file)
				return false; //TO DO Error log
			 m_dbf.file = shapelib::DBFOpen(dbfFilePath.cwstr(), mode);
			 if(!m_dbf.file)
				 return false; //TO DO Error log
		}
	}
}