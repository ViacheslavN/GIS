#ifndef _EMBEDDED_DATABASE_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_STRING_FIELD_H_

#include "ValueField.h"
#include "StringTree.h"

namespace embDB
{

	template<class TBTree>
	class StringFieldIterator: public IFieldIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		StringFieldIterator(iterator& it) 
			: m_ParentIt(it)

		{

		}
		StringFieldIterator() :
		{}
		virtual ~StringFieldIterator(){}

		virtual bool isValid()
		{
			return !m_ParentIt.isNull();
		}
		virtual bool next() 
		{
			return m_ParentIt.next();
		}
		virtual bool back() 
		{
			return m_ParentIt.back();
		}
		virtual bool isNull()
		{
			return m_ParentIt.isNull();
		}
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
		virtual uint64 getRowID()
		{
			return m_ParentIt.key();
		}


		virtual int64 addr() const
		{
			return m_ParentIt.addr();
		}
		virtual int32 pos() const
		{
			return m_ParentIt.pos();
		}

		virtual bool copy(IFieldIterator *pIter)
		{
			return m_ParentIt.setAddr(pIter->addr(), pIter->pos());
		}

		void set(iterator it)
		{
			m_ParentIt = it;
		}


	public:
		iterator m_ParentIt;
		embDB::eStringCoding m_cs;
		CommonLib::alloc_t *m_pAlloc;

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

		TStringValueField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) : TBase(pTransactions,pAlloc) 
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


	class StringValueFieldHandler : public CDBFieldHandlerBase
	{
	public:

		typedef TBPStringTree<uint64, IDBTransactions> TBTree;
		typedef TStringValueField<TBTree> TField;

		StringValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
		{}
		~StringValueFieldHandler()
		{}

		virtual bool save(int64 nAddr, IDBTransactions *pTran)
		{
			return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
		}
		virtual IValueFiled* getValueField(IDBTransactions* pTransactions, IDBStorage *pStorage)
		{
			TField * pField = new  TField(pTransactions, m_pAlloc);
			pField->load(m_fi.m_nFieldPage, pTransactions->getType());
			return pField;	
		}
		virtual bool release(IValueFiled* pField)
		{
			TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;
			return true;
		}
	};

}

#endif