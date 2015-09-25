#ifndef GIS_ENGINE_GEO_DATABASE_SQLITE_TABLE_H_
#define GIS_ENGINE_GEO_DATABASE_SQLITE_TABLE_H_

#include "TableBase.h"
 

namespace GisEngine
{
	namespace GeoDatabase
	{
		class CSQLiteWorkspace;
		class CSQLiteTable : public ITableBase<ITable>
		{
			public:
				typedef  ITableBase<ITable> TBase;
				CSQLiteTable(CSQLiteWorkspace *pWorkspace, const CommonLib::CString& sName,  const CommonLib::CString& sViewName);
				~CSQLiteTable();

				virtual IRowPtr		GetRow(int64 id);
				virtual ICursorPtr	Search(IQueryFilter* filter, bool recycling);
				bool load();

				virtual bool save(CommonLib::IWriteStream *pWriteStream) const;
				virtual bool load(CommonLib::IReadStream* pReadStream);

				virtual bool saveXML(GisCommon::IXMLNode* pXmlNode) const;
				virtual bool load(GisCommon::IXMLNode* pXmlNode);

			private:
				 CSQLiteWorkspace *m_pSQLiteWorkspace;
		};
	}
}

#endif