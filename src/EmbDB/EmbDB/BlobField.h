#ifndef _EMBEDDED_DATABASE_BLOB_FIELD_H_
#define _EMBEDDED_DATABASE_BLOB_FIELD_H_

#include "ValueField.h"
#include "BlobTree.h"
#include "FieldIteratorBase.h"
namespace embDB
{

	template<class TBTree>
	class BlobFieldIterator: public TFieldIteratorBase<TBTree, IFieldIterator>
	{
	public:
		typedef typename TBTree::iterator  iterator;
		typedef TFieldIteratorBase<TBTree, IFieldIterator> TBase;
		BlobFieldIterator(iterator& it, IValueField* pField) 
			: TBase(it, pField)

		{

		}
		BlobFieldIterator() :
		{}
		virtual ~BlobFieldIterator(){}

		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sBlobVal& sBlob = m_ParentIt.value();
			CommonLib::CBlob blob;

			((TBTree*)m_ParentIt.m_pTree)->convert(sBlob, blob);


			pVal->setVal(blob);
			return true;
		}

		virtual bool getVal(CommonLib::CBlob& blob)
		{
			((TBTree*)m_ParentIt.m_pTree)->convert(m_ParentIt.value(), blob);
			return true;
		}
	};


	template<class _TBTree>
	class TBlobValueField : public ValueFieldBase<CommonLib::CBlob, _TBTree, BlobFieldIterator<_TBTree> >
	{
	public:
		typedef  BlobFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CBlob,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TBlobValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, pFieldInfo) 
		{

		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			TBTree::iterator it = m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::CBlob blob(m_pAlloc);
			m_tree.convert(it.value(), blob);

			pFieldVal->setVal(blob);
			return true;
		}



	};


	class BlobValueFieldHandler : public CDBFieldHandlerBase
	{
	public:

		typedef TBPBlobTree<int64, IDBTransaction> TBTree;
		typedef TBlobValueField<TBTree> TField;

		typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef TBTree::TLeafCompressorParams TLeafCompressorParams;

		BlobValueFieldHandler(CommonLib::alloc_t* pAlloc, const SFieldProp* pFP, int64 nPageAdd) : CDBFieldHandlerBase(pAlloc, pFP, nPageAdd)
		{}
		~BlobValueFieldHandler()
		{}

		/*virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			return CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
		}*/
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{
			return CDBFieldHandlerBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc);
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHandlerBase::getValueField<TField>(pTransactions, pStorage);
		}
		virtual bool release(IValueField* pField)
		{
		/*	TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;*/
			return true;
		}
	};

}

#endif