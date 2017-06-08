#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#include "../../embDBInternal.h"
#include "CommonLibrary/BoundaryBox.h"
#include "CommonLibrary/SpatialKey.h"
#include "../../BTreePlus/spatial/Point/SpatialPointQuery.h"
#include "../../BTreePlus/spatial/Rect/SpatialRectQuery.h"
#include "../../BTreePlus/spatial/Rect/RectSpatialBPMapTree.h"
#include "../IndexIteratorBase.h"



#include "../../BTreePlus/BaseNodeCompressor.h"
#include "../../BTreePlus/BaseDIffEncoder.h"
#include "../../Fields/BaseFieldEncoders.h"



#include "point/PointZOrderCompressor.h"
#include "rectangle/RectZOrderCompressor.h"
 





namespace embDB
{


	template<class TBTree, class TIterator, class _TSpObj>
	class TSpatialIndexIterator: public TIndexIteratorBase<TIterator, IIndexIterator>
	{
	public:
		typedef TIterator iterator;
		typedef _TSpObj TSpObj;
		typedef typename TBTree::TPointKey  TPointKey;
		typedef TIndexIteratorBase<iterator, IIndexIterator> TBase;
		TSpatialIndexIterator(iterator& it, IndexFiled *pIndex) : TBase(it, pIndex)
		{
		}
		virtual ~TSpatialIndexIterator(){}

		virtual bool getKey(CommonLib::CVariant* pVal)
		{
			TPointKey& zKey = this->m_ParentIt.key();
			TSpObj spObj; 
			zKey.getXY(spObj);
			pVal->setVal(spObj);
			return true;
		}
		virtual int64 getRowID()
		{
			return this->m_ParentIt.value();
		}
	};

	class ISpatialIndex : public IndexFiled
	{
	public:
		ISpatialIndex() {}
		virtual ~ISpatialIndex() {}
		virtual IIndexIteratorPtr find(const CommonLib::bbox& bbox, SpatialQueryMode mode = sqmIntersect) = 0;
		
	};


	class ISpatialIndexRect : public ISpatialIndex
	{
		virtual bool insert(const CommonLib::bbox& bbox, int64 nOID) = 0;
	};

	class ISpatialIndexPoint : public ISpatialIndex
	{
		virtual bool insert(double dX, double dY, int64 nOID) = 0;
	};


	template<class _TSpatialTree, class _TZCoordType, class _TSpObj, class _TIndex>
	class TStatialIndexBase : public _TIndex
	{
	public:
		TStatialIndexBase( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageNodeSize, uint32 nBTreeChacheSize) :
		  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, nBTreeChacheSize, nPageNodeSize), 
			  m_nBTreeRootPage(-1), m_dOffsetX(0.), m_dOffsetY(0.), m_nScaleX(1), m_nScaleY(1), m_dCalcScaleX(0.), m_dCalcScaleY(0.)
		  {

		  }

		  ~TStatialIndexBase()
		  {

		  }

		  typedef _TSpatialTree TSpatialTree;
		  typedef _TSpObj TSpatialObj;
		  typedef typename TSpatialTree::TSpatialIterator  TSpatialIterator;
		  typedef typename TSpatialTree::iterator  iterator;
		  typedef _TZCoordType	TZCoordType;
		  typedef typename TZCoordType::TPointType TPointType;
		  typedef TSpatialIndexIterator<TSpatialTree, TSpatialIterator, TSpatialObj> SpatialIndexIterator;
		  typedef TSpatialIndexIterator<TSpatialTree, iterator, TSpatialObj> TIndexIterator;


		  typedef typename 	TSpatialTree::TLeafCompressorParams TLeafCompressorParams;
		  typedef typename 	TSpatialTree::TInnerCompressorParams TInnerCompressorParams;
 		  virtual bool save()
		  {
			 return true;// return m_tree.saveBTreeInfo();
		  }
		  virtual bool load(int64 nBTreeRootPage, IDBShapeFieldHolder* pHolderField)
		  {
			  m_nBTreeRootPage = nBTreeRootPage;


			  m_dOffsetX = pHolderField->GetOffsetX();
			  m_dOffsetY = pHolderField->GetOffsetY();
			  m_nScaleX = pHolderField->GetScaleX();
			  m_nScaleY = pHolderField->GetScaleY();

			  m_extent = pHolderField->GetBoundingBox();
			  m_Type = pHolderField->GetPointType();
			  m_ShapeType = pHolderField->GetShapeType();

			  m_dCalcScaleX = 1/pow(10., m_nScaleX);
			  m_dCalcScaleY = 1/pow(10., m_nScaleY);
		


			  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.loadBTreeInfo(); 

		  }


