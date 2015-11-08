#ifndef _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_

#include "ValueField.h"
#include "FixedStringTree.h"
#include "FieldIteratorBase.h"
namespace embDB
{

	template<class TBTree>
	class FixedStringFieldIterator: public TFieldIteratorBase<TBTree, IFieldIterator>
	{
	public:
		typedef typename TBTree::iterator  iterator;
		typedef TFieldIteratorBase<TBTree, IFieldIterator> TBase;

		FixedStringFieldIterator(iterator& it, IValueField *pField) 
			: TBase(it, pField)

		{

		}
		FixedStringFieldIterator() :
		{}
		virtual ~FixedStringFieldIterator(){}

	
		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sFixedStringVal& sString= m_ParentIt.value();
			CommonLib::CString sVal;

			((TBTree*)m_ParentIt.m_pTree)->convert(m_ParentIt.value(), sVal);

			
			pVal->setVal(sVal);
			return true;
		}

		virtual bool getVal(CommonLib::CString& sValue)
		{
			((TBTree*)m_ParentIt.m_pTree)->convert(m_ParentIt.value(), sValue);
			return true;
		}
	};


	template<class _TBTree>
	class TFixedStringValueField : public ValueFieldBase<CommonLib::CString, _TBTree, FixedStringFieldIterator<_TBTree> >
	{
	public:
		typedef  FixedStringFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CString,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TFixedStringValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, pFieldInfo) 
		{

		}


		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal)
		{
			TBTree::iterator it = m_tree.find(nOID);
			if(it.isNull())
				return false;
		 
			CommonLib::CString sVal(m_pAlloc);
			m_tree.convert(it.value(), sVal);

			pFieldVal->setVal(sVal);
			return true;
		}

	
 
	};


	class FixedStringValueFieldHandler : public CDBFieldHandlerBase
	{
	public:
 
		typedef TBPFixedString<uint64, IDBTransaction> TBTree;
		typedef TFixedStringValueField<TBTree> TField;

		FixedStringValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
		{}
		~FixedStringValueFieldHandler()
		{}

		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			FilePagePtr pLeafCompRootPage = pTran->getNewPage();
			TBTree::TLeafCompressorParams compParams;
			compParams.setRootPage(pLeafCompRootPage->getAddr());
			compParams.SetMaxPageStringSize(pTran->getPageSize()/15);
			compParams.SetStringLen(m_fi.m_nLenField);
			compParams.save(pTran);

			return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE, -1, pLeafCompRootPage->getAddr());
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHandlerBase::getValueField<TField>(pTransactions, pStorage);
		}
		virtual bool release(IValueField* pField)
		{
			/*TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;*/
			return true;
		}
	};

}

#endif