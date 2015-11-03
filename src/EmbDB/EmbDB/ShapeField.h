#ifndef _EMBEDDED_DATABASE_SHAPE_FIELD_H_
#define _EMBEDDED_DATABASE_SHAPE_FIELD_H_

#include "ValueField.h"
#include "ShapeTree.h"

namespace embDB
{

	template<class TBTree>
	class ShapeFieldIterator: public IFieldIterator
	{
	public:
		typedef typename TBTree::iterator  iterator;

		ShapeFieldIterator(iterator& it) 
			: m_ParentIt(it)

		{

		}
		ShapeFieldIterator() :
		{}
		virtual ~ShapeFieldIterator(){}

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
			CommonLib::CGeoShape shape;

			((TBTree*)m_ParentIt.m_pTree)->convert(sBlob, shape);


			pVal->setVal(shape);
			return true;
		}

		virtual bool getVal(CommonLib::CGeoShape& shape)
		{
			((TBTree*)m_ParentIt.m_pTree)->convert(m_ParentIt.value(), shape);
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
		CommonLib::alloc_t *m_pAlloc;

	};


	template<class _TBTree>
	class TShapeValueField : public ValueFieldBase<CommonLib::CGeoShape, _TBTree, ShapeFieldIterator<_TBTree> >
	{
	public:
		typedef  ShapeFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::CGeoShape,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TShapeValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, pFieldInfo) 
		{

		}


		virtual bool find(uint64 nOID, CommonLib::CVariant* pFieldVal)
		{
			TBTree::iterator it = m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::CGeoShape shape(m_pAlloc);
			m_tree.convert(it.value(), shape);

			pFieldVal->setVal(shape);
			return true;
		}



	};


	class ShapeValueFieldHandler : public CDBFieldHandlerBase
	{
	public:

		typedef TBPShapeTree<uint64, IDBTransaction> TBTree;
		typedef TShapeValueField<TBTree> TField;

		ShapeValueFieldHandler(CommonLib::alloc_t* pAlloc) : CDBFieldHandlerBase(pAlloc)
		{}
		~ShapeValueFieldHandler()
		{}

		virtual bool save(int64 nAddr, IDBTransaction *pTran)
		{

			FilePagePtr pLeafCompRootPage = pTran->getNewPage();
			TBTree::TLeafCompressorParams compParams;
			compParams.setRootPage(pLeafCompRootPage->getAddr());
			compParams.SetParams(m_SpatialFi);
			compParams.save(pTran);

			if(!CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE))
				return false;

			

			return true;

		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			TField * pField = new  TField(pTransactions, m_pAlloc, (sFieldInfo*)&m_SpatialFi);
			pField->load(m_SpatialFi.m_nFieldPage, pTransactions->getType());
			if(m_pIndexHandler.get())
			{
				IndexFiledPtr pIndex = m_pIndexHandler->getIndex(pTransactions, pStorage);
				pField->SetIndex(pIndex.get());
			}
			return IValueFieldPtr(pField);	
		}
		virtual bool release(IValueField* pField)
		{
			/*TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;*/
			return true;
		}

		virtual sFieldInfo* getFieldInfoType()
		{
			return (sFieldInfo*)&m_SpatialFi;
		}
		virtual void setFieldInfoType(sFieldInfo* fi)
		{
			assert(fi);
			sSpatialFieldInfo *pSpFi = dynamic_cast<sSpatialFieldInfo *>(fi);
			assert(pSpFi);
			if(pSpFi)
				m_SpatialFi = *pSpFi;
		}
	private:
		sSpatialFieldInfo m_SpatialFi;


	};

}

#endif