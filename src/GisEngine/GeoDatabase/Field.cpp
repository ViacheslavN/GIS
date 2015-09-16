#include "stdafx.h"
#include "Field.h"

namespace GisEngine
{
	namespace GeoDatabase
	{
		CField::CField(CommonLib::CString& sName, CommonLib::CString& sAliasName, bool bIsEditable,	bool bIsNullable, bool bIsRquired, eDataTypes type,
			int nLength, int nPrecision, int nScale, IGeometryDef* pGeometry, IDomain* pDomain, bool bIsPrimaryKey) : 
			m_sName(sName), m_sAliasName(sAliasName), m_bIsEditable(bIsEditable), m_bIsNullable(bIsNullable), m_bIsRquired(bIsRquired), m_type(type),
				m_nLength(nLength), m_nPrecision(nPrecision), m_nScale(nScale), m_pGeometryDefPtr(pGeometry), m_pDomainPtr(pDomain), m_bIsPrimaryKey(bIsPrimaryKey)
		{

		}
		CField::CField() : m_bIsEditable(true), m_bIsNullable(true), m_bIsRquired(false), m_type(dtUnknown), m_nLength(0), m_nPrecision(0), m_nScale(0), m_bIsPrimaryKey(false)
			
		{

		}
		CField::~CField()
		{

		}
		CField::CField(const CField& field)
		{
			copy(&field);
		}
		CField& CField::operator=(const CField& field)
		{
			copy(&field);
			return *this;
		}

		void CField::copy(const CField *pField)
		{
			if(!pField)
				return;

			SetName(pField->GetName());
			SetAliasName(pField->GetAliasName());
			SetIsEditable(pField->GetIsEditable());
			SetIsNullable(pField->GetIsNullable());
			SetIsRequired(pField->GetIsRequired());
			SetType(pField->GetType());
			SetLength(pField->GetLength());
			SetPrecision(pField->GetPrecision());
			SetScale(pField->GetScale());
			SetDomain(pField->GetDomain().get());
			SetGeometryDef(pField->GetGeometryDef().get());

		}

		const CommonLib::CString& CField::GetName() const
		{
			return m_sName;
		}
		void   CField::SetName( const CommonLib::CString& sName)
		{
			m_sName = sName;
		}
		const CommonLib::CString&  CField::GetAliasName() const
		{
			return m_sAliasName;
		}
		void  CField::SetAliasName(const   CommonLib::CString&  sAliasName)
		{
			m_sAliasName = sAliasName;
		}
		bool  CField::GetIsEditable() const
		{
			return m_bIsEditable;
		}
		void  CField::SetIsEditable(bool bIsEditable)
		{
			m_bIsEditable = bIsEditable;
		}
		bool  CField::GetIsNullable() const
		{
			return m_bIsNullable;
		}
		void  CField::SetIsNullable(bool bIsNullable)
		{
			m_bIsNullable = bIsNullable;
		}
		bool  CField::GetIsRequired() const
		{
			return m_bIsRquired;
		}
		void  CField::SetIsRequired(bool bIsRquired)
		{
			m_bIsRquired = bIsRquired;
		}
		eDataTypes  CField::GetType() const
		{
			return m_type;
		}
		void  CField::SetType(eDataTypes type)
		{
			m_type = type;
		}
		int  CField::GetLength() const
		{
			return m_nLength;
		}
		void  CField::SetLength(int length)
		{
			m_nLength = length;
		}
		int        CField::GetPrecision() const
		{
			return m_nPrecision;
		}
		void  CField::SetPrecision(int precision)
		{
			m_nPrecision = precision;
		}
		int  CField::GetScale() const
		{
			return m_nScale;
		}
		void CField::SetScale(int scale)
		{
			m_nScale = scale;
		}
		
		IDomainPtr CField::GetDomain() const
		{
			return m_pDomainPtr;
		}
		void  CField::SetDomain(IDomain* pDomain)
		{
			m_pDomainPtr = pDomain;
		}

		// IShapeField
		IGeometryDefPtr  CField::GetGeometryDef() const
		{
			return m_pGeometryDefPtr;
		}
		void  CField::SetGeometryDef(IGeometryDef* pGeometryDef)
		{
			m_pGeometryDefPtr = pGeometryDef;
		}
		const CommonLib::CVariant& CField::GetDefaultValue() const
		{
			return m_DefValue;
		}
		void  CField::SetIsDefault(const CommonLib::CVariant& value)
		{
			m_DefValue = value;
		}

		bool  CField::GetIsPrimaryKey() const
		{
			return m_bIsPrimaryKey;
		}
		void   CField::SetIsPrimaryKey(bool bFlag)
		{
			m_bIsPrimaryKey = bFlag;
		}
	}
}