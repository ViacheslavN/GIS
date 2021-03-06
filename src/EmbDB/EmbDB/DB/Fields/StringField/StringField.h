#ifndef _EMBEDDED_DATABASE_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_STRING_FIELD_H_

#include "DB/Fields/ValueField.h"
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
		StringFieldIterator()
		{}
		virtual ~StringFieldIterator(){}

	
		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sStringVal& sString= this->m_ParentIt.value();
			CommonLib::CString sVal;

			((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), sVal);


			pVal->setVal(sVal);
			return true;
		}

		virtual bool getVal(CommonLib::CString& sValue)
		{
			((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), sValue);
			return true;
		}
	};


	template<class _TBTree>
	class TStringValueField : public ValueFieldBase<CommonLib::CString, _TBTree, StringFieldIterator<_TBTree>, TStringVarConvertor<_TBTree, sStringVal> >
	{
	public:
		typedef  StringFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CString,_TBTree, TFieldIterator, TStringVarConvertor<_TBTree, sStringVal> > TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TStringValueField( IDBFieldHolder* pFieldHolder,  IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) : TBase(pFieldHolder, pTransactions, pAlloc, nPageSize, nBTreeChacheSize) 
		{
			this->m_ConvertTypeToVar.Init(&m_tree, m_pAlloc);
		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			typename TBTree::iterator it = this->m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::CString sVal(this->m_pAlloc);
			this->m_tree.convert(it.value(), sVal);

			pFieldVal->setVal(sVal);
			return true;
		}



	};


	class StringValueFieldHolder : public CDBFieldHolderBase<IDBFieldHolder>
	{
	public:

		typedef TBPStringTree<int64, IDBTransaction> TBTree;
		typedef TStringValueField<TBTree> TField;

		typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef TBTree::TLeafCompressorParams TLeafCompressorParams;

		StringValueFieldHolder(CommonLib::alloc_t* pAlloc, const SFieldProp *pFP, int64 nPageAdd) : CDBFieldHolderBase(pAlloc, pFP, nPageAdd)
		{}
		~StringValueFieldHolder()
		{}

		virtual bool save(CommonLib::IWriteStream* pStream,    IDBTransaction *pTran)
		{


			//FilePagePtr pLeafCompRootPage = pTran->getNewPage(MIN_PAGE_SIZE);
			TBTree::TLeafCompressorParams compParams(this->m_bCheckCRC);
			//compParams.setRootPage(pLeafCompRootPage->getAddr());
			compParams.SetMaxPageStringSize(m_nPageSize/25); //TO DO FIX
			compParams.SetStringLen(m_nLenField);
		//	compParams.save(pTran);

			TBTree::TInnerCompressorParams compInnerParams;

			

			if(!CDBFieldHolderBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &compInnerParams, &compParams))
				return false;

			return true;
 
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{return CDBFieldHolderBase::getValueField<TField>(pTransactions, pStorage);
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