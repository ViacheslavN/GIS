#ifndef	_EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#define _EMBEDDED_DATABASE_I_TRANSACTIONS_BASE_H_
#include "../../embDBInternal.h"
#include "CommonLibrary/HashKey.h"
#include "Cursors/InsertCursor/InsertCursor.h"
#include "../Indexs/spatial/SpatialIndexBase.h"
#include "Cursors/SelectOpCursor/SimpleSearchCursor.h"
#include "Cursors/SelectOpCursor/SimpleSelectCursor.h"
#include "Cursors/DeleteCursor.h"
#include <set>
#include "Cursors/SelectOpCursor/SimpleSelectOpCursor.h"
#include "../../ConsolLog.h"
namespace embDB
{
	 
	template<typename I>
	class ITransactionBase : public I
	{
		public:

			ITransactionBase(IDBConnection* pDBConnection, CommonLib::alloc_t* pAlloc) : m_pConnection(pDBConnection), m_bError(false),
				m_nMaxMessageSize(1024 * 1024), m_pAlloc(pAlloc)
			{
				if(m_pConnection) //check for test bplus tree 
				{
					m_pSchema = pDBConnection->getSchema();
					m_pDBStorage = pDBConnection->getDBStorage();
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

				IDBFieldHolder *pFieldHolder = dynamic_cast<IDBFieldHolder*>(pField.get());
				if(!pFieldHolder)
					return  IValueFieldPtr();

				IValueFieldPtr pValueField = pFieldHolder->getValueField(this, m_pDBStorage.get());

				m_mapValueField.insert(std::make_pair(key, pValueField));
				return pValueField;
			}

			

			virtual IInsertCursorPtr createInsertCursor(const wchar_t *pszTable, IFieldSet *pFileds = 0)
			{
				ITablePtr pTable = m_pSchema->getTableByName(pszTable);
				if(!pTable.get())
					return IInsertCursorPtr();				 

				IDBTable *pDBTable = (IDBTable *)pTable.get();

				pDBTable->lockWrite(); //TO DO Add to wait graf

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

				IDBFieldHolderPtr pFieldHolder(pField->GetFieldHolder());


				if(pFieldHolder->getType() != dtGeometry)
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

			virtual void setMaxLimitErrorMessage(uint32 nSize) 
			{
				m_nMaxMessageSize = nSize;
			}

			virtual void SetLogger(ILogger *pLogger)
			{
				m_pLogger = pLogger;
			}



			virtual bool isError() const
			{
				return m_bError;
			}
			virtual void error(const wchar_t *pszFormat, ...)
			{
				m_bError = true;

				if(m_pLogger.get())
				{
					va_list args;
					va_start(args, pszFormat);

					uint32 len =  _vscwprintf(pszFormat, args);

					if(len != 0)
					{
						wchar_t* buffer= (wchar_t*)_alloca ((len + 1)* sizeof (wchar_t)); 
						vswprintf(buffer, pszFormat, args);
						m_pLogger->error(buffer);
					}

					va_end(args);

				}


				if(m_sErrorMSG.length() < m_nMaxMessageSize)
				{
					 CommonLib::CString sTemp;
					va_list qlist;
					va_start(qlist, pszFormat);
					m_sErrorMSG += sTemp.format(pszFormat, qlist);
					va_end(qlist);
				}

			}

			void log_msg(uint32 nLevel, const wchar_t *pszMsg) 
			{
 
				if(m_pLogger.get() && m_pLogger->GetLogLevel() >= nLevel)
					m_pLogger->log_msg(nLevel, pszMsg);
			}
			virtual void log(uint32 nLevel, const wchar_t *pszFormat, ...)
			{
 

				if(m_pLogger.get() && m_pLogger->GetLogLevel() >= nLevel)
				{
					va_list args;
					va_start(args, pszFormat);

					uint32 len =  _vscwprintf(pszFormat, args);
					
					if(len != 0)
					{
						wchar_t* buffer= (wchar_t*)_alloca ((len + 1)* sizeof (wchar_t)); 
						vswprintf(buffer, pszFormat, args);
						m_pLogger->log_msg(nLevel, buffer);
					}

					va_end(args);
				}
			}


			virtual uint32 getErrorMessageSize() const
			{
				return m_sErrorMSG.length();
			}
			virtual uint32 getErroMessage(wchar_t * pBuf, uint32 nSize) const
			{
				uint32 nLenSize = min(nSize, getErrorMessageSize());
				memcpy(pBuf, m_sErrorMSG.cwstr(), nLenSize * sizeof(wchar_t));
				return nLenSize;
			}

	protected:


		CommonLib::CString m_sErrorMSG;

		IDBConnection* m_pConnection;
		IDBStoragePtr m_pDBStorage;
		ISchemaPtr m_pSchema;
		typedef CommonLib::CHash2Key<CommonLib::CString, CommonLib::CString> TValueFieldKey;

		typedef std::map<TValueFieldKey, IValueFieldPtr> TMapValueField;

		typedef std::set<IDBTablePtr>  TChangeTable;
		TMapValueField m_mapValueField;

		TChangeTable m_setChangeTable;
		ILoggerPtr m_pLogger;
		bool m_bError;
		uint32 m_nMaxMessageSize;
		CommonLib::alloc_t *m_pAlloc;


	};
}


#endif