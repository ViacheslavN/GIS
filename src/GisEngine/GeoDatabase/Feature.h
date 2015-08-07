#ifndef GIS_ENGINE_GEO_DATABASE_FEATURE_H
#define GIS_ENGINE_GEO_DATABASE_FEATURE_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		class  CFeature : public IFeature
		{
		public:
			CFeature(IFieldSet* fieldSet, IFields* fields);
			virtual ~CFeature();

		private:
			CFeature(const CFeature&);
			CFeature& operator=(const CFeature&);

		public:
			//IRow
			virtual IFieldSetPtr		   GetFieldSet() const;
			virtual IFieldsPtr             GetSourceFields() const ;
			virtual bool                   IsFieldSelected(int index) const;
			virtual const CommonLib::CVariant*   GetValue(int index) const;
			virtual void                   SetValue(int index, const CommonLib::CVariant& value);
			virtual bool                   HasOID() const;
			virtual int64                  GetOID() const;
			virtual void                   SetOID(int64 id);

			// IFeature
			virtual CommonLib::IGeoShapePtr GetShape() const;
			virtual void                   SetShape(CommonLib::IGeoShape* pShape);
		private:
			IFieldSetPtr                        m_pFieldSet;
			IFieldsPtr                          m_pFields;
			int                                 m_nOidFieldIndex;
			int                                 m_nShapeFieldIndex;
			int                                 m_nAnnoFieldIndex;
			std::vector<int>                    m_vecFieldMap;
			std::vector<CommonLib::CVariant>	m_vecValues;
		};
	}
}

#endif