#ifndef GIS_ENGINE_GEO_DATABASE_FEATURE_H
#define GIS_ENGINE_GEO_DATABASE_FEATURE_H

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		class  Feature : public IFeature
		{
		public:
			Feature(IFieldSet* fieldSet, IFields* fields);
			virtual ~Feature();

		private:
			Feature(const Feature&);
			Feature& operator=(const Feature&);

		public:
			//IRow
			virtual IFieldSetPtr			   GetFieldSet() const = 0;
			virtual IFieldsPtr               GetSourceFields() const = 0;
			virtual bool                   IsFieldSelected(int index) const = 0;
			virtual CommonLib::IVariantPtr   GetValue(int index) const = 0;
			virtual void                   SetValue(int index, CommonLib::IVariant* value) = 0;
			virtual bool                   HasOID() const = 0;
			virtual int64                  GetOID() const = 0;
			virtual void                   SetOID(int64 id) = 0;

			// IFeature
			virtual CommonLib::IGeoShapePtr GetShape() const;
			virtual void                   SetShape(CommonLib::IGeoShape* pShape);
		private:
			IFieldSetPtr                        fieldSet_;
			IFieldsPtr                          fields_;
			int                                 oidFieldIndex_;
			int                                 shapeFieldIndex_;
			int                                 annoFieldIndex_;
			std::vector<int>                    fieldMap_;
			std::vector<CommonLib::IVariantPtr> values_;
		};
	}
}

#endif