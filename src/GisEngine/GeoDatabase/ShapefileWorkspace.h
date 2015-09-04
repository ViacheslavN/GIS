#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_

#include "GeoDatabase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CShapefileWorkspace : public IWorkspace
		{

			//typedef Common::CSimpleEnum<IDataset, IDatasetContainer> TDatasetContainer;

			CShapefileWorkspace(GisCommon::IPropertySetPtr& protSetPtr);
			CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath);

			public:


				static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath);
				static IWorkspacePtr Open(CommonLib::IReadStream* pSteram);
				static IWorkspacePtr Open(GisCommon::IXMLNode *pNode);

				static const wchar_t c_PropertyName[];
				static const wchar_t c_PropertyPath[];



				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);

				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(GisCommon::IXMLNode* pXmlNode);

		
				~CShapefileWorkspace();
			
				virtual const CommonLib::str_t& GetWorkspaceName() const; 
				virtual GisCommon::IPropertySetPtr  GetConnectionProperties() const; 
				virtual eWorkspaceID GetWorkspaceID() const;
				//virtual IDatasetContainer* GetDatasetContainer();

				virtual uint32 GetDatasetCount() const;
				virtual IDatasetPtr GetDataset(uint32 nIdx) const;
				virtual void RemoveDataset(uint32 nIdx);
				virtual void RemoveDataset(IDataset *pDataset);

				virtual ITablePtr  CreateTable(const CommonLib::str_t& name, IFields* fields);
				virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::str_t& name, IFields* fields, const CommonLib::str_t& shapeFieldName);
				virtual ITablePtr  OpenTable(const CommonLib::str_t& name);
				virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::str_t& name);


				virtual ITablePtr GetTable(const CommonLib::str_t& name);
				virtual IFeatureClassPtr GetFeatureClass(const CommonLib::str_t& name);




 			private:
				void load();
				void clear();
			private:

				typedef std::map<CommonLib::str_t, IWorkspacePtr> TWksMap;

				static TWksMap m_wksMap;
				static CommonLib::CSSection m_SharedMutex;


				IWorkspace *m_pWorkSpace;
				IFieldsPtr m_FieldsPtr;

				bool m_bLoad;
				CommonLib::str_t m_sPath;
				CommonLib::str_t m_sName;
				GisCommon::IPropertySetPtr  m_ConnectProp;


				typedef std::vector<IDatasetPtr> TVecDataset;
				TVecDataset m_vecDatasets;

				typedef std::map<CommonLib::str_t, IDataset*> TDatasetMap;
				TDatasetMap	m_DataSetMap;


				 mutable CommonLib::CSSection m_mutex;
				//TDatasetContainer m_DatasetContainer;



		};
	}
}
#endif