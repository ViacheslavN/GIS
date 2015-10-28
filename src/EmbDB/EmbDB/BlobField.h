#ifndef _EMBEDDED_DATABASE_BLOB_FIELD_H_
#define _EMBEDDED_DATABASE_BLOB_FIELD_H_

#include "ValueField.h"
#include "BlobTree.h"

namespace embDB
{

	template<class TBTree>
	class BlobFieldIterator: public IFieldIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		BlobFieldIterator(iterator& it) 
			: m_ParentIt(it)

		{

		}
		BlobFieldIterator() :
		{}
		virtual ~BlobFieldIterator(){}

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
	class TBlobValueField : public ValueFieldBase<CommonLib::CBlob, _TBTree, BlobFieldIterator<_TBTree> >
	{
	public:
		typedef  BlobFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CBlob,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TBlobValueField( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) : TBase(pTransactions,pAlloc) 
		{

		}


		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal)
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

		typedef TBPBlobTree<uint64, IDBTransactions> TBTree;
		typedef TBlobValueField<TBTree> TField;

		BlobValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
		{}
		~BlobValueFieldHandler()
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