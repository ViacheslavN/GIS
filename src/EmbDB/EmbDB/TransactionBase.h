#ifndef	_EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#include "embDBInternal.h"
#include "CommonLibrary/HashKey.h"
#include "InsertCursor.h"
#include "SpatialIndexBase.h"
#include "SimpleSearchCursor.h"
#include "SimpleSelectCursor.h"
#include "DeleteCursor.h"
#include <set>
#include "SimpleSelectOpCursor.h"
namespace embDB
{
	 
	template<typename I>
	class ITransactionBase : public I
	{
		public:

			ITransactionBase(IDBDatabase* pDatabase) : m_pDatabase(pDatabase)
			{
				if(m_pDatabase) //check for test bplus tree 
				{
					m_pSchema = m_pDatabase->getSchema();
					m_pDBStorage = m_pDatabase->getDBStorage();
				}
			}
			~ITransactionBase()
			{
				m_mapValueField.clear();
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
				return IInsertCursorPtr(pInsertCursor);//TO DO save cursors and close cursors before close transactions
			}

			virtual ICursorPtr executeSpatialQuery(const CommonLib::bbox& extent, const wchar_t *pszTable, const wchar_t* pszSpatialField, SpatialQueryMode mode = sqmIntersect,  IFieldSet *pFileds = 0)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return ICursorPtr(); //TO DO Error



				IValueFieldPtr pField = GetField(pTable->getName().cwstr(), pszSpatialField);
				if(!pField.get())
					return ICursorPtr(); //TO DO Error

				IDBFieldHandlerPtr pFieldHandler(pField->GetFieldHandler());


				if(pFieldHandler->getType() != dtGeometry)
				{
					return ICursorPtr(); //TO DO Error
				}

				IndexFiledPtr pIndex(pField->GetIndex());
				if(pIndex.get())
				{
					

					ISpatialIndex* pSpatialIndex = dynamic_cast<ISpatialIndex*>(pIndex.get());
					if(!pSpatialIndex)
						return ICursorPtr(); //TO DO Error

					IIndexIteratorPtr pIndexIterator = pSpatialIndex->find(extent, mode);

					SimpleSearchCursor *pCursor = new SimpleSearchCursor(pIndexIterator.get(), this, pTable.get(), pFileds);
					if(!pCursor->Init())
						return ICursorPtr(); //TO DO Error
					return  ICursorPtr(pCursor);


				}

				return ICursorPtr();

			}
			virtual ICursorPtr executeSelectQuery(const wchar_t *pszTable, IFieldSet *pFileds = 0, const wchar_t *pszSQLQuery = NULL)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return ICursorPtr(); //TO DO Error


				if(pszSQLQuery == NULL)
				{
					SimpleSelectCursor* pCursor = new SimpleSelectCursor(this, pTable.get(), pFileds); 
					if(!pCursor->Init())
					{
						delete pCursor;
						return ICursorPtr(); //TO DO Error
					}
					return ICursorPtr(pCursor);
				}
			 
				return ICursorPtr();
			}
			virtual IDeleteCursorPtr createDeleteCursor(const wchar_t *pszTable) 
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return IDeleteCursorPtr(); //TO DO Error

				CDeleteCursor* pCursor = new CDeleteCursor(this, pTable.get());
				if(!pCursor->Init())
				{
					delete pCursor;
					return IDeleteCursorPtr(); //TO DO Error
				}
				return IDeleteCursorPtr(pCursor);
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
				m_mapValueField.clear();
				m_setChangeTable.clear();
				return true;
			}


			virtual ICursorPtr executeSelectQuery(const wchar_t *pszTable, IFieldSet *pFileds, const wchar_t *pszFiel, const CommonLib::CVariant& var, OpType opType)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return ICursorPtr(); //TO DO Error
				SimpleSelectOpCursor* pCursor = new SimpleSelectOpCursor(this, pTable.get(), pFileds, pszFiel, var, opType); 
				if(!pCursor->Init())
				{
					delete pCursor;
					return ICursorPtr(); //TO DO Error
				}
				return ICursorPtr(pCursor);
			}
	protected:

		IDBDatabase* m_pDatabase;
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