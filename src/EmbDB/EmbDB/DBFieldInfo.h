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

	template<class I>
	class CDBFieldHandlerBase  : public I
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
			m_sFieldName = pFieldProp->m_sFieldName;
			m_sFieldAlias = pFieldProp->m_sFieldAlias;

		}
		~CDBFieldHandlerBase(){}



		template<class TField, class TInnerCompParams, class TLeafCompParams>
		bool base_save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,
			TInnerCompParams *pInnerCompParams = NULL, TLeafCompParams* pLeafCompParams = NULL)
		{
			uint32 nNameUtf8Len = m_sFieldName.calcUTF8Length();
			uint32 nAliasUtf8Len = m_sFieldAlias.calcUTF8Length();
			std::vector<char> vecBufName(nNameUtf8Len + 1);
			std::vector<char> vecAliasName(nAliasUtf8Len + 1);

			m_sFieldName.exportToUTF8(&vecBufName[0], vecBufName.size());
			m_sFieldAlias.exportToUTF8(&vecAliasName[0], vecAliasName.size());

			pStream->write(nNameUtf8Len);
			pStream->write((byte*)&vecBufName[0], vecBufName.size());
			pStream->write(nAliasUtf8Len);
			pStream->write((byte*)&vecAliasName[0], vecAliasName.size());
			pStream->write(m_nPrecision);
			pStream->write(m_dScale);

			FilePagePtr pFieldInfoPage(pTran->getNewPage(MIN_PAGE_SIZE)); 
			if(!pFieldInfoPage.get())
				return false;
			m_nFieldInfoPage = pFieldInfoPage->getAddr();
			pStream->write(m_nFieldInfoPage);
			return true;
		}

		template<class TField, class TInnerCompParams, class TLeafCompParams>
		bool initField(IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,
			TInnerCompParams *pInnerCompParams = NULL, TLeafCompParams* pLeafCompParams = NULL)
		{

		

			TField field(pTran, pAlloc, m_nPageSize);
			field.init(m_nFieldInfoPage, pInnerCompParams, pLeafCompParams);

			return true;
		}

		template<class TField, class TInnerCompParams, class TLeafCompParams>
		bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran, CommonLib::alloc_t *pAlloc,
			TInnerCompParams *pInnerCompParams = NULL, TLeafCompParams* pLeafCompParams = NULL)
		{

			base_save<TField, TInnerCompParams, TLeafCompParams>(pStream, pTran, pAlloc, pInnerCompParams, pLeafCompParams);
			initField<TField, TInnerCompParams, TLeafCompParams>(pTran, pAlloc, pInnerCompParams, pLeafCompParams);
			pStream->write(m_nFieldInfoPage); 
			return true;
		}

		void base_load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			uint32 nNameUtf8Len = pStream->readIntu32();
			if(nNameUtf8Len != 0)
			{
				std::vector<char> vecBufName(nNameUtf8Len + 1);
				pStream->read((byte*)&vecBufName[0], vecBufName.size());
				m_sFieldName.loadFromUTF8(&vecBufName[0]);
			}
			uint32 nAliasUtf8Len = pStream->readIntu32();
			if(nAliasUtf8Len != 0)
			{
				std::vector<char> vecAliasName(nAliasUtf8Len + 1);
				pStream->read((byte*)&vecAliasName[0], vecAliasName.size());
				m_sFieldAlias.loadFromUTF8(&vecAliasName[0]);
			}
			m_nPrecision = pStream->readInt32();
			m_dScale = pStream->readDouble();
		}

		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			base_load(pStream, pStorage);
			m_nFieldInfoPage = pStream->readInt64();
			return true;
		}





		template<class TField>
		IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TField * pField = new  TField(pTransactions, m_pAlloc, m_nPageSize);
			pField->load(m_nFieldInfoPage);
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