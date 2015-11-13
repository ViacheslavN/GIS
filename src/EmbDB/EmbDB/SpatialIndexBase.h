#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#include "embDBInternal.h"
#include "CommonLibrary/BoundaryBox.h"
#include "CommonLibrary/SpatialKey.h"
#include "SpatialPointQuery.h"
#include "SpatialRectQuery.h"
#include "RectSpatialBPMapTree.h"
#include "PoinMapLeafCompressor64.h"
#include "PoinMapInnerCompressor64.h"
#include "IndexIteratorBase.h"
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
			TPointKey& zKey = m_ParentIt.key();
			TSpObj spObj; 
			zKey.getXY(spObj);
			pVal->setVal(spObj);
			return true;
		}
		virtual int64 getRowID()
		{
			return m_ParentIt.value();
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
		TStatialIndexBase( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) :
		  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, 100), 
			  m_nBTreeRootPage(-1)
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
 		  virtual bool save()
		  {
			  return m_tree.saveBTreeInfo();
		  }
		  virtual bool load(int64 nAddr, eTransactionType type)
		  {

			  int64 m_nFieldInfoPage = nAddr;
			  FilePagePtr pPage = m_pDBTransactions->getFilePage(nAddr);
			  if(!pPage.get())
				  return false;
			  CommonLib::FxMemoryReadStream stream;
			  stream.attach(pPage->getRowData(), pPage->getPageSize());
			  sFilePageHeader header(stream);
			  if(!header.isValid())
			  {
				  m_pDBTransactions->error(_T("IndexField: Page %I64d Error CRC for node page"), pPage->getAddr()); //TO DO log error
				  return false;
			  }
			  if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != TABLE_INDEX_PAGE)
			  {
				  m_pDBTransactions->error(_T("IndexField: Page %I64d Not index info page"), pPage->getAddr()); //TO DO log error
				  return false;
			  }
			  stream.read(m_nBTreeRootPage);
			  stream.read(m_dOffsetX);
			  stream.read(m_dOffsetY);
			  stream.read(m_dScaleX);
			  stream.read(m_dScaleY);
			  stream.read(m_extent.xMin);
			  stream.read(m_extent.yMin);
			  stream.read(m_extent.xMax);
			  stream.read(m_extent.yMax);
 
			  m_Type = (eDataTypes)stream.readIntu32();
			  m_ShapeType = (CommonLib::eShapeType)stream.readIntu32();
			  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.loadBTreeInfo(); 
		  }


		  virtual bool init(int64 nBTreeRootPage, sSpatialFieldInfo* pSPFi)
		  {

			  m_nBTreeRootPage = nBTreeRootPage;
			  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.init();
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

			  TPointType xMin = TPointType((bb.xMin + m_dOffsetX) / m_dScaleX);
			  TPointType yMin = TPointType((bb.yMin + m_dOffsetY) / m_dScaleY);
			  TPointType xMax = TPointType((bb.xMax + m_dOffsetX) / m_dScaleX);
			  TPointType yMax = TPointType((bb.yMax + m_dOffsetY) / m_dScaleY);
			   
			 TSpatialIterator it =  m_tree.spatialQuery(xMin, yMin, xMax, yMax, mode);
			 SpatialIndexIterator *pSpIndexIterator = new SpatialIndexIterator(it, this);
			 return IIndexIteratorPtr(pSpIndexIterator);
		  }


		  virtual void GetSpatialObj(TSpatialObj& zVal, CommonLib::CVariant* pValue) = 0;
		  virtual bool insert(CommonLib::CVariant* pValue, uint64 nOID)
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


		  virtual bool insert (CommonLib::CVariant* pIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
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
		  virtual bool update (CommonLib::CVariant* pOldIndexKey, CommonLib::CVariant* pNewIndexKey, int64 nOID, IIndexIterator* pFromIter = NULL, IIndexIterator** pRetIter = NULL)
		  {
			  //FType val;
			  //pIndexKey->getVal(val);
			  //return  m_tree.update(nOID, val);
			  return true;

		  }
		  virtual bool remove (CommonLib::CVariant* pIndexKey, IIndexIterator** pRetIter = NULL)
		  {
			  return true;
		  }
		  virtual bool remove (IIndexIterator* pIter )
		  {
			  return true;
		  }
		  virtual IIndexIteratorPtr find(CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  iterator it = m_tree.find(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }
		  virtual IIndexIteratorPtr lower_bound(CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  pIndexKey->getVal(val);
			  TZCoordType zVal(val);
			  iterator it = m_tree.lower_bound(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }
		  virtual IIndexIteratorPtr upper_bound(CommonLib::CVariant* pIndexKey)
		  {
			  TSpatialObj val;
			  pIndexKey->getVal(val);
			   TZCoordType zVal(val);
			  iterator it = m_tree.upper_bound(zVal);
			  TIndexIterator *pIndexIterator = new TIndexIterator(it, this);
			  return IIndexIteratorPtr(pIndexIterator);
		  }

		  bool remove (CommonLib::CVariant* pIndexKey)
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
		double m_dScaleX;
		double m_dScaleY;
		eDataTypes m_Type;
		CommonLib::eShapeType m_ShapeType;
		CommonLib::bbox m_extent;
	};


	template<class _TSpatialTree, class _TZCoordType, class _TSpObj>
	class TStatialIndexRect : public TStatialIndexBase<_TSpatialTree, _TZCoordType, _TSpObj, ISpatialIndexRect>
	{
	public:
		typedef TStatialIndexBase<_TSpatialTree, _TZCoordType, _TSpObj,ISpatialIndexRect> TBase;



		TStatialIndexRect( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) : TBase(pTransactions, pAlloc)
		{}

		~TStatialIndexRect()
		{}


		virtual void GetSpatialObj(TSpatialObj& Obj, CommonLib::CVariant* pValue)
		{
			CommonLib::IGeoShapePtr& pShape = pValue->Get<CommonLib::IGeoShapePtr>();
			CommonLib::bbox bbox = pShape->getBB();
			Obj.m_minX = TPointType((bbox.xMin + m_dOffsetX) / m_dScaleX);
			Obj.m_maxX = TPointType((bbox.xMax + m_dOffsetX) / m_dScaleX);
			Obj.m_minY = TPointType((bbox.yMin + m_dOffsetY) / m_dScaleY);
			Obj.m_maxY = TPointType((bbox.yMax + m_dOffsetY) / m_dScaleY);
		}
 

		virtual bool insert(const CommonLib::bbox& bbox, int64 nOID)
		{
			if(bbox.xMin < m_extent.xMin)
				return false;
			if(bbox.xMax > m_extent.xMax)
				return false;
			if(bbox.yMin < m_extent.yMin)
				return false;
			if(bbox.yMax > m_extent.yMax)
				return false;

			TPointType xMin = TPointType((bbox.xMin + m_dOffsetX) / m_dScaleX);
			TPointType xMax = TPointType((bbox.xMax + m_dOffsetX) / m_dScaleX);
			TPointType yMin = TPointType((bbox.yMin + m_dOffsetY) / m_dScaleY);
			TPointType yMax = TPointType((bbox.yMax + m_dOffsetY) / m_dScaleY);

			return m_tree.insert(xMin, yMin, xMax, yMax, nOID);

		}
	};


	template<class _TSpatialTree, class _TZCoordType, class _TSpObj>
	class TStatialIndexPoint : public TStatialIndexBase<_TSpatialTree, _TZCoordType,  _TSpObj, ISpatialIndexPoint>
	{
	public:
		typedef TStatialIndexBase<_TSpatialTree, _TZCoordType,  _TSpObj, ISpatialIndexPoint> TBase;

		TStatialIndexPoint( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc) : TBase(pTransactions, pAlloc)
		{}

		~TStatialIndexPoint()
		{}
		 virtual void GetSpatialObj(TSpatialObj& zVal, CommonLib::CVariant* pValue)
		 {

		 }

		virtual bool insert(double dX, double dY, int64 nOID)
		{
			if(dX < m_extent.xMin || dX > m_extent.xMax )
				return false;
			if(dY < m_extent.yMin || dY > m_extent.yMax )
				return false;

			TPointType x = TPointType((dX + m_dOffsetX) / m_dScaleX);
			TPointType y = TPointType((dY + m_dOffsetY) / m_dScaleY);
			
			return m_tree.insert(x, y, nOID);

		}
	};


	typedef TBPPointSpatialMap<ZOrderPoint2DU16, int64,
		ZPointComp<ZOrderPoint2DU16> > TBPMapPoint16;	
	typedef TBPPointSpatialMap<ZOrderPoint2DU32, int64,
		ZPointComp<ZOrderPoint2DU32> > TBPMapPoint32;

	typedef TBPPointSpatialMap<ZOrderPoint2DU64, 	int64,	ZPointComp64, IDBTransaction,
		BPSpatialPointInnerNodeSimpleCompressor64,
		BPSpatialPointLeafNodeMapSimpleCompressor64<int64> > TBPMapPoint64;


	typedef TBPRectSpatialMap<ZOrderRect2DU16, int64,
		ZPointComp<ZOrderRect2DU16> 	> TBPMapRect16;

	typedef TBPRectSpatialMap<ZOrderRect2DU32, int64,
		ZRect32Comp, IDBTransaction,
		BPSpatialRectInnerNodeSimpleCompressor< ZOrderRect2DU32>,	
		 BPSpatialRectLeafNodeMapSimpleCompressor<ZOrderRect2DU32, int64> > TBPMapRect32;

	typedef TBPRectSpatialMap<ZOrderRect2DU64, int64,
		ZRect64Comp, IDBTransaction,
		BPSpatialRectInnerNodeSimpleCompressor<ZOrderRect2DU64 >,	
		 BPSpatialRectLeafNodeMapSimpleCompressor<ZOrderRect2DU64, int64 > > TBPMapRect64;


	typedef TStatialIndexPoint<TBPMapPoint16, ZOrderPoint2DU16, CommonLib::TPoint2Du16> TIndexPoint16;
	typedef TStatialIndexPoint<TBPMapPoint32, ZOrderPoint2DU32, CommonLib::TPoint2Du32> TIndexPoint32;
	typedef TStatialIndexPoint<TBPMapPoint64, ZOrderPoint2DU64, CommonLib::TPoint2Du64> TIndexPoint64;


	typedef TStatialIndexRect<TBPMapRect16, ZOrderRect2DU16, CommonLib::TRect2Du16> TIndexRect16;
	typedef TStatialIndexRect<TBPMapRect32, ZOrderRect2DU32, CommonLib::TRect2Du32> TIndexRect32;
	typedef TStatialIndexRect<TBPMapRect64, ZOrderRect2DU64, CommonLib::TRect2Du64> TIndexRect64;

}

#endif


 