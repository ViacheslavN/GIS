#ifndef GIS_ENGINE_GEO_DATABASE_EMBDB_TABLE_H_
#define GIS_ENGINE_GEO_DATABASE_EMBDB_TABLE_H_

#include "TableBase.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
		class CEmbDBWorkspace;
		class CEmbDBTable : public ITableBase<ITable>
		{
		public:
			typedef  ITableBase<ITable> TBase;
			CEmbDBTable(CEmbDBWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName);
			~CEmbDBTable();

			virtual IRowPtr		GetRow(int64 id);
			virtual ICursorPtr	Search(IQueryFilter* filter, bool recycling);
			bool load();

			virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
			virtual bool load(CommonLib::IReadStream* pReadStream);

			virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
			virtual bool load(const GisCommon::IXMLNode* pXmlNode);

			bool open();
			bool CreateTable(IFields* pFields);

		private:
			CEmbDBWorkspace *m_pEmbDBWorkspace;
		};
	}
}

#endif
