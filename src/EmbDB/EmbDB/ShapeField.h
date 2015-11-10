#ifndef _EMBEDDED_DATABASE_SHAPE_FIELD_H_
#define _EMBEDDED_DATABASE_SHAPE_FIELD_H_

#include "ValueField.h"
#include "ShapeTree.h"
#include "FieldIteratorBase.h"
namespace embDB
{

	template<class TBTree>
	class ShapeFieldIterator: public TFieldIteratorBase<TBTree, IFieldIterator>
	{
	public:
		typedef typename TBTree::iterator iterator;
		typedef TFieldIteratorBase<TBTree, IFieldIterator> TBase;

		ShapeFieldIterator(iterator& it, IValueField* pField) 
			: TBase(it, pField)

		{

		}
		ShapeFieldIterator() :
		{}
		virtual ~ShapeFieldIterator(){}

		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sBlobVal& sBlob = m_ParentIt.value();
			CommonLib::IGeoShapePtr shape(new CommonLib::CGeoShape());

			((TBTree*)m_ParentIt.m_pTree)->convert(sBlob, shape);


			pVal->setVal(shape);
			return true;
		}

		virtual bool getVal(CommonLib::IGeoShapePtr& shape)
		{
			((TBTree*)m_ParentIt.m_pTree)->convert(m_ParentIt.value(), shape);
			return true;
		}
		
	};


	template<class _TBTree>
	class TShapeValueField : public ValueFieldBase<CommonLib::IGeoShapePtr, _TBTree, ShapeFieldIterator<_TBTree> >
	{
	public:
		typedef typename TBTree::iterator  iterator;
		typedef  ShapeFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::IGeoShapePtr,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
	

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

		TShapeValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, pFieldInfo) 
		{

		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			TBTree::iterator it = m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::IGeoShapePtr shape( new CommonLib::CGeoShape(m_pAlloc));
			m_tree.convert(it.value(), shape);

			pFieldVal->setVal(shape);
			return true;
		}



	};


	class ShapeValueFieldHandler : public CDBFieldHandlerBase
	{
	public:

		typedef TBPShapeTree<int64, IDBTransaction> TBTree;
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
			compParams.SetMaxPageBlobSize(pTran->getPageSize()/15);
			compParams.save(pTran);

			if(!CDBFieldHandlerBase::save<TField>(nAddr, pTran, m_pAlloc, FIELD_PAGE, FIELD_INFO_PAGE, -1, pLeafCompRootPage->getAddr()))
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
		virtual eDataTypes getType() const
		{
			return (eDataTypes)m_SpatialFi.m_nFieldType;
		}
		virtual const CommonLib::CString& getName() const
		{
			return m_SpatialFi.m_sFieldName;
		}
		virtual const CommonLib::CString& getAlias() const
		{
			return m_SpatialFi.m_sFieldAlias;
		}
		virtual uint32 GetLength()	const
		{
			return m_SpatialFi.m_nLenField;
		}
		virtual bool GetIsNotEmpty() const
		{
			return (m_SpatialFi.m_nFieldDataType&dteIsNotEmpty) != 0;
		}
	private:
		sSpatialFieldInfo m_SpatialFi;


	};

}

#endif