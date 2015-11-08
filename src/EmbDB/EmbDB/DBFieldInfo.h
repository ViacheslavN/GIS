#ifndef _EMBEDDED_DATABASE__DB_FIELD_INFO_H_
#define _EMBEDDED_DATABASE__DB_FIELD_INFO_H_

#include "CommonLibrary/String.h"
#include "embDBInternal.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "Key.h"
#include "CommonLibrary/SpatialKey.h"

#include "CommonLibrary/BoundaryBox.h"

namespace embDB
{
	class CStorage;

	class CDBFieldHandlerBase  : IDBFieldHandler
	{
	public:

		CDBFieldHandlerBase(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc), m_pIndexHandler(0)
		{}
		~CDBFieldHandlerBase(){}
		template<class TField>
		bool save(int64 nAddr, IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,  uint16 nObjectPageType, uint16 nSubObjectPageType,
			int64 nInnerCompParams = -1, int64 nLeafCompParams = -1)
		{
			//m_nFieldInfoPage = nAddr;
			FilePagePtr pPage(pTran->getFilePage(nAddr));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, nObjectPageType, nSubObjectPageType);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage());
			if(!pRootPage.get())
				return false;
			m_nBTreeRootPage = pRootPage->getAddr();
			stream.write(m_nBTreeRootPage);
			header.writeCRC32(stream);
			pPage->setFlag(eFP_CHANGE, true);
			pTran->saveFilePage(pPage);

			TField field(pTran, pAlloc, NULL);
			field.init(m_nBTreeRootPage, nInnerCompParams, nLeafCompParams);
			return field.save();
		}

		template<class TField>
		IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TField * pField = new  TField(pTransactions, m_pAlloc, &m_fi);
			pField->load(m_fi.m_nFieldPage, pTransactions->getType());
			if(m_pIndexHandler.get())
			{
				IndexFiledPtr pIndex = m_pIndexHandler->getIndex(pTransactions, pStorage);
				pField->SetIndex(pIndex.get());
			}
			return IValueFieldPtr(pField);	
		}

		bool isCanBeRemoving()
		{
			return true;
		}

		virtual void setIndexHandler(IDBIndexHandler *pIndexHandler)
		{
			m_pIndexHandler = pIndexHandler;
		}
		virtual IDBIndexHandlerPtr getIndexIndexHandler()
		{
			return m_pIndexHandler;
		}
		virtual bool lock()
		{
			return true;
		}
		virtual bool unlock()
		{
			return true;
		}

		virtual eDataTypes getType() const
		{
			return (eDataTypes)m_fi.m_nFieldType;
		}
		virtual const CommonLib::CString& getName() const
		{
			return m_fi.m_sFieldName;
		}
		virtual const CommonLib::CString& getAlias() const
		{
			return m_fi.m_sFieldAlias;
		}
		virtual uint32 GetLength()	const
		{
			return m_fi.m_nLenField;
		}
		virtual bool GetIsNotEmpty() const
		{
			return (m_fi.m_nFieldDataType&dteIsNotEmpty) != 0;
		}
		virtual sFieldInfo* getFieldInfoType()
		{
			return &m_fi;
		}
		virtual void setFieldInfoType(sFieldInfo* fi)
		{
			assert(fi);
			m_fi = *fi;
		}
		
		virtual bool load(int64 nAddr, IDBStorage *pStorage)
		{
			return true;
		}

	protected:
		sFieldInfo m_fi;
		CommonLib::alloc_t* m_pAlloc;
		IDBIndexHandlerPtr m_pIndexHandler;
	};

 
}
#endif