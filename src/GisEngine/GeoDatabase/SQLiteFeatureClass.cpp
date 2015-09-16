#include "stdafx.h"
#include "SQLiteFeatureClass.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CSQLiteFeatureClass::CSQLiteFeatureClass(CSQLiteWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName) :
			TBase((IWorkspace*)pWorkspace), m_pSQLiteWorkspace(pWorkspace)
		{
				m_sDatasetName = sName;
				m_sDatasetViewName = sViewName;
		}
 
		CSQLiteFeatureClass::~CSQLiteFeatureClass()
		{

		}

		IRowPtr	CSQLiteFeatureClass::GetRow(int64 id)
		{
			return IRowPtr();
		}
		ICursorPtr	CSQLiteFeatureClass::Search(IQueryFilter* filter, bool recycling)
		{
			return ICursorPtr();
		}


		bool CSQLiteFeatureClass::save(CommonLib::IWriteStream *pWriteStream) const
		{
			return false;
		}
		bool CSQLiteFeatureClass::load(CommonLib::IReadStream* pReadStream)
		{
			return false;
		}

		bool CSQLiteFeatureClass::saveXML(GisCommon::IXMLNode* pXmlNode) const
		{
			return false;
		}
		bool CSQLiteFeatureClass::load(GisCommon::IXMLNode* pXmlNode)
		{
			return false;
		}
	}
}
