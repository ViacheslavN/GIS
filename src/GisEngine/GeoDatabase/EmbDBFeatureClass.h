#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_FEATURE_CLASS_H_
#define GIS_ENGINE_GEO_DATABASE_EMBDB_FEATURE_CLASS_H_

#include "FeatureClassBase.h"
#include "../../EmbDB/EmbDB/embDB.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		
		class CEmbDBWorkspace;
		class CEmbDBFeatureClass : public IFeatureClassBase<IFeatureClass>
		{
		public:
			typedef  IFeatureClassBase<IFeatureClass> TBase;
			CEmbDBFeatureClass(CEmbDBWorkspace *pWorkspace, 
				const CommonLib::CString& sName, 
				const CommonLib::CString& sViewName);
			~CEmbDBFeatureClass();

			virtual IRowPtr		GetRow(int64 id);
			virtual ICursorPtr	Search(IQueryFilter* filter, bool recycling);

 
			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

			bool open();
			bool saveFeatureInfo();
			bool CreateFeatureClass(IFields* pFields);
			static bool IsFeatureClass(embDB::ITable* pTable, embDB::ISchema* pSchema);


		private:
			CEmbDBWorkspace *m_pEmbDBWorkspace;
			CommonLib::CString m_sPropTableName;
			eDataTypes m_nOIDType;
			bool m_bUseRowID;
		};
	}
}

#endif