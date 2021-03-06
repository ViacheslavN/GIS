#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_WORKSPACE_H_
#define GIS_ENGINE_GEO_DATABASE_EMBDB_WORKSPACE_H_

#include "WorkspaceBase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"
#include "../../EmbDB/EmbDB/embDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		
		class CEmbDBWorkspace : public IWorkspaceBase<IWorkspace>
		{

			typedef IWorkspaceBase<IWorkspace> TBase;
			CEmbDBWorkspace(int32 ID);
			CEmbDBWorkspace(const wchar_t *pszName, const wchar_t *pszPath, int32 ID);
			~CEmbDBWorkspace();
		public:
			static IWorkspacePtr Create(const wchar_t *pszName, const wchar_t *pszPath, const wchar_t *pszPWD = NULL);
			static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath, bool bWrite, bool bOpenAll = false, const wchar_t *pszPWD = NULL);
			static IWorkspacePtr Open(CommonLib::IReadStream* pSteram, const wchar_t *pszPWD = NULL, bool bOpenAll = false);
			static IWorkspacePtr Open(GisCommon::IXMLNode *pNode, const wchar_t *pszPWD = NULL, bool bOpenAll = false);


			virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* fields);

			virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name,
				IFields* fields, bool bSaveFCProp = false);

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



			embDB::IDatabasePtr GetDB(){return m_pDB;}
			embDB::IConnectionPtr GetConnection() {return m_pConnection;}
		private:
 
			virtual const CommonLib::CString& GetHash() const{return m_sHash;};

			void close();
			bool create(const CommonLib::CString& sFullName, const CommonLib::CString& sPWD);
			bool load(const CommonLib::CString& sFullName, const CommonLib::CString& sPWD, bool bWrite, bool bOpenAll = true);
		private:

			CommonLib::CString m_sPath;
			CommonLib::CString m_sHash;
			embDB::IDatabasePtr m_pDB;
			embDB::IConnectionPtr m_pConnection;
		};
	
	}
}

#endif