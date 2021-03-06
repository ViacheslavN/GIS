#ifndef _EMBEDDED_DATABASE_BLOB_FIELD_H_
#define _EMBEDDED_DATABASE_BLOB_FIELD_H_

#include "DB/Fields/ValueField.h"
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
		BlobFieldIterator()
		{}
		virtual ~BlobFieldIterator(){}

		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sBlobVal& sBlob = this->m_ParentIt.value();
			CommonLib::CBlob blob;

			((TBTree*)this->m_ParentIt.m_pTree)->convert(sBlob, blob);


			pVal->setVal(blob);
			return true;
		}

		virtual bool getVal(CommonLib::CBlob& blob)
		{
			((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), blob);
			return true;
		}
	};


	template<class _TBTree>
	class TBlobVarConvertor
	{
	public:
		TBlobVarConvertor() : m_pTree(nullptr)
		{}

		void convert(CommonLib::CVariant *pVar, const sBlobVal& value)
		{
			CommonLib::CBlob blob(this->m_pAlloc);
			m_pTree->convert(value, blob);
			pVar->setVal(blob);
		}
		void Init(_TBTree *pTree, CommonLib::alloc_t *pAlloc)
		{
			m_pTree = pTree;
			m_pAlloc = pAlloc;
		}
	private:
		_TBTree *m_pTree;
		CommonLib::alloc_t *m_pAlloc;
	};

	template<class _TBTree>
	class TBlobValueField : public ValueFieldBase<CommonLib::CBlob, _TBTree, BlobFieldIterator<_TBTree>, TBlobVarConvertor<_TBTree> >
	{
	public:
		typedef  BlobFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CBlob,_TBTree, TFieldIterator, TBlobVarConvertor<_TBTree> > TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TBlobValueField(IDBFieldHolder* pFieldHolder,  IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) : TBase(pFieldHolder, pTransactions, pAlloc, nPageSize, nBTreeChacheSize) 
		{
			this->m_ConvertTypeToVar.Init(&this->m_tree, this->m_pAlloc);
		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			typename TBTree::iterator it = this->m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::CBlob blob(this->m_pAlloc);
			this->m_tree.convert(it.value(), blob);

			pFieldVal->setVal(blob);
			return true;
		}

		virtual bool removeWithIndex(int64 nOID)
		{
			assert(false);
			return false;
		}

	};


	class BlobValueFieldHolder : public CDBFieldHolderBase<IDBFieldHolder>
	{
	public:
		typedef CDBFieldHolderBase<IDBFieldHolder> TBase;
		typedef TBPBlobTree<IDBTransaction> TBTree;
		typedef TBlobValueField<TBTree> TField;

		typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef TBTree::TLeafCompressorParams TLeafCompressorParams;

		BlobValueFieldHolder(CommonLib::alloc_t* pAlloc, const SFieldProp* pFP, int64 nPageAdd) : 
			TBase(pAlloc, pFP, nPageAdd)
		{}
		~BlobValueFieldHolder()
		{}

		/*virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{
			return CDBFieldHolderBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE);
		}*/
		virtual bool save(CommonLib::IWriteStream* pStream,  IDBTransaction *pTran)
		{
			BlobFieldCompressorParams leafCompParams;
			leafCompParams.SetMaxPageBlobSize(m_nPageSize/20);


			TInnerCompressorParams innerCompParams;

			innerCompParams.m_compressType = m_CompressType;
			innerCompParams.m_bCalcOnlineSize = m_bOnlineCalcCompSize;
			innerCompParams.m_nErrorCalc		  = m_nCompCalcError;
	
			return CDBFieldHolderBase::save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &innerCompParams, &leafCompParams);
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHolderBase::getValueField<TField>(pTransactions, pStorage);
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