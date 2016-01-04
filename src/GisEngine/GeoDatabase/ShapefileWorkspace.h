#ifndef GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_
#define GIS_ENGINE_GEO_DATABASE_SHAPE_WORK_SPACE_H_

#include "GeoDatabase.h"
#include "Common/SimpleEnum.h"
#include "Common/GisEngineCommon.h"
#include "CommonLibrary/CSSection.h"
#include "WorkspaceBase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		
		class CShapefileWorkspace : public IWorkspaceBase<IWorkspace>
		{

			//typedef Common::CSimpleEnum<IDataset, IDatasetContainer> TDatasetContainer;
			typedef IWorkspaceBase<IWorkspace> TBase;
			CShapefileWorkspace(GisCommon::IPropertySetPtr& protSetPtr, int32 nID);
			CShapefileWorkspace(const wchar_t *pszName, const wchar_t *pszPath, int32 nID);

			public:


				static IWorkspacePtr Open(const wchar_t *pszName, const wchar_t *pszPath);
				static IWorkspacePtr Open(CommonLib::IReadStream* pSteram);
				static IWorkspacePtr Open(GisCommon::IXMLNode *pNode);

				static const wchar_t c_PropertyName[];
				static const wchar_t c_PropertyPath[];


				virtual ITablePtr CShapefileWorkspace::GetTable(const CommonLib::CString& name);

				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);

				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(const GisCommon::IXMLNode* pXmlNode);

		
				~CShapefileWorkspace();
			
				
				virtual ITablePtr  CreateTable(const CommonLib::CString& name, IFields* fields);
				virtual IFeatureClassPtr CreateFeatureClass(const CommonLib::CString& name,	IFields* fields, bool bSaveFCProp = false);
				virtual ITablePtr  OpenTable(const CommonLib::CString& name);
				virtual IFeatureClassPtr OpenFeatureClass(const CommonLib::CString& name);



				virtual bool IsError() const {return false;}
				virtual uint32 GetErrorCode() const {return 0;}
				virtual void GetErrorText( CommonLib::CString& sStr, uint32 nCode) {}
				virtual ITransactionPtr startTransaction(eTransactionType type);
				virtual const CommonLib::CString& GetHash() const {return m_sPath;};
 			private:
				void load();
				void clear();
			private:
				
				bool m_bLoad;
				CommonLib::CString m_sPath;
  		
				//TDatasetContainer m_DatasetContainer;



		};
	}
}
#endif