#ifndef _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_FIELD_H_

#include "DB/Fields/ValueField.h"
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
		FixedStringFieldIterator() 
		{}
		virtual ~FixedStringFieldIterator(){}

	
		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			/*const sFixedStringVal& sString= this->m_ParentIt.value();
			CommonLib::CString sVal;

			((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), sVal);
			*/
			
			pVal->setVal(this->m_ParentIt.value());
			return true;
		}

		virtual bool getVal(CommonLib::CString& sValue)
		{
			//((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), sValue);
			sValue = this->m_ParentIt.value();
			return true;
		}
	};


	template<class _TBTree>
	class TFixedStringValueField : public ValueFieldBase<CommonLib::CString, _TBTree, FixedStringFieldIterator<_TBTree>, TVarConvertor<CommonLib::CString>  >
	{
	public:
		typedef  FixedStringFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CString,_TBTree, TFieldIterator, TVarConvertor<CommonLib::CString> > TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TFixedStringValueField( IDBFieldHolder* pFieldHolder, IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) : TBase(pFieldHolder, pTransactions, pAlloc, nPageSize, nBTreeChacheSize) 
		{
			//this->m_ConvertTypeToVar.Init(&m_tree, m_pAlloc);
		}

		~TFixedStringValueField()
		{

		}

		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			typename TBTree::iterator it = this->m_tree.find(nOID);
			if(it.isNull())
				return false;
		 
		//	CommonLib::CString sVal(this->m_pAlloc);
		//	this->m_tree.convert(it.value(), sVal);

			pFieldVal->setVal(it.value());
			return true;
		}

	
 
	};


	class FixedStringValueFieldHolder : public CDBFieldHolderBase<IDBFieldHolder>
	{
	public:
 
		typedef TBPFixedString<int64, IDBTransaction> TBTree;
		typedef TFixedStringValueField<TBTree> TField;
		typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef TBTree::TLeafCompressorParams TLeafCompressorParams;

		FixedStringValueFieldHolder(CommonLib::alloc_t* pAlloc, const SFieldProp* pFP, int64 nPageAdd) : CDBFieldHolderBase<IDBFieldHolder>(pAlloc, pFP, nPageAdd)
		{}
		~FixedStringValueFieldHolder()
		{}

		/*virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			FilePagePtr pLeafCompRootPage = pTran->getNewPage(MIN_PAGE_SIZE);
			TBTree::TLeafCompressorParams compParams;
			compParams.setRootPage(pLeafCompRootPage->getAddr());
			compParams.SetMaxPageStringSize(/*pTran->getPageSize()/15); //TO FO FIX 
			compParams.SetStringLen(m_fi.m_nLenField);
			compParams.save(pTran);

			return CDBFieldHolderBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE, -1, pLeafCompRootPage->getAddr());
		}*/
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{

			//FilePagePtr pLeafCompRootPage = pTran->getNewPage(MIN_PAGE_SIZE);
			TLeafCompressorParams compParams(m_bCheckCRC);
			//compParams.setRootPage(pLeafCompRootPage->getAddr());
			compParams.SetMaxPageStringSize(m_nPageSize/15); //TO FO FIX 
			compParams.SetStringLen(m_nLenField);


			TInnerCompressorParams innerCompParams;

			innerCompParams.m_compressType = m_CompressType;
			innerCompParams.m_bCalcOnlineSize = m_bOnlineCalcCompSize;
			innerCompParams.m_nErrorCalc		  = m_nCompCalcError;

			//compParams.save(pTran);

			return CDBFieldHolderBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &innerCompParams, &compParams);
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHolderBase::getValueField<TField>(pTransactions, pStorage);
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