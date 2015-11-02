#ifndef _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_

#include "ValueField.h"
#include "FixedStringTree.h"

namespace embDB
{

	template<class TBTree>
	class FixedStringFieldIterator: public IFieldIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		FixedStringFieldIterator(iterator& it) 
			: m_ParentIt(it)

		{

		}
		FixedStringFieldIterator() :
		{}
		virtual ~FixedStringFieldIterator(){}

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
	class TFixedStringValueField : public ValueFieldBase<CommonLib::CString, _TBTree, FixedStringFieldIterator<_TBTree> >
	{
	public:
		typedef  FixedStringFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CString,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TFixedStringValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) : TBase(pTransactions,pAlloc) 
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
			return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
		}
		virtual IValueFiledPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHandlerBase::getValueField<TField>(pTransactions, pStorage);
		}
		virtual bool release(IValueFiled* pField)
		{
			/*TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;*/
			return true;
		}
	};

}

#endif