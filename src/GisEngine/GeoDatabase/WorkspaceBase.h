#ifndef GIS_ENGINE_GEO_DATABASE_WORKSPACE_BASE_H
#define GIS_ENGINE_GEO_DATABASE_WORKSPACE_BASE_H
#include "GeoDatabase.h"
#include "CommonLibrary/CSSection.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template<class I>
		class IWorkspaceBase : public I
		{
		public:

			IWorkspaceBase(eWorkspaceType type) : m_WorkspaceType(type)
			{

			}
			virtual ~IWorkspaceBase(){}




			virtual const CommonLib::CString& GetWorkspaceName() const
			{
				return m_sName;
			}

			virtual eWorkspaceType GetWorkspaceType() const
			{
				return m_WorkspaceType;
			}

				
			virtual GisCommon::IPropertySetPtr  GetConnectionProperties() const
			{
				return m_ConnectProp;
			}
		
			virtual uint32 GetDatasetCount() const
			{
				CommonLib::CSSection::scoped_lock lock (m_mutex);
				return m_vecDatasets.size();
			}
			virtual IDatasetPtr GetDataset(uint32 nIdx) const
			{
				CommonLib::CSSection::scoped_lock lock (m_mutex);
				return m_vecDatasets[nIdx];
			}
			virtual void RemoveDataset(uint32 nIdx)
			{
				CommonLib::CSSection::scoped_lock lock (m_mutex);
				assert(nIdx < m_vecDatasets.size());
				m_DataSetMap.erase(m_vecDatasets[nIdx]->GetDatasetName());
				m_vecDatasets.erase(m_vecDatasets.begin() + nIdx);
			}
			virtual void RemoveDataset(IDataset *pDataset)
			{
				CommonLib::CSSection::scoped_lock lock (m_mutex);
				TVecDataset::iterator it = std::find(m_vecDatasets.begin(), m_vecDatasets.end(), pDataset);
				if(it != m_vecDatasets.end())
				{
					m_DataSetMap.erase((*it)->GetDatasetName());
					m_vecDatasets.erase(it);
				}
			}


			virtual IDatasetPtr GetDataset(const CommonLib::CString& sName) const 
			{

				TDatasetMap::const_iterator it = m_DataSetMap.find(sName);
				if(it != m_DataSetMap.end())
					return it->second;
				return IDatasetPtr();
			}

			virtual ITablePtr GetTable(const CommonLib::CString& sName)
			{
				IDatasetPtr pDataset = GetDataset(sName);
				ITable *pTable = (ITable*)pDataset.get();
				return ITablePtr(pTable);
			}
			virtual IFeatureClassPtr GetFeatureClass(const CommonLib::CString& sName)
			{
				IDatasetPtr pDataset = GetDataset(sName);
				IFeatureClass *pFeatureClass  = (IFeatureClass*)pDataset.get();
				return IFeatureClassPtr(pFeatureClass);
			}
		protected:
			
			typedef std::vector<IDatasetPtr> TVecDataset;
			TVecDataset m_vecDatasets;

			typedef std::map<CommonLib::CString, IDatasetPtr> TDatasetMap;
			TDatasetMap	m_DataSetMap;

			eWorkspaceType m_WorkspaceType;
			CommonLib::CString m_sName;
			GisCommon::IPropertySetPtr  m_ConnectProp;
			mutable CommonLib::CSSection m_mutex;
		};


		class CWorkspaceHolder
		{
		public:
			CWorkspaceHolder(){}
			~CWorkspaceHolder(){}

			struct SWksInfo
			{
				SWksInfo(eWorkspaceType _eWksType, const CommonLib::CString& sUID) : m_eWksType(_eWksType),
					m_sUID(sUID)
				{}

				bool operator < (const SWksInfo& wks) const
				{
					if(m_eWksType != wks.m_eWksType)
						return m_eWksType < wks.m_eWksType;
					return m_sUID < wks.m_sUID;
				}

				eWorkspaceType m_eWksType;
				CommonLib::CString	m_sUID;
			};
			typedef std::map<SWksInfo, IWorkspacePtr> TWksMap;

			static TWksMap m_wksMap;
			static CommonLib::CSSection m_SharedMutex;

			static IWorkspacePtr GetWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sID);
			static void AddWorkspace(IWorkspace* pWks, const CommonLib::CString& sID);
			static void RemoveWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sID);
		};

	}
}

#endif