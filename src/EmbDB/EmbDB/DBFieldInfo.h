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

		CDBFieldHandlerBase(CommonLib::alloc_t* pAlloc, const SFieldProp* pFieldProp, int64 nPageAdd) : m_pAlloc(pAlloc), 
			m_pIndexHandler(0), m_nFieldInfoPage(-1), m_nPageAdd(nPageAdd)
		{
			assert(pFieldProp);

			m_FieldType = pFieldProp->m_dataType;
			m_nLenField = pFieldProp->m_nLenField;
			m_bNoNull = pFieldProp->m_bNotNull;
			m_dScale = pFieldProp->m_dScale;
			m_nPrecision = pFieldProp->m_nPrecision;
			m_defValue  = pFieldProp->m_devaultValue;
			m_nPageSize = pFieldProp->m_nPageSize;

		}
		~CDBFieldHandlerBase(){}
	/*	template<class TField>
		bool save(int64 nAddr, IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,  uint16 nObjectPageType, uint16 nSubObjectPageType,
			int64 nInnerCompParams = -1, int64 nLeafCompParams = -1)
		{
			//m_nFieldInfoPage = nAddr;
			FilePagePtr pPage(pTran->getFilePage(nAddr, MIN_PAGE_SIZE));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, nObjectPageType, nSubObjectPageType);
			int64 m_nBTreeRootPage = -1;
			FilePagePtr pRootPage(pTran->getNewPage(8192)); // TO DO FIX
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

			return true;
		}*/


		template<class TField, class TInnerCompParams, class TLeafCompParams>
		bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,
			TInnerCompParams *pInnerCompParams = NULL, TLeafCompParams* pLeafCompParams = NULL)
		{

			int nNameUtf8Len = m_sFieldName.calcUTF8Length();
			int nAliasUtf8Len = m_sFieldAlias.calcUTF8Length();
			std::vector<char> vecBufName(nNameUtf8Len + 1);
			std::vector<char> vecAliasName(nAliasUtf8Len + 1);

			m_sFieldName.exportToUTF8(&vecBufName[0], vecBufName.size());
			m_sFieldAlias.exportToUTF8(&vecAliasName[0], vecAliasName.size());

			pStream->write(nNameUtf8Len);
			pStream->write((byte*)&vecBufName[0], vecBufName.size());
			pStream->write(nAliasUtf8Len);
			pStream->write((byte*)&vecAliasName[0], vecAliasName.size());
			pStream->write(m_nPageSize);
			pStream->write(m_nPrecision);
			pStream->write(m_dScale);


			FilePagePtr pFieldInfoPage(pTran->getNewPage(MIN_PAGE_SIZE)); 
			if(!pFieldInfoPage.get())
				return false;
			m_nFieldInfoPage = pFieldInfoPage->getAddr();
			pStream->write(m_nFieldInfoPage);

			TField field(pTran, pAlloc, m_nPageSize);
			field.init(m_nFieldInfoPage, pInnerCompParams, pLeafCompParams);
		}
		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			int nNameUtf8Len = pStream->readIntu32();
			int nAliasUtf8Len = pStream->readIntu32();
			if(nNameUtf8Len != 0)
			{
				std::vector<char> vecBufName(nNameUtf8Len + 1);
				pStream->read((byte*)&vecBufName[0], vecBufName.size());
				m_sFieldName.loadFromUTF8(&vecBufName[0]);
			}
			if(nAliasUtf8Len != 0)
			{
				std::vector<char> vecAliasName(nAliasUtf8Len + 1);
				pStream->read((byte*)&vecAliasName[0], vecAliasName.size());
				m_sFieldName.loadFromUTF8(&vecAliasName[0]);
			}
			m_nPageSize = pStream->readIntu32();
			m_nPrecision = pStream->readInt32();
			m_dScale = pStream->readDouble();
			m_nFieldInfoPage = pStream->readInt64();
			return true;
		}





		template<class TField>
		IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TField * pField = new  TField(pTransactions, m_pAlloc, m_nPageSize);
			pField->load(m_nBTreeRootPage, pTransactions->getType());
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
			return m_FieldType;
		}
		virtual const CommonLib::CString& getName() const
		{
			return m_sFieldName;
		}
		virtual const CommonLib::CString& getAlias() const
		{
			return m_sFieldAlias;
		}
		virtual uint32 GetLength()	const
		{
			return m_nLenField;
		}
		virtual bool GetIsNotNull() const
		{
			return m_bNoNull;
		}
	
		virtual double GetScale() const
		{
			return m_dScale;
		}
		virtual const CommonLib::CVariant& 	GetDefaultValue() const
		{
			return m_defValue;
		}
		virtual int  GetPrecision() const
		{
			return m_nPrecision;
		}
		
		virtual int64 GetPageAddr() const {return m_nPageAdd;}
	protected:
		CommonLib::CString m_sFieldName;
		CommonLib::CString m_sFieldAlias;
		eDataTypes			m_FieldType;
		uint32				m_nLenField;
		bool				m_bNoNull;
		double				m_dScale;
		int32				m_nPrecision;
		uint32				m_nPageSize;
		CommonLib::CVariant m_defValue;
		CommonLib::alloc_t* m_pAlloc;
		IDBIndexHandlerPtr m_pIndexHandler;
		uint64				m_nFieldInfoPage;
		int64				m_nPageAdd;
	};

 
}
#endif