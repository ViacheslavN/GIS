#ifndef GIS_ENGINE_GEO_DATABASE_TABLE_BASE_H
#define GIS_ENGINE_GEO_DATABASE_TABLE_BASE_H

#include "DatasetBase.h"
#include "Fields.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		template< class I>
		class ITableBase : public IDataSetBase<I>
		{
		public:
			typedef IDataSetBase<I> TBase;
			ITableBase(IWorkspace *pWks) : TBase(pWks)
			{
				m_DatasetType = dtTypeTable;
				m_pFields = new CFields();
				m_bHashOID = false;
			}

			virtual ~ITableBase()
			{

			}
			virtual void AddField(IField* pField)
			{
				m_pFields->AddField(pField);
			}
			virtual void  DeleteField(const CommonLib::CString& fieldName)
			{
				int nIndex = m_pFields->FindField(fieldName);
				if(nIndex != -1)
					m_pFields->RemoveField(nIndex);
			}
			virtual IFieldsPtr	GetFields() const 
			{
				return m_pFields;
			}
			virtual void	SetFields(IFields *pFields) 
			{
				m_pFields = pFields;
			}
			void SetOIDFieldName(const CommonLib::CString& sOIDFieldName)
			{
				m_sOIDFieldName = sOIDFieldName;
			}
			virtual bool   HasOIDField() const
			{
				return m_bHashOID;
			}
			virtual void   SetHasOIDField(bool bFlag)
			{
				 m_bHashOID = bFlag;
			}
			virtual const CommonLib::CString&	 GetOIDFieldName() const
			{
				return m_sOIDFieldName;
			}
		protected:
			mutable IFieldsPtr           m_pFields;
			mutable CommonLib::CString m_sOIDFieldName;
			bool					   m_bHashOID;
		};
	}

}

#endif