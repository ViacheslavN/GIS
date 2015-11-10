#ifndef _EMBEDDED_DATABASE_ROW_H_
#define _EMBEDDED_DATABASE_ROW_H_

#include "embDB.h"
#include <vector>
namespace embDB
{

	class CRow : public IRow
	{
		public:

			CRow(IFields* pFields, IFieldSet* pFieldSet = NULL);
			virtual ~CRow();
			virtual int32 count() const;
			virtual CommonLib::CVariant* value(int32 nNum);
			virtual const CommonLib::CVariant* value(int32 nNum) const;
			virtual bool set(CommonLib::CVariant& pValue, int32 nNum);
			virtual IFieldSetPtr		   GetFieldSet() const {return m_pFieldsSet;}
			virtual IFieldsPtr             GetSourceFields() const {return m_pFields;}
			virtual bool IsFieldSelected(int index) const;
			virtual int64				GetRowID() const;
			virtual void				SetRow(int64 nRowID);
	private:
		std::vector<CommonLib::CVariant>	m_vecValues;
		IFieldSetPtr m_pFieldsSet;
		IFieldsPtr m_pFields;
		std::vector<int>                    m_vecFieldMap;
		int64								m_nRowID;
	};
}
#endif