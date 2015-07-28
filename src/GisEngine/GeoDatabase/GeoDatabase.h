#ifndef GIS_ENGINE_GEO_DATABASE_H_
#define GIS_ENGINE_GEO_DATABASE_H_

#include "CommonLibrary/str_t.h" 
#include "Common/GisEngineCommon.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		enum eWorkspaceID
		{
			wiShapeFile = 1,
			wiEmbDB,
			wiSpatialLite,
			wiOracle,
			wiMSSQL,
			wiPostGIS

		};
		enum eDatasetType
		{
			dtUndefined    = 0,
			dtTypeTable    = 1,
			dtFeatureClass = 2,
			dtGroupDataset = 4,
			dtTypeRaster   = 8,
			dtTypeAny      = -1
		};

		struct IWorkspace
		{
				IWorkspace(){}
				virtual ~IWorkspace(){}
				virtual const CommonLib::str_t& GetWorkspaceName() const = 0; 
				virtual Common::IPropertySet*  GetConnectionProperties() const = 0; 
				virtual eWorkspaceID GetWorkspaceID() const = 0;
		};
		
		struct IDataset  
		{
			virtual eDatasetType  GetDatasetType() const = 0;
			virtual IWorkspace*    GetWorkspace() const = 0;
			virtual const CommonLib::str_t&   GetDatasetName() const = 0;
		};

		struct ITable : public IDataset
		{

		};
	}
}