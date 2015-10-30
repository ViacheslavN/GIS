#ifndef _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#define _EMBEDDED_DATABASE_B_SPATIAL_INDEX_H_
#include "IDBField.h"
#include "CommonLibrary/BoundaryBox.h"
#include "CommonLibrary/SpatialKey.h"
#include "SpatialPointQuery.h"
#include "SpatialRectQuery.h"
#include "RectSpatialBPMapTree.h"
#include "PoinMapLeafCompressor64.h"

namespace embDB
{

	class ISpatialIndexFiled : public IndexFiled
	{
	public:
		ISpatialIndexFiled() {}
		virtual ~ISpatialIndexFiled() {}
		virtual IndexIteratorPtr find(const CommonLib::bbox& bbox, SpatialQueryMode mode = sqmIntersect) = 0;
	};


	template<class _TSpatialTree, class _TCoordType>
	class CStatialIndexBase : public ISpatialIndexFiled
	{
	public:
		CStatialIndexBase( IDBTransactions* pTransactions, CommonLib::alloc_t* pAlloc) :
		  m_pDBTransactions(pTransactions),
			  m_tree(-1, pTransactions, pAlloc, 100), 
			  m_nBTreeRootPage(-1)
		  {

		  }

		  typedef _TSpatialTree TSpatialTree;
		  typedef _TCoordType	TCoordType;
 		  virtual bool save()
		  {
			  return m_tree.saveBTreeInfo();
		  }
		  virtual bool load(int64 nAddr, eTransactionsType type)
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
			  if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != INDEX_INFO_PAGE)
			  {
				  m_pDBTransactions->error(_T("IndexField: Page %I64d Not field info page"), pPage->getAddr()); //TO DO log error
				  return false;
			  }
			  stream.read(m_nBTreeRootPage);
			  stream.read(m_dOffsetX);
			  stream.read(m_dOffsetY);
			  stream.read(m_dScaleX);
			  stream.read(m_dScaleY);
			  stream.read(m_bbox.xMin);
			  stream.read(m_bbox.yMin);
			  stream.read(m_bbox.xMax);
			  stream.read(m_bbox.yMax);
 
			  m_Type = (embDB::eDataTypes)stream.readIntu32();
			  m_ShapeType = (embDB::eShapeType)stream.readIntu32();
			  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.loadBTreeInfo(); 
		  }


		  virtual bool init(int64 nBTreeRootPage)
		  {

			  m_nBTreeRootPage = nBTreeRootPage;
			  m_tree.setRootPage(m_nBTreeRootPage);
			  return m_tree.init();
		  }

		  TBTree* getBTree() {return &m_tree;}
	protected:
		IDBTransactions* m_pDBTransactions;
		TBTree m_tree;
		int64 m_nBTreeRootPage;

		double m_dOffsetX;
		double m_dOffsetY;
		double m_dScaleX;
		double m_dScaleY;
		embDB::eDataTypes m_Type;
		CommonLib::eShapeType m_ShapeType;
		CommonLib::bbox m_bbox;
	};

}

#endif


 