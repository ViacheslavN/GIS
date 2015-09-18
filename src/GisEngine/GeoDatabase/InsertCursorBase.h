#ifndef GIS_ENGINE_GEO_DATABASE_INSERT_CURSOR_BASE_H
#define GIS_ENGINE_GEO_DATABASE_INSERT_CURSOR_BASE_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template<class I>
		class IInsertCursorBase : public I
		{
		public:

			IInsertCursorBase(ITable* pTable, IFieldSet *pFileds)
			{
			  assert(pTable);
			  m_pTable = pTable;
			  m_pFieldSet = pFileds;
			  m_pSourceFields = m_pTable->GetFields();

			}

			virtual ~IInsertCursorBase(){}
			virtual IFieldSetPtr GetFieldSet() const
			{
			  return	m_pFieldSet;
			}
			virtual IFieldsPtr   GetSourceFields() const
			{
			  return m_pSourceFields;
			}

		
		protected:
			IFieldSetPtr  m_pFieldSet;
			IFieldsPtr m_pSourceFields;
			ITablePtr  m_pTable;
			
		};
	}
}

#endif