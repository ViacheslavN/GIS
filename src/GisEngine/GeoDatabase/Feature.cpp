#include "stdafx.h"
#include "Feature.h"
#include "GeoDatabase/FieldSet.h"
#include "CommonLibrary/Variant.h"
namespace GisEngine
{
	namespace GeoDatabase
	{

		CFeature::CFeature(IFieldSet* fieldSet, IFields* fields)
			: m_pFieldSet(fieldSet)
			, m_pFields(fields)
			, m_nOidFieldIndex(-1)
			, m_nShapeFieldIndex(-1)
			, m_nAnnoFieldIndex(-1)
		{
			int fieldCount = fields->GetFieldCount();
			m_vecFieldMap.resize(fieldCount, -1);

			if(!m_pFieldSet.get())
			{
				m_pFieldSet = new CFieldSet();
				for(int i = 0; i < fieldCount; ++i)
					m_pFieldSet->Add(fields->GetField(i)->GetName());
			}

			CommonLib::CString fieldName;
			m_pFieldSet->Reset();
			while(m_pFieldSet->Next(&fieldName))
			{
				if(fieldName == L"*")
				{
					m_pFieldSet->Clear();
					for(int i = 0; i < fieldCount; ++i)
						m_pFieldSet->Add(m_pFields->GetField(i)->GetName());
					m_pFieldSet->Reset();
					continue;
				}

				int fieldIndex = m_pFields->FindField(fieldName);
				eDataTypes fieldType = m_pFields->GetField(fieldIndex)->GetType();

				if((fieldType == dtOid32 || fieldType == dtOid64) && m_nOidFieldIndex < 0)
					m_nOidFieldIndex = fieldIndex;
				else if(fieldType == dtGeometry && (m_nShapeFieldIndex < 0 || m_nShapeFieldIndex > fieldIndex))
					m_nShapeFieldIndex = fieldIndex;
				else if(fieldType == dtAnnotation && (m_nAnnoFieldIndex < 0 || m_nAnnoFieldIndex > fieldIndex))
					m_nAnnoFieldIndex = fieldIndex;
			}


			/*for(int v = fieldSet_->GetCount(); v > 0;)
				values_[--v] = CommonLib::CreateBaseVariant(fieldSet_->)*/

			 m_vecValues.resize(m_pFieldSet->GetCount());
			m_pFieldSet->Reset();
			int i = 0;
			while(m_pFieldSet->Next(&fieldName))
			{
				int fieldIndex = m_pFields->FindField(fieldName);
				m_vecFieldMap[fieldIndex] = i;
				m_vecValues[i] = CommonLib::CVariant();
				++i;
			}
		}

		CFeature::~CFeature()
		{}

		// IRow
		IFieldSetPtr CFeature::GetFieldSet() const
		{
			return m_pFieldSet;
		}

		IFieldsPtr CFeature::GetSourceFields() const
		{
			return m_pFields;
		}

		bool CFeature::IsFieldSelected(int index) const
		{
			return m_vecFieldMap[index] >= 0;
		}

		const CommonLib::CVariant* CFeature::GetValue(int index) const
		{
			if(m_vecFieldMap[index] < 0)
				return NULL;

			return &m_vecValues[m_vecFieldMap[index]];
		}

		CommonLib::CVariant* CFeature::GetValue(int index)
		{
			if(m_vecFieldMap[index] < 0)
				return NULL;

			return &m_vecValues[m_vecFieldMap[index]];
		}

		void CFeature::SetValue(int index, const CommonLib::CVariant& value)
		{
			if(m_vecFieldMap[index] < 0)
				return;

			m_vecValues[m_vecFieldMap[index]] = value;
		}

		// IRow
		bool CFeature::HasOID() const
		{
			return m_nOidFieldIndex >= 0;
		}

		int64 CFeature::GetOID() const
		{
			if(m_nOidFieldIndex < 0)
				return -1;

			int64 nOID = 0;
			//return m_vecValues[m_vecFieldMap[m_nOidFieldIndex]].Get<int64>();
			const CommonLib::CVariant& var = m_vecValues[m_vecFieldMap[m_nOidFieldIndex]]; 
			if(var.isType<int64>())
				nOID = var.Get<int64>();
			else
				nOID = var.Get<int32>();
			return nOID;
		}

		void CFeature::SetOID(int64 nOID)
		{
			if(m_nOidFieldIndex < 0)
				return;

			CommonLib::CVariant& var = m_vecValues[m_vecFieldMap[m_nOidFieldIndex]];
			if(var.isType<int64>())
				var = nOID;
			else
				var = (uint32)nOID;
			//m_vecValues[m_vecFieldMap[m_nOidFieldIndex]] = nOID;
		}

		// IFeature
		CommonLib::IGeoShapePtr CFeature::GetShape() const
		{
			if(m_nShapeFieldIndex < 0 && m_nAnnoFieldIndex < 0)
				CommonLib::IGeoShapePtr();

			return m_pShape;

			CommonLib::IRefObjectPtr ptr;
			if(m_nShapeFieldIndex < 0)
			{
				ptr = m_vecValues[m_vecFieldMap[m_nAnnoFieldIndex]].Get<CommonLib::IRefObjectPtr>();
			}
			else
			{
				ptr = m_vecValues[m_vecFieldMap[m_nShapeFieldIndex]].Get<CommonLib::IRefObjectPtr>();
			}

			return CommonLib::IGeoShapePtr((CommonLib::CGeoShape*)ptr.get());

		}

		void CFeature::SetShape(CommonLib::CGeoShape* pShape)
		{
			if(m_nShapeFieldIndex < 0)
				return;

			m_pShape = pShape;
			return;

			CommonLib::IRefObjectPtr ptr((IRefCnt*)pShape);

			m_vecValues[m_vecFieldMap[m_nShapeFieldIndex]] = ptr;
		}
	}
}