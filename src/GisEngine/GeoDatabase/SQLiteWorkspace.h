#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_

#include "GeoDatabase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"


struct sqlite3;
struct sqlite3_stmt;

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CSQLiteWorkspace : public IWorkspace
		{
		public:

			CSQLiteWorkspace();
			CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath);
			~CSQLiteWorkspace();

			static IWorkspacePtr Create(const wchar_t *pszName, const wchar_t *pszPath);
			static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite);
			static IWorkspacePtr Open(CommonLib::IReadStream* pSteram);
			static IWorkspacePtr Open(GisCommon::IXMLNode *pNode);

			virtual const CommonLib::CString& GetWorkspaceName() const; 
			virtual GisCommon::IPropertySetPtr GetConnectionProperties() const; 
			virtual eWorkspaceID GetWorkspaceID() const;
 
			virtual uint32 GetDatasetCount() const;
			virtual IDatasetPtr GetDataset(uint32 nIdx) const;
			virtual void RemoveDataset(uint32 nIdx);
			virtual void RemoveDataset(IDataset *pDataset);

			virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* fields);
			virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& shapeFieldName);

			virtual ITablePtr OpenTable(const CommonLib::CString& name);
			virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::CString& name);

			virtual ITablePtr GetTable(const CommonLib::CString& name);
			virtual IFeatureClassPtr GetFeatureClass(const CommonLib::CString& name);

			virtual bool IsError() const {return false;}
			virtual uint32 GetErrorCode() const {return 0;}
			virtual void GetErrorText( CommonLib::CString& sStr, uint32 nCode) {}


			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);

			virtual	ITransactionPtr startTransaction();
		
		private:
			bool create(const CommonLib::CString& sFullName);
			bool load(const CommonLib::CString& sFullName, bool bWrite);
			void close();
		private:

			typedef std::vector<IDatasetPtr> TVecDataset;
			TVecDataset m_vecDatasets;

			typedef std::map<CommonLib::CString, IDataset*> TDatasetMap;
			TDatasetMap	m_DataSetMap;

			typedef std::map<CommonLib::CString, IWorkspacePtr> TWksMap;

			static TWksMap m_wksMap;
			static CommonLib::CSSection m_SharedMutex;

			CommonLib::CString m_sName;
			CommonLib::CString m_sPath;

			sqlite3*		m_pConn;
			sqlite3_stmt*	m_precordSet;
		};
	}
}

#endif