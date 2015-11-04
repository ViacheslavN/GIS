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
				return pValueField;
			}


			virtual IInsertCursorPtr createInsertCursor(const wchar_t *pszTable, IFieldSet *pFileds = 0)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return IInsertCursorPtr();				 


				CInsertCursor * pInsertCursor = new CInsertCursor(this, pTable.get(), pFileds);
				if(!pInsertCursor->init())
				{
					delete pInsertCursor;
					return IInsertCursorPtr();	
				}
				m_setChangeTable.insert(IDBTablePtr((IDBTable*)pTable.get()));
				return IInsertCursorPtr(pInsertCursor);
			}

			virtual ICursorPtr executeSpatialQuery(const CommonLib::bbox& extent, const wchar_t *pszTable, const wchar_t* pszSpatialField, SpatialQueryMode mode = sqmIntersect,  IFieldSet *pFileds = 0)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return ICursorPtr(); //TO DO Error

				IFieldPtr pField = pTable->getField(pszSpatialField);
				if(!pField.get())
					return ICursorPtr(); //TO DO Error


				if(pField->getType() != dtPoint16 && pField->getType() != dtPoint32 && pField->getType() != dtPoint64
					&& pField->getType() != dtRect16 && pField->getType() != dtRect32 && pField->getType() != dtRect64)
				{
					return ICursorPtr(); //TO DO Error
				}

				IDBFieldHandler *pDBFieldHandler =  dynamic_cast<IDBFieldHandler*>(pField.get());
				if(!pDBFieldHandler)
					return ICursorPtr(); //TO DO Error

				IDBIndexHandlerPtr pIndexHandle =  pDBFieldHandler->getIndexIndexHandler();
				if(pIndexHandle.get())
				{
					embDB::IndexFiledPtr pIndex =  pIndexHandle->getIndex(this, NULL);

					ISpatialIndex* pSpatialIndex = dynamic_cast<ISpatialIndex*>(pIndex.get());
					if(!pSpatialIndex)
						return ICursorPtr(); //TO DO Error

					IIndexIteratorPtr pIndexIterator = pSpatialIndex->find(extent);





				}

				return ICursorPtr();

			}


			bool CommitTemp()
			{
				{
					TMapValueField::iterator it =  m_mapValueField.begin();
					TMapValueField::iterator end =  m_mapValueField.end();
					for (; it != end; ++it)
					{
						IValueFieldPtr pValueField = it->second;
						if(!pValueField->commit())
							return false;
					}
				}
				
				
				{
					TChangeTable::iterator it = m_setChangeTable.begin();
					TChangeTable::iterator end = m_setChangeTable.end();
					for(; it != end; ++it)
					{
						IDBTablePtr pTable = *it;
						pTable->commit(this);
					}
				}

				return true;
			}
	protected:

		CDatabase* m_pDatabase;
		IDBStoragePtr m_pDBStorage;
		ISchemaPtr m_pSchema;
		typedef CommonLib::CHash2Key<CommonLib::CString, CommonLib::CString> TValueFieldKey;
		typedef std::map<TValueFieldKey, IValueFieldPtr> TMapValueField;
		typedef std::set<IDBTablePtr>  TChangeTable;
		TMapValueField m_mapValueField;
		TChangeTable m_setChangeTable;


	};
}


#endif