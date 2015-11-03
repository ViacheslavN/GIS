#ifndef	_EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#include "embDBInternal.h"
#include "CommonLibrary/HashKey.h"
#include "InsertCursor.h"
namespace embDB
{
	class CDatabase;
	template<typename I>
	class ITransactionBase : public I
	{
		public:

			ITransactionBase(CDatabase* pDatabase) : m_pDatabase(pDatabase)
			{
				if(m_pDatabase) //check for test bplus tree 
				{
					m_pSchema = m_pDatabase->getSchema();
					m_pDBStorage = m_pDatabase->getDBStorage();
				}
			}

			virtual IValueFieldPtr GetField(const wchar_t* pszTableName, const wchar_t* pszFieldName)
			{

				TValueFieldKey key(pszTableName, pszFieldName);
				TMapValueField::iterator it =  m_mapValueField.find(key);
				if(it != m_mapValueField.end())
					return it->second;

				ITablePtr pTable = m_pSchema->getTableByName(pszTableName);
				if(!pTable.get())
					return IValueFieldPtr();

				IFieldPtr pField = pTable->getField(pszFieldName);
				if(!pField.get())
					return IValueFieldPtr();

				IDBFieldHandler *pFieldHandler = dynamic_cast<IDBFieldHandler*>(pField.get());
				if(!pFieldHandler)
					return  IValueFieldPtr();

				IValueFieldPtr pValueField = pFieldHandler->getValueField(this, m_pDBStorage.get());

				m_mapValueField.insert(std::make_pair(key, pValueField));
				return IValueFieldPtr();
			}


			virtual IInsertCursorPtr createInsertCursor(const wchar_t *pszTable, IFieldSet *pFileds = 0)
			{
				return IInsertCursorPtr();
			}


			bool CommitTemp()
			{
				TMapValueField::iterator it =  m_mapValueField.begin();
				TMapValueField::iterator end =  m_mapValueField.end();
				for (; it != end; ++it)
				{
					IValueFieldPtr pValueField = it->second;
					if(!pValueField->commit())
						return false;
				}
				return true;
			}
	protected:

		CDatabase* m_pDatabase;
		IDBStoragePtr m_pDBStorage;
		ISchemaPtr m_pSchema;
		typedef CommonLib::CHash2Key<CommonLib::CString, CommonLib::CString> TValueFieldKey;
		typedef std::map<TValueFieldKey, IValueFieldPtr> TMapValueField;
		TMapValueField m_mapValueField;


	};
}


#endif