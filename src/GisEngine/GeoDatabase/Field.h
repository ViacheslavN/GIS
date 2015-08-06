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
				CField(CommonLib::str_t& sName, CommonLib::str_t& sAliasName, bool bIsEditable,	bool bIsNullable, bool bIsRquired, eDataTypes type,
					int nLength, int nPrecision, int nScale, IGeometryDef* pGeometry, IDomain* pDomain);
				CField();
				virtual ~CField();
				CField(const CField&);
				CField& operator=(const CField&);

	
				// IField
				virtual const CommonLib::str_t& GetName() const;
				virtual void                 SetName( const CommonLib::str_t& sName);
				virtual  const CommonLib::str_t&  GetAliasName() const;
				virtual void                 SetAliasName(const   CommonLib::str_t&  sAliasName);
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

				// IShapeField
				virtual IGeometryDefPtr  GetGeometryDef() const;
				virtual void            SetGeometryDef(IGeometryDef* pGeometryDef);
			private:
				void copy(const CField *pField);
			private:
				CommonLib::str_t		m_sName;
				CommonLib::str_t		m_sAliasName;
				bool					m_bIsEditable;
				bool					m_bIsNullable;
				bool					m_bIsRquired;
				eDataTypes				m_type;
				int						m_nLength;
				int						m_nPrecision;
				int						m_nScale;
				IGeometryDefPtr			m_pGeometryDefPtr;
				IDomainPtr				m_pDomainPtr;



		};
	}
}

#endif