		  virtual bool init(int64 nBTreeRootPage, EncoderType nType, uint32 nCompCalcError, bool bOnlineCalcCompSize )
		  {

			  TLeafCompressorParams leafCompParams;
			  TInnerCompressorParams innerCompParams;


			  leafCompParams.m_compressType = nType;
			  leafCompParams.m_bCalcOnlineSize = bOnlineCalcCompSize;
			  leafCompParams.m_nErrorCalc		  = nCompCalcError;

			  innerCompParams.m_compressType = nType;
			  innerCompParams.m_bCalcOnlineSize = bOnlineCalcCompSize;
			  innerCompParams.m_nErrorCalc		  =nCompCalcError;

			  m_nBTreeRootPage = nBTreeRootPage;
			//  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.init(m_nBTreeRootPage, &innerCompParams, &leafCompParams);
		  }

		  TSpatialTree* getBTree() {return &m_tree;}


		 
		  virtual indexTypes GetType() const  {return itSpatial;}

		  virtual IIndexIteratorPtr find(const CommonLib::bbox& extent, SpatialQueryMode mode = sqmIntersect)
		  {

			  CommonLib::bbox bb = extent;
			  if(bb.xMin < m_extent.xMin)
				  bb.xMin = m_extent.xMin;
			  if(bb.yMin < m_extent.yMin)
				  bb.yMin = m_extent.yMin;

			  if(bb.xMax > m_extent.xMax)
				  bb.xMax = m_extent.xMax;
			  if(bb.yMax > m_extent.yMax)
				  bb.yMax = m_extent.yMax;


 
			  TPointType xMin = TPointType((bb.xMin + m_dOffsetX) / m_dCalcScaleX);
			  TPointType yMin = TPointType((bb.yMin + m_dOffsetY) / m_dCalcScaleY);
			  TPointType xMax = TPointType((bb.xMax + m_dOffsetX) / m_dCalcScaleX);
			  TPointType yMax = TPointType((bb.yMax + m_dOffsetY) / m_dCalcScaleY);
			   
			 TSpatialIterator it =  m_tree.spatialQuery(xMin, yMin, xMax, yMax, mode);
			 SpatialIndexIterator *pSpIndexIterator = new SpatialIndexIterator(it, this);
			 return IIndexIteratorPtr(pSpIndexIterator);
		  }


		  virtual void GetSpatialObj(TSpatialObj& zVal, const CommonLib::CVariant* pValue) = 0;
		  virtual bool insert(const CommonLib::CVariant* pValue, uint64 nOID)
		  {
			  if(!pValue)
				  return false;
			  if(!pValue->isType<TSpatialObj>())
				  return false;

			  TSpatialObj zVal;
			  pValue->getVal(zVal);
			 return m_tree.insert(zVal, nOID);

		  }
		  virtual bool commit()
		  {
			  return m_tree.commit();
		  }


