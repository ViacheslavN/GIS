#include "stdafx.h"
#include "Feature.h"
#include "GeoDatabase/FieldSet.h"
#include "CommonLibrary/BaseVariant.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		Feature::Feature(IFieldSet* fieldSet, IFields* fields)
			: fieldSet_(fieldSet)
			, fields_(fields)
			, oidFieldIndex_(-1)
			, shapeFieldIndex_(-1)
			, annoFieldIndex_(-1)
		{
			int fieldCount = fields->GetFieldCount();
			fieldMap_.resize(fieldCount, -1);

			if(!fieldSet_.get())
			{
				fieldSet_ = new CFieldSet();
				for(int i = 0; i < fieldCount; ++i)
					fieldSet_->Add(fields->GetField(i)->GetName());
			}

			CommonLib::str_t fieldName;
			fieldSet_->Reset();
			while(fieldSet_->Next(&fieldName))
			{
				if(fieldName == L"*")
				{
					fieldSet_->Clear();
					for(int i = 0; i < fieldCount; ++i)
						fieldSet_->Add(fields_->GetField(i)->GetName());
					fieldSet_->Reset();
					continue;
				}

				int fieldIndex = fields_->FindField(fieldName);
				CommonLib::eDataTypes fieldType = fields_->GetField(fieldIndex)->GetType();

				if(fieldType == CommonLib::dtOid && oidFieldIndex_ < 0)
					oidFieldIndex_ = fieldIndex;
				else if(fieldType == CommonLib::dtGeometry && (shapeFieldIndex_ < 0 || shapeFieldIndex_ > fieldIndex))
					shapeFieldIndex_ = fieldIndex;
				else if(fieldType == CommonLib::dtAnnotation && (annoFieldIndex_ < 0 || annoFieldIndex_ > fieldIndex))
					annoFieldIndex_ = fieldIndex;
			}


			/*for(int v = fieldSet_->GetCount(); v > 0;)
				values_[--v] = CommonLib::CreateBaseVariant(fieldSet_->)*/

			 values_.resize(fieldSet_->GetCount());
			fieldSet_->Reset();
			int i = 0;
			while(fieldSet_->Next(&fieldName))
			{
				int fieldIndex = fields_->FindField(fieldName);
				fieldMap_[fieldIndex] = i;
				CommonLib::eDataTypes fieldType = fields_->GetField(fieldIndex)->GetType();
				values_[i] = CommonLib::CreateBaseVariant(fieldType);
				++i;
			}
		}

		Feature::~Feature()
		{}

		// IRow
		IFieldSetPtr Feature::GetFieldSet() const
		{
			return fieldSet_;
		}

		IFieldsPtr Feature::GetSourceFields() const
		{
			return fields_;
		}

		bool Feature::IsFieldSelected(int index) const
		{
			return fieldMap_[index] >= 0;
		}

		CommonLib::IVariantPtr Feature::GetValue(int index) const
		{
			if(fieldMap_[index] < 0)
				CommonLib::IVariantPtr();

			return values_[fieldMap_[index]];
		}

		void Feature::SetValue(int index, CommonLib::IVariant* pValue)
		{
			if(fieldMap_[index] < 0)
				return;

			values_[fieldMap_[index]] = pValue;
		}

		// IRow
		bool Feature::HasOID() const
		{
			return oidFieldIndex_ >= 0;
		}

		int64 Feature::GetOID() const
		{
			if(oidFieldIndex_ < 0)
				return -1;

			int64 nOID = 0;
			values_[fieldMap_[oidFieldIndex_]]->getVal(nOID);
			return nOID;
		}

		void Feature::SetOID(int64 nOID)
		{
			if(oidFieldIndex_ < 0)
				return;

			CommonLib::IVariantPtr pVar = values_[fieldMap_[oidFieldIndex_]];
			pVar->set(nOID);
		}

		// IFeature
		CommonLib::IGeoShapePtr Feature::GetShape() const
		{
			if(shapeFieldIndex_ < 0 && annoFieldIndex_ < 0)
				CommonLib::IGeoShapePtr();

			CommonLib::IGeoShape *pShape;
			if(shapeFieldIndex_ < 0)
			{
				values_[fieldMap_[annoFieldIndex_]]->Get(pShape);
			}
			else
			{
				values_[fieldMap_[shapeFieldIndex_]]->Get(pShape);
			}

			return CommonLib::IGeoShapePtr(pShape);

			//  return GIS_VARIANT_GET(values_[fieldMap_[shapeFieldIndex_]]->Get(), gisCommon::IObjectPtr);
		}

		void Feature::SetShape(CommonLib::IGeoShape* pShape)
		{
			if(shapeFieldIndex_ < 0)
				return;

			IVariantEditPtr var = values_[fieldMap_[shapeFieldIndex_]];
			var->Get() = IObjectPtr(shape);
		}

	/*	bool Feature::HasAnnotation() const
		{
			return annoFieldIndex_ >= 0;
		}

		gisCommon::IObjectPtr Feature::GetAnnotation() const
		{
			if(annoFieldIndex_ < 0)
				GIS_THROW_DEFAULT_EXCEPTION(L"Cannot get Annotation");

			return GIS_VARIANT_GET(values_[fieldMap_[annoFieldIndex_]]->Get(), gisCommon::IObjectPtr);
		}

		void Feature::SetAnnotation(gisCommon::IObject* element)
		{
			if(annoFieldIndex_ < 0)
				GIS_THROW_DEFAULT_EXCEPTION(L"Cannot set Anotation. There is not found anno field");

			IVariantEditPtr var = values_[fieldMap_[annoFieldIndex_]];
			var->Get() = gisCommon::IObjectPtr(element);
		}*/

	}
}