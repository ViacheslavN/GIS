#ifndef GIS_ENGINE_GEO_DATABASE_FIELD_H_
#define GIS_ENGINE_GEO_DATABASE_FIELD_H_

#include "GeoDatabase.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		class CField : public IShapeField
		{
			public:
				CField(CommonLib::CString& sName, CommonLib::CString& sAliasName, bool bIsEditable,	bool bIsNullable, bool bIsRquired, eDataTypes type,
					int nLength, int nPrecision, int nScale, IGeometryDef* pGeometry, IDomain* pDomain, bool bIsPrimaryKey);
				CField();
				virtual ~CField();
				CField(const CField&);
				CField& operator=(const CField&);

	
				// IField
				virtual const CommonLib::CString& GetName() const;
				virtual void                 SetName( const CommonLib::CString& sName);
				virtual  const CommonLib::CString&  GetAliasName() const;
				virtual void                 SetAliasName(const   CommonLib::CString&  sAliasName);
				virtual bool                 GetIsEditable() const;
				virtual void                 SetIsEditable(bool bIsEditable);
				virtual bool                 GetIsNullable() const;
				virtual void                 SetIsNullable(bool bIsNullable);
				virtual bool                 GetIsRequired() const;
				virtual void                 SetIsRequired(bool bIsRequired);
				virtual eDataTypes			 GetType() const;
				virtual void                 SetType(eDataTypes type);
				virtual int                  GetLength() const;
				virtual void                 SetLength(int length);
				virtual int                  GetPrecision() const;
				virtual void                 SetPrecision(int precision);
				virtual int                  GetScale() const;
				virtual void                 SetScale(int scale);
				virtual IDomainPtr           GetDomain() const ;
				virtual void                 SetDomain(IDomain* pDomain);
				virtual const CommonLib::CVariant& 	 GetDefaultValue() const;
				virtual void					 SetIsDefault(const CommonLib::CVariant& value);
				virtual bool                 GetIsPrimaryKey() const;
				virtual void                 SetIsPrimaryKey(bool flag);

				// IShapeField
				virtual IGeometryDefPtr  GetGeometryDef() const;
				virtual void            SetGeometryDef(IGeometryDef* pGeometryDef);
			private:
				void copy(const CField *pField);
			private:
				CommonLib::CString		m_sName;
				CommonLib::CString		m_sAliasName;
				bool					m_bIsEditable;
				bool					m_bIsNullable;
				bool					m_bIsRquired;
				eDataTypes				m_type;
				int						m_nLength;
				int						m_nPrecision;
				int						m_nScale;
				IGeometryDefPtr			m_pGeometryDefPtr;
				IDomainPtr				m_pDomainPtr;
				CommonLib::CVariant		m_DefValue;
				bool					m_bIsPrimaryKey;




		};
	}
}

#endif