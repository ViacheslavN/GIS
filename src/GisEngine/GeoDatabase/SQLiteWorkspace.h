#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_

#include "WorkspaceBase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		namespace SQLiteUtils
		{
			class CSQLiteDB;
		}
		class CSQLiteWorkspace : public IWorkspaceBase<IWorkspace>
		{
	
			typedef IWorkspaceBase<IWorkspace> TBase;
			CSQLiteWorkspace(int32 ID);
			CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath, int32 ID);
			~CSQLiteWorkspace();
	public:
			static IWorkspacePtr Create(const wchar_t *pszName, const wchar_t *pszPath);
			static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite, bool bOpenAll = false);
			static IWorkspacePtr Open(CommonLib::IReadStream* pSteram, bool bOpenAll = false);
			static IWorkspacePtr Open(GisCommon::IXMLNode *pNode, bool bOpenAll = false);
 

			virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* pFields);

			virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name, IFields* pFields, bool bSaveFCProp = false);

			virtual ITablePtr OpenTable(const CommonLib::CString& name);
			virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::CString& name);

			
			virtual bool IsError() const;
			virtual uint32 GetErrorCode() const;
			virtual void GetErrorText( CommonLib::CString& sStr, uint32 nCode); 
			

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

			virtual	ITransactionPtr startTransaction(eTransactionType type);

	

			SQLiteUtils::CSQLiteDB *GetDB(){return m_pDB.get();}
		private:
			bool create(const CommonLib::CString& sFullName);
			bool load(const CommonLib::CString& sFullName, bool bWrite, bool bOpenAll = true);
			void close();
			bool IsConnect() const;
			virtual const CommonLib::CString& GetHash() const{return m_sHash;};
		private:
  
			CommonLib::CString m_sPath;
			CommonLib::CString m_sHash;
			std::auto_ptr<SQLiteUtils::CSQLiteDB> m_pDB;
		};
	}
}

#endif