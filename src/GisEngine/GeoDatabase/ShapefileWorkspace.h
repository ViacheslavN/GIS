#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_

#include "GeoDatabase.h"
#include "Common/SimpleEnum.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileWorkspace : public IWorkspace
		{

			//typedef Common::CSimpleEnum<IDataset, IDatasetContainer> TDatasetContainer;
			public:

				static const wchar_t c_PropertyName[];
				static const wchar_t c_PropertyPath[];

				CShapefileWorkspace(Common::IPropertySetPtr& protSetPtr);
				CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath);
				~CShapefileWorkspace();
			
				virtual const CommonLib::str_t& GetWorkspaceName() const; 
				virtual Common::IPropertySetPtr  GetConnectionProperties() const; 
				virtual eWorkspaceID GetWorkspaceID() const;
				//virtual IDatasetContainer* GetDatasetContainer();

				virtual uint32 GetDatasetCount() const = 0;
				virtual IDatasetPtr GetDataset(uint32 nIdx) const = 0;
				virtual void RemoveDataset(uint32 nIdx) = 0;
				virtual void RemoveDataset(IDataset *pDataset) = 0;

				virtual ITablePtr  CreateTable(const CommonLib::str_t& name, IFields* fields);
				virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::str_t& name, IFields* fields, const CommonLib::str_t& shapeFieldName);
				virtual ITablePtr  OpenTable(const CommonLib::str_t& name);
				virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::str_t& name);
 			private:
				void load();
				void clear();
			private:
				IWorkspace *m_pWorkSpace;
				IFieldsPtr m_FieldsPtr;

				bool m_bLoad;
				CommonLib::str_t m_sPath;
				CommonLib::str_t m_sName;
				Common::IPropertySetPtr  m_ConnectProp;
				typedef std::vector<IDatasetPtr> TVecDataset;
				TVecDataset m_vecDatasets;
				//TDatasetContainer m_DatasetContainer;



		};
	}
}
#endif