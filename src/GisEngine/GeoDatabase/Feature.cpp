#include "stdafx.h"
#include "Feature.h"
#include "GeoDatabase/FieldSet.h"
#include "CommonLibrary/Variant.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		CFeature::CFeature(IFieldSet* fieldSet, IFields* fields)
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
				eDataTypes fieldType = fields_->GetField(fieldIndex)->GetType();

				if(fieldType == dtOid && oidFieldIndex_ < 0)
					oidFieldIndex_ = fieldIndex;
				else if(fieldType == dtGeometry && (shapeFieldIndex_ < 0 || shapeFieldIndex_ > fieldIndex))
					shapeFieldIndex_ = fieldIndex;
				else if(fieldType == dtAnnotation && (annoFieldIndex_ < 0 || annoFieldIndex_ > fieldIndex))
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
				values_[i] = CommonLib::CVariant();
				++i;
			}
		}

		CFeature::~CFeature()
		{}

		// IRow
		IFieldSetPtr CFeature::GetFieldSet() const
		{
			return fieldSet_;
		}

		IFieldsPtr CFeature::GetSourceFields() const
		{
			return fields_;
		}

		bool CFeature::IsFieldSelected(int index) const
		{
			return fieldMap_[index] >= 0;
		}

		const CommonLib::CVariant* CFeature::GetValue(int index) const
		{
			if(fieldMap_[index] < 0)
				return NULL;

			return &values_[fieldMap_[index]];
		}

		void CFeature::SetValue(int index, const CommonLib::CVariant& value)
		{
			if(fieldMap_[index] < 0)
				return;

			values_[fieldMap_[index]] = value;
		}

		// IRow
		bool CFeature::HasOID() const
		{
			return oidFieldIndex_ >= 0;
		}

		int64 CFeature::GetOID() const
		{
			if(oidFieldIndex_ < 0)
				return -1;

			int64 nOID = 0;
			return values_[fieldMap_[oidFieldIndex_]].Get<int64>();
			 
		}

		void CFeature::SetOID(int64 nOID)
		{
			if(oidFieldIndex_ < 0)
				return;

			values_[fieldMap_[oidFieldIndex_]] = nOID;
		}

		// IFeature
		CommonLib::IGeoShapePtr CFeature::GetShape() const
		{
			if(shapeFieldIndex_ < 0 && annoFieldIndex_ < 0)
				CommonLib::IGeoShapePtr();

			CommonLib::IRefObjectPtr ptr;
			if(shapeFieldIndex_ < 0)
			{
				ptr = values_[fieldMap_[annoFieldIndex_]].Get<CommonLib::IRefObjectPtr>();
			}
			else
			{
				ptr = values_[fieldMap_[shapeFieldIndex_]].Get<CommonLib::IRefObjectPtr>();
			}

			return CommonLib::IGeoShapePtr((CommonLib::IGeoShape*)ptr.get());

		}

		void CFeature::SetShape(CommonLib::IGeoShape* pShape)
		{
			if(shapeFieldIndex_ < 0)
				return;

			CommonLib::IRefObjectPtr ptr((IRefCnt*)pShape);

			values_[fieldMap_[shapeFieldIndex_]] = ptr;
		}
	}
}