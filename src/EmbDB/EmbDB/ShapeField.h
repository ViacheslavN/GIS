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
		ShapeFieldIterator()
		{}
		virtual ~ShapeFieldIterator(){}

		virtual bool getVal(CommonLib::CVariant* pVal)
		{
			const sBlobVal& sBlob = this->m_ParentIt.value();
			CommonLib::IGeoShapePtr shape(new CommonLib::CGeoShape());

			((TBTree*)this->m_ParentIt.m_pTree)->convert(sBlob, shape);


			pVal->setVal(shape);
			return true;
		}

		virtual bool getVal(CommonLib::IGeoShapePtr& shape)
		{
			((TBTree*)this->m_ParentIt.m_pTree)->convert(this->m_ParentIt.value(), shape);
			return true;
		}
		
	};


	template<class _TBTree>
	class TShapeValueField : public ValueFieldBase<CommonLib::IGeoShapePtr, _TBTree, ShapeFieldIterator<_TBTree> >
	{
	public:
	
		typedef  ShapeFieldIterator<_TBTree> TFieldIterator;
		typedef ValueFieldBase<CommonLib::IGeoShapePtr,_TBTree, TFieldIterator> TBase;
		typedef typename TBase::TBTree TBTree;
		typedef typename TBTree::iterator  iterator;

		typedef typename TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TBTree::TLeafCompressorParams TLeafCompressorParams;

	/*	TShapeValueField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, const sFieldInfo* pFieldInfo) : TBase(pTransactions, pAlloc, 8192) 
		{

		}*/

		TShapeValueField(IDBFieldHandler* pFieldHandler,  IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageSize, uint32 nBTreeChacheSize) : TBase(pFieldHandler, pTransactions, pAlloc, nPageSize, nBTreeChacheSize) 
		{

		}

		~TShapeValueField()
		{

		}


		virtual bool find(int64 nOID, CommonLib::CVariant* pFieldVal)
		{
			typename TBTree::iterator it = this->m_tree.find(nOID);
			if(it.isNull())
				return false;

			CommonLib::IGeoShapePtr shape( new CommonLib::CGeoShape(this->m_pAlloc)); //TO DO use shape cache
			this->m_tree.convert(it.value(), shape);

			pFieldVal->setVal(shape);
			return true;
		}


		virtual bool remove(int64 nOID)
		{

			if(m_pIndex.get())
			{
				 assert(m_pIndex->GetType() == itSpatial);
				 ISpatialIndex* pSpatialIndex = dynamic_cast<ISpatialIndex*>(m_pIndex.get());
				 assert(m_pIndex.get());

				 typename TBTree::iterator it = this->m_tree.find(nOID);
				 if(it.isNull())
					 return false;

				 CommonLib::IGeoShapePtr shape( new CommonLib::CGeoShape(this->m_pAlloc)); //TO DO use shape cache
				 this->m_tree.convert(it.value(), shape);
				
				 m_tree.remove(it);
				 IIndexIteratorPtr pIndexIterator = pSpatialIndex->find(shape->getBB(), sqmByFeature);
				 if(pIndexIterator->isNull())
				 {
		
					 const CommonLib::bbox& bb shape->getBB();
					 m_pDBTransactions->error(L"Not Found shape in index RowID: %I64d, bbox xMin: %f, yMin: %f, xMax: %f, yMax: %f", nOID, bb.xMin, bb.yMax, bb.xMax, bb.yMax);
					 return false;
				 }
				 while(!pIndexIterator->isNull())
				 {
					 if(pIndexIterator->getRowID() == nOID)
					 {
						 return pSpatialIndex->remove(pIndexIterator.get());
					
					 }
					 pIndexIterator->next();
				 }
				 const CommonLib::bbox& bb shape->getBB();
				 m_pDBTransactions->error(L"Not Found shape in index RowID: %I64d, bbox xMin: %f, yMin: %f, xMax: %f, yMax: %f", nOID, bb.xMin, bb.yMax, bb.xMax, bb.yMax);
				 return false;

			}
			else 
				return m_tree.remove(nOID);



		}
 
	};


	class ShapeValueFieldHandler : public CDBFieldHandlerBase<IDBShapeFieldHandler>
	{
	public:

		typedef CDBFieldHandlerBase<IDBShapeFieldHandler> TBase;
		typedef TBPShapeTree<IDBTransaction> TBTree;
		typedef TShapeValueField<TBTree> TField;
		typedef TBTree::TInnerCompressorParams TInnerCompressorParams;
		typedef TBTree::TLeafCompressorParams TLeafCompressorParams;
		ShapeValueFieldHandler(CommonLib::alloc_t* pAlloc, const SFieldProp *pFP, int64 nPageAdd) : 
			TBase(pAlloc, pFP, nPageAdd)
		{}
		~ShapeValueFieldHandler()
		{}

		virtual bool save(CommonLib::IWriteStream* pStream,   IDBTransaction *pTran)
		{


			TBTree::TLeafCompressorParams compParams;
			compParams.SetMaxPageBlobSize(m_nPageSize/10); 
			compParams.SetCoordType(m_PointType);
			compParams.SetShapeType(m_ShapeType);
			compParams.SetScaleX(m_nScaleX);
			compParams.SetScaleY(m_nScaleY);
			compParams.SetOffsetX(m_dOffsetX);
			compParams.SetOffsetY(m_dOffsetY);


			TInnerCompressorParams innerCompParams;

			innerCompParams.m_compressType = m_CompressType;
			innerCompParams.m_bCalcOnlineSize = m_bOnlineCalcCompSize;
			innerCompParams.m_nErrorCalc		  = m_nCompCalcError;

			TBase::base_save<TField, TInnerCompressorParams, TLeafCompressorParams>(pStream, pTran, m_pAlloc, &innerCompParams, &compParams);

			pStream->write((uint32)m_PointType);
			pStream->write((uint32)m_ShapeType);
			pStream->write((uint32)m_Units);
			pStream->write(m_dOffsetX);
			pStream->write(m_dOffsetY);
			pStream->write(m_nScaleX);
			pStream->write(m_nScaleY);
			pStream->write(m_extent.xMin);
			pStream->write(m_extent.xMax);
			pStream->write(m_extent.yMin);
			pStream->write(m_extent.yMax);
			TBase::initField<TField, TInnerCompressorParams, TLeafCompressorParams>(pTran, m_pAlloc, &innerCompParams, &compParams);
			
		
			return true;

		}

		virtual bool load(CommonLib::IReadStream* pStream, IDBStorage *pStorage)
		{
			TBase::base_load(pStream, pStorage);
		 
			m_PointType = (eSpatialType)pStream->readIntu32();
			m_ShapeType = (CommonLib::eShapeType)pStream->readIntu32();
			m_Units = (eSpatialCoordinatesUnits)pStream->readIntu32();
			pStream->read(m_dOffsetX);
			pStream->read(m_dOffsetY);
			pStream->read(m_nScaleX);
			pStream->read(m_nScaleY);
			pStream->read(m_extent.xMin);
			pStream->read(m_extent.xMax);
			pStream->read(m_extent.yMin);
			pStream->read(m_extent.yMax);

			return true;
		}
		virtual IValueFieldPtr getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage)
		{
			return CDBFieldHandlerBase::getValueField<TField>(pTransactions, pStorage);
		}
		virtual bool release(IValueField* pField)
		{
			/*TField* pOIDField = (TField*)pField;

			TField::TBTree *pBTree = pOIDField->getBTree();

			delete pField;*/
			return true;
		}


		virtual  CommonLib::eShapeType GetShapeType() const
		{
			return m_ShapeType;
		}
		virtual eSpatialType GetPointType() const 
		{
			return m_PointType;
		}
		virtual eSpatialCoordinatesUnits GetUnits() const
		{
			return m_Units;
		}
		virtual const CommonLib::bbox& GetBoundingBox() const
		{
			return m_extent;
		}
		virtual double GetOffsetX()  const 
		{
			return m_dOffsetX;
		}
		virtual double GetOffsetY()  const
		{
			return m_dOffsetY;
		}
		virtual byte GetScaleX()  const 
		{
			return m_nScaleX;
		}
		virtual byte GetScaleY()  const 
		{
			return m_nScaleY;
		}



		void SetShapeType( CommonLib::eShapeType ShapeType) 
		{
			m_ShapeType = ShapeType;
		}
		void SetPointType( eSpatialType SpatialType)  
		{
			m_PointType = SpatialType;
		}
		void SetUnits(eSpatialCoordinatesUnits Units) 
		{
			m_Units = Units;
		}
		void SetBoundingBox(const CommonLib::bbox& bb) 
		{
			m_extent = bb;
		}
		void SetOffsetX(double dOffsetX)   
		{
			m_dOffsetX = dOffsetX;
		}
		void SetOffsetY(double dOffsetY)  
		{
			m_dOffsetY = dOffsetY;
		}
		void SetScaleX(byte nScaleX)   
		{
			m_nScaleX = nScaleX;
		}
		void SetScaleY(byte nScaleY)   
		{
			m_nScaleY = nScaleY;
		}
	private:
 
		 CommonLib::eShapeType m_ShapeType;
		eSpatialType m_PointType;
		CommonLib::bbox  m_extent;
		eSpatialCoordinatesUnits m_Units;

		double m_dOffsetX;
		double m_dOffsetY;
		byte m_nScaleX;
		byte m_nScaleY;
 
	};

}

#endif