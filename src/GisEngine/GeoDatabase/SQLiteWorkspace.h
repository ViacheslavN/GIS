#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_WORKSPACE_H_

#include "WorkspaceBase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"


struct sqlite3;
struct sqlite3_stmt;

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CSQLiteWorkspace : public IWorkspaceBase<IWorkspace>
		{
		public:
			typedef IWorkspaceBase<IWorkspace> TBase;
			CSQLiteWorkspace();
			CSQLiteWorkspace(const wchar_t *pszName, const wchar_t *pszPath);
			~CSQLiteWorkspace();

			static IWorkspacePtr Create(const wchar_t *pszName, const wchar_t *pszPath);
			static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite);
			static IWorkspacePtr Open(CommonLib::IReadStream* pSteram);
			static IWorkspacePtr Open(GisCommon::IXMLNode *pNode);
 

			virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* fields);
			virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name, IFields* fields, const CommonLib::CString& shapeFieldName);

			virtual ITablePtr OpenTable(const CommonLib::CString& name);
			virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::CString& name);

			
			virtual bool IsError() const {return m_bError;}
			virtual uint32 GetErrorCode() const {return 0;}
			virtual void GetErrorText( CommonLib::CString& sStr, uint32 nCode) 
			{
				sStr = m_sErrorMessage;
			}


			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(GisCommon::IXMLNode* pXmlNode);

			virtual	ITransactionPtr startTransaction();
			sqlite3 *GetConnections(){return m_pConn;}
		private:
			bool create(const CommonLib::CString& sFullName);
			bool load(const CommonLib::CString& sFullName, bool bWrite);
			void close();
			bool IsConnect() const;
		private:
  
			CommonLib::CString m_sPath;

			sqlite3*		m_pConn;
			sqlite3_stmt*	m_precordSet;
			CommonLib::CString m_sErrorMessage;
			bool m_bError;
		};
	}
}

#endif