		  virtual bool insert (const CommonLib::CVariant* pIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		  {

			  iterator *pFromIterator = NULL;
			  iterator RetIterator;
			  if(pFromIterator)
			  {
				  TIndexIterator *pFieldIterator = (TIndexIterator*)pFromIter;
				  assert(pFromIterator);
				  pFromIterator = &pFieldIterator->m_ParentIt;
			  }

			  TSpatialObj rect;
			  GetSpatialObj(rect, pIndexKey);
			  bool bRet =  m_tree.insert(rect, nOID, pFromIterator, pRetIter ? &RetIterator : NULL);


			  if(pRetIter)
			  {
				  if(*pRetIter) 
					  ((TIndexIterator*)(*pRetIter))->set(RetIterator);
				  else
					  *pRetIter = new TIndexIterator(RetIterator, this); 
			  }
			  return bRet;
		  }
		  virtual bool update (const CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant* pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		  {
			  //FType val;
			  //pIndexKey->getVal(val);
			  //return  m_tree.update(nOID, val);
			  return true;

		  }
		  virtual bool remove (const CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		  {
			  return true;
		  }
		  virtual bool remove (IIndexIterator* pIter )
		  {
			  assert(pIter);
			  TIndexIterator *pSpIter = (TIndexIterator*)pIter;
			  iterator& it = pSpIter->m_ParentIt;

			   m_tree.remove(it);
			   return true;
		  }

		  virtual bool IsExsist(const CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  iterator it = m_tree.find(zVal);
			  return !it.isNull();
		  }

		  virtual IIndexIteratorPtr find(const CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  iterator it = m_tree.find(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }
		  virtual IIndexIteratorPtr lower_bound(const CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  iterator it = m_tree.lower_bound(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }
		  virtual IIndexIteratorPtr upper_bound(const CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  pIndexKey->getVal(val);
			   TZCoordType zVal(val);
			  iterator it = m_tree.upper_bound(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }

		  bool remove (const CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			   
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  return m_tree.remove(zVal);
		  }
		 

	protected:
		IDBTransaction* m_pDBTransactions;
		TSpatialTree m_tree;
		int64 m_nBTreeRootPage;

		double m_dOffsetX;
		double m_dOffsetY;
		byte m_nScaleX;
		byte m_nScaleY;

		double m_dCalcScaleX;
		double m_dCalcScaleY;

		eSpatialType m_Type;
		CommonLib::eShapeType m_ShapeType;
		CommonLib::bbox m_extent;
	};


	template<class _TSpatialTree, class _TZCoordType, class _TSpObj>
	class TStatialIndexRect : public TStatialIndexBase<_TSpatialTree, _TZCoordType, _TSpObj, ISpatialIndexRect>
	{
	public:
		typedef TStatialIndexBase<_TSpatialTree, _TZCoordType, _TSpObj,ISpatialIndexRect> TBase;
		typedef typename TBase::TSpatialObj	TSpatialObj;
		typedef typename TBase::TPointType	TPointType;

		TStatialIndexRect( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageNodeSize, uint32 nBTreeChacheSize) : TBase(pTransactions, pAlloc, nPageNodeSize, nBTreeChacheSize)
		{}

		~TStatialIndexRect()
		{}


		virtual void GetSpatialObj(TSpatialObj& Obj, const CommonLib::CVariant* pValue)
		{
			const CommonLib::IGeoShapePtr& pShape = pValue->Get<CommonLib::IGeoShapePtr>();
			CommonLib::bbox bbox = pShape->getBB();
			Obj.m_minX = TPointType((bbox.xMin + this->m_dOffsetX) / this->m_dCalcScaleX);
			Obj.m_maxX = TPointType((bbox.xMax + this->m_dOffsetX) / this->m_dCalcScaleX);
			Obj.m_minY = TPointType((bbox.yMin + this->m_dOffsetY) / this->m_dCalcScaleY);
			Obj.m_maxY = TPointType((bbox.yMax + this->m_dOffsetY) / this->m_dCalcScaleY);
		}
 

		virtual bool insert(const CommonLib::bbox& bbox, int64 nOID)
		{
			if(bbox.xMin < this->m_extent.xMin)
				return false;
			if(bbox.xMax > this->m_extent.xMax)
				return false;
			if(bbox.yMin < this->m_extent.yMin)
				return false;
			if(bbox.yMax > this->m_extent.yMax)
				return false;

			TPointType xMin = TPointType((bbox.xMin + this->m_dOffsetX) / this->m_dCalcScaleX);
			TPointType xMax = TPointType((bbox.xMax + this->m_dOffsetX) / this->m_dCalcScaleX);
			TPointType yMin = TPointType((bbox.yMin + this->m_dOffsetY) / this->m_dCalcScaleY);
			TPointType yMax = TPointType((bbox.yMax + this->m_dOffsetY) / this->m_dCalcScaleY);

			return this->m_tree.insert(xMin, yMin, xMax, yMax, nOID);

		}
	};


	template<class _TSpatialTree, class _TZCoordType, class _TSpObj>
	class TStatialIndexPoint : public TStatialIndexBase<_TSpatialTree, _TZCoordType,  _TSpObj, ISpatialIndexPoint>
	{
	public:
		typedef TStatialIndexBase<_TSpatialTree, _TZCoordType,  _TSpObj, ISpatialIndexPoint> TBase;
		typedef typename TBase::TSpatialObj	TSpatialObj;
		typedef typename TBase::TPointType	TPointType;

		TStatialIndexPoint( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, uint32 nPageNodeSize, uint32 nBTreeChacheSize) : TBase(pTransactions, pAlloc, nPageNodeSize, nBTreeChacheSize)
		{}

		~TStatialIndexPoint()
		{}
		 virtual void GetSpatialObj(TSpatialObj& zVal, const CommonLib::CVariant* pValue)
		 {

		 }

		virtual bool insert(double dX, double dY, int64 nOID)
		{
			if(dX < this->m_extent.xMin || dX > this->m_extent.xMax )
				return false;
			if(dY < this->m_extent.yMin || dY > this->m_extent.yMax )
				return false;

			TPointType x = TPointType((dX + this->m_dOffsetX) / this->m_dCalcScaleX);
			TPointType y = TPointType((dY + this->m_dOffsetY) / this->m_dCalcScaleY);
			
			return this->m_tree.insert(x, y, nOID);

		}
	};


	typedef TValueDiffEncoder<int64, int64, SignedNumLenEncoder64> TIndexOIDEncoder;


 
	typedef TBasePointCompressor<ZOrderPoint2DU16, uint16, TPointZOrderCompressor<ZOrderPoint2DU16, uint16, 16> > TPointSpatialEncoder16;
	typedef TBasePointCompressor<ZOrderPoint2DU32, uint32, TPointZOrderCompressor<ZOrderPoint2DU32, uint32, 32> > TPointSpatialEncoder32;
	typedef TBasePointCompressor<ZOrderPoint2DU64, uint64, TPointZOrderCompressor<ZOrderPoint2DU64, uint64, 64> > TPointSpatialEncoder64;
			


	typedef TBaseNodeCompressor<ZOrderPoint2DU16, int64, IDBTransaction, TPointSpatialEncoder16, TIndexOIDEncoder> TPointNodeCompressor16;
	typedef TBaseNodeCompressor<ZOrderPoint2DU32, int64, IDBTransaction, TPointSpatialEncoder32, TIndexOIDEncoder> TPointNodeCompressor32;
	typedef TBaseNodeCompressor<ZOrderPoint2DU64, int64, IDBTransaction, TPointSpatialEncoder64, TIndexOIDEncoder> TPointNodeCompressor64;




	typedef TBaseRectCompressor<ZOrderRect2DU16, uint16, TRectZOrderCompressor<ZOrderRect2DU16, uint16, 16> > TRectSpatialEncoder16;
	typedef TBaseRectCompressor<ZOrderRect2DU32, uint32, TRectZOrderCompressor<ZOrderRect2DU32, uint32, 32> > TRectSpatialEncoder32;
	typedef TBaseRectCompressor<ZOrderRect2DU64, uint64, TRectZOrderCompressor<ZOrderRect2DU64, uint64, 64> > TRectSpatialEncoder64;


	typedef TBaseNodeCompressor<ZOrderRect2DU16, int64, IDBTransaction, TRectSpatialEncoder16, TIndexOIDEncoder> TRectNodeCompressor16;
	typedef TBaseNodeCompressor<ZOrderRect2DU32, int64, IDBTransaction, TRectSpatialEncoder32, TIndexOIDEncoder> TRectNodeCompressor32;
	typedef TBaseNodeCompressor<ZOrderRect2DU64, int64, IDBTransaction, TRectSpatialEncoder64, TIndexOIDEncoder> TRectNodeCompressor64;


	typedef TBPPointSpatialMap<ZOrderPoint2DU16, int64,	ZPointComp<ZOrderPoint2DU16>, IDBTransaction, TPointNodeCompressor16, TPointNodeCompressor16> TBPMapPoint16;
	typedef TBPPointSpatialMap<ZOrderPoint2DU32, int64,	ZPointComp<ZOrderPoint2DU32>, IDBTransaction, TPointNodeCompressor32, TPointNodeCompressor32> TBPMapPoint32;
	typedef TBPPointSpatialMap<ZOrderPoint2DU64, int64,	ZPointComp64, IDBTransaction, TPointNodeCompressor64, TPointNodeCompressor64> TBPMapPoint64;


	typedef TBPRectSpatialMap<ZOrderRect2DU16, int64, ZPointComp<ZOrderRect2DU16>, IDBTransaction, TRectNodeCompressor16, TRectNodeCompressor16> TBPMapRect16;
	typedef TBPRectSpatialMap<ZOrderRect2DU32, int64, ZRect32Comp, IDBTransaction, TRectNodeCompressor32, TRectNodeCompressor32 > TBPMapRect32;
	typedef TBPRectSpatialMap<ZOrderRect2DU64, int64, ZRect64Comp, IDBTransaction, TRectNodeCompressor64, TRectNodeCompressor64> TBPMapRect64;


	typedef TStatialIndexPoint<TBPMapPoint16, ZOrderPoint2DU16, CommonLib::TPoint2Du16> TIndexPoint16;
	typedef TStatialIndexPoint<TBPMapPoint32, ZOrderPoint2DU32, CommonLib::TPoint2Du32> TIndexPoint32;
	typedef TStatialIndexPoint<TBPMapPoint64, ZOrderPoint2DU64, CommonLib::TPoint2Du64> TIndexPoint64;


	typedef TStatialIndexRect<TBPMapRect16, ZOrderRect2DU16, CommonLib::TRect2Du16> TIndexRect16;
	typedef TStatialIndexRect<TBPMapRect32, ZOrderRect2DU32, CommonLib::TRect2Du32> TIndexRect32;
	typedef TStatialIndexRect<TBPMapRect64, ZOrderRect2DU64, CommonLib::TRect2Du64> TIndexRect64;

}

#endif


 