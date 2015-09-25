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

			IWorkspaceBase(eWorkspaceType type, uint32 nID) : m_WorkspaceType(type), m_nID(nID)
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
				assert(nIdx < (int)m_vecDatasets.size());
				m_DataSetMap.erase(m_vecDatasets[nIdx]->GetDatasetName());
				m_vecDatasets.erase(m_vecDatasets.begin() + nIdx);
				RebuildMap();
			}
			virtual void RemoveDataset(IDataset *pDataset)
			{
				CommonLib::CSSection::scoped_lock lock (m_mutex);

				TDatasetMap::const_iterator it = m_DataSetMap.find(pDataset->GetDatasetName());
				if(it == m_DataSetMap.end())
					return;
				assert(it->second < (int)m_vecDatasets.size());
				m_vecDatasets.erase(m_vecDatasets.begin() + it->second);
				m_DataSetMap.erase(it);
				RebuildMap();
			}


			virtual IDatasetPtr GetDataset(const CommonLib::CString& sName) const 
			{

				TDatasetMap::const_iterator it = m_DataSetMap.find(sName);
				if(it != m_DataSetMap.end())
				{
					assert(it->second < (int)m_vecDatasets.size());
					return m_vecDatasets[it->second];
				}
				return IDatasetPtr();
			}

			virtual void AddDataset(IDataset *pDataset)
			{
		 		m_vecDatasets.push_back(IDatasetPtr(pDataset));
				m_DataSetMap[pDataset->GetDatasetName()] = m_vecDatasets.size() - 1;
				
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

			virtual uint32 GetID() const
			{
				return m_nID;
			}
		protected:
			void RebuildMap()
			{
				m_DataSetMap.clear();
				for (size_t i = 0, sz = m_vecDatasets.size(); i < sz; ++i)
				{
					m_DataSetMap[m_vecDatasets[i]->GetDatasetName()] = i;
				}
			}


			typedef std::vector<IDatasetPtr> TVecDataset;
			TVecDataset m_vecDatasets;

			typedef std::map<CommonLib::CString, int> TDatasetMap;
			TDatasetMap	m_DataSetMap;

			eWorkspaceType m_WorkspaceType;
			CommonLib::CString m_sName;
			GisCommon::IPropertySetPtr  m_ConnectProp;
			mutable CommonLib::CSSection m_mutex;
			uint32 m_nID;
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
			typedef std::map<uint32, IWorkspacePtr> TWksMapByID;
	 


			static IWorkspacePtr GetWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sHash);
			static IWorkspacePtr GetWorkspace(uint32 nID);
			static void AddWorkspace(IWorkspace* pWks);
			static void RemoveWorkspace(eWorkspaceType WorkspaceType, const CommonLib::CString& sHash);
			static void RemoveWorkspace(uint32 nID);
			static uint32 GetIDWorkspace();
			static void SetLastID(uint32 nID);


			static bool SaveWks(CommonLib::IWriteStream *pStream);
			static bool LoadWks(CommonLib::IReadStream *pStream);

			static bool SaveWks(GisCommon::IXMLNode *pXML);
			static bool LoadWks(GisCommon::IXMLNode *pXML);

		private:
		

			static TWksMap m_wksMap;
			static TWksMapByID m_wksMapByID;
			static CommonLib::CSSection m_SharedMutex;
			static uint32 m_nWksID;
		};

	}
}

#endif