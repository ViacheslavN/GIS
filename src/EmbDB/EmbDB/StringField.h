#ifndef _EMBEDDED_DATABASE_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_STRING_FIELD_H_

#include "ValueField.h"
#include "StringTree.h"
#include "FieldIteratorBase.h"
namespace embDB
{

	template<class TBTree>
	class StringFieldIterator: public TFieldIteratorBase<TBTree, IFieldIterator>
	{
	public:
		typedef typename TBTree::iterator  iterator;
		typedef TFieldIteratorBase<TBTree, IFieldIterator> TBase;

		StringFieldIterator(iterator& it, IValueField *pField) 
			: TBase(it, pField)

		{

		}
		StringFieldIterator() :
		{}
		virtual ~StringFieldIterator(){}

	
		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sStringVal& sString= m_ParentIt.value();
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
	class TStringValueField : public ValueFieldBase<CommonLib::CString, _TBTree, StringFieldIterator<_TBTree> >
	{
	public:
		typedef  StringFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CString,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TStringValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, pFieldInfo) 
		{

		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
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


	class StringValueFieldHandler : public CDBFieldHandlerBase
	{
	public:

		typedef TBPStringTree<int64, IDBTransaction> TBTree;
		typedef TStringValueField<TBTree> TField;

		StringValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
		{}
		~StringValueFieldHandler()
		{}

		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{return CDBFieldHandlerBase::getValueField<TField>(pTransactions, pStorage);
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