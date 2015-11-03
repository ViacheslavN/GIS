#ifndef _EMBEDDED_DATABASE_SPATIAL_OID_FIELD_H_
#define _EMBEDDED_DATABASE_SPATIAL_OID_FIELD_H_
#include "PointSpatialBPMapTree.h"
#include "DBFieldInfo.h"
#include "CommonLibrary/SpatialKey.h"
#include "SpatialPointQuery.h"
#include "SpatialRectQuery.h"
#include "RectSpatialBPMapTree.h"
#include "PoinMapLeafCompressor64.h"
#include "PoinMapInnerCompressor64.h"
namespace embDB
{


	template <class _TSpatialBPTree, class _TPointType, class _TSpatialObject>
	class OIDSpatialField/* : public IOIDFiled*/
	{
	public:
		typedef _TSpatialBPTree TSpatialBPTree;
		typedef _TPointType 	TPointType;
		typedef CommonLib::TRect2D<TPointType>   TRect;
		typedef _TSpatialObject TSpatialObject;

		OIDSpatialField( IDBTransaction* pTransactions, CommonLib::alloc_t* pAlloc, sFieldInfo fi) :
			m_pDBTransactions(pTransactions),
			m_SpatialTree(-1, pTransactions, pAlloc, 100), 
			m_nBTreeRootPage(-1), m_fi(fi), m_shiftX(0), m_shiftY(0)
		{

		}

		virtual bool save()
		{
			return m_SpatialTree.saveBTreeInfo();
		}
		virtual bool load(int64 nAddr)
		{

			int64 m_nFieldInfoPage = nAddr;
			FilePagePtr pPage(m_pDBTransactions->getFilePage(nAddr));
			if(!pPage.get())
				return false;
			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream);
			if(!header.isValid())
			{
				m_pDBTransactions->error(_T("OIDField: Page %I64d Error CRC for node page"), pPage->getAddr()); //TO DO log error
				return false;
			}
			if(header.m_nObjectPageType != FIELD_PAGE || header.m_nSubObjectPageType != FIELD_INFO_PAGE)
			{
				m_pDBTransactions->error(_T("OIDField: Page %I64d Not field info page"), pPage->getAddr()); //TO DO log error
				return false;
			}
			stream.read(m_nBTreeRootPage);
			stream.read(m_shiftX);
			stream.read(m_shiftY);
			stream.read(m_Extent.m_minX);
			stream.read(m_Extent.m_minY);
			stream.read(m_Extent.m_maxX);
			stream.read(m_Extent.m_maxY);

			m_SpatialTree.setRootPage(m_nBTreeRootPage);
			m_SpatialTree.setExtent(m_Extent);
			return true;
		}

		virtual bool init(int64 nBTreeRootPage)
		{

			m_nBTreeRootPage = nBTreeRootPage;
			m_SpatialTree.setRootPage(m_nBTreeRootPage);
			return m_SpatialTree.init();
		
		}


		virtual bool insert (CommonLib::CVariant* pFieldVal, uint64 nOID)
		{
			TSpatialObject obj;
			pFieldVal->getVal(obj);
			return m_SpatialTree.insert(obj, nOID);

		}
	/*	virtual uint64 insert (IFieldVariant* pFieldVal)
		{
			TSpatialObject obj;
			pFieldVal->getVal(obj);
			uint64 nOID;
			if(!m_SpatialTree.insertLast(TOIDIncFunctor(), obj, &nOID))
				return  0;
			return nOID;
		}*/

		TSpatialBPTree* getBTree() {return &m_SpatialTree;}
		TPointType getShiftX(){return m_shiftX; }
		TPointType getShiftY(){return m_shiftY; }
	private:
		TSpatialBPTree m_SpatialTree;
		IDBTransaction* m_pDBTransactions;
		int64 m_nBTreeRootPage;
		sFieldInfo m_fi;
		TPointType m_shiftX;
		TPointType m_shiftY;
		TRect	m_Extent;
	 
	};
	template<class _TSpatialBPTree, class _TPointType, class _TSpatialObject>
	class OIDSpatialFieldHandler : public IDBFieldHandler
	{
		public:

			typedef _TSpatialBPTree TSpatialBPTree;
			typedef _TPointType 	TPointType;
			typedef CommonLib::TRect2D<TPointType>   TRect;
			typedef _TSpatialObject TSpatialObject;
			typedef OIDSpatialField<TSpatialBPTree, TPointType, TSpatialObject>  TOIDSpatialField;

			OIDSpatialFieldHandler(CommonLib::alloc_t* pAlloc) : m_pAlloc(pAlloc), m_pIndexHandler(NULL)
			{

			}
			~OIDSpatialFieldHandler()
			{

			}


			eDataTypes getType() const
			{
				return (eDataTypes)m_fi.m_nFieldDataType;
			}
			const CommonLib::CString& getName() const
			{
				return m_fi.m_sFieldName;
			}

			virtual sFieldInfo* getFieldInfoType()
			{
				return &m_fi;
			}
			virtual void setFieldInfoType(sFieldInfo* fi)
			{
				m_fi = *fi;
			}
			virtual bool save(int64 nAddr, IDBTransaction *pTran)
			{
				FilePagePtr pPage = pTran->getFilePage(nAddr);
				if(!pPage.get())
					return false;
				CommonLib::FxMemoryWriteStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());
				sFilePageHeader header(stream, FIELD_PAGE, FIELD_INFO_PAGE);
				int64 m_nBTreeRootPage = -1;
				FilePagePtr pRootPage(pTran->getNewPage());
				if(!pRootPage.get())
					return false;
				m_nBTreeRootPage = pRootPage->getAddr();

				TPointType nShiftX = TPointType(m_fi.m_Extent.m_minX < 0 ? - 1* m_fi.m_Extent.m_minX : 0);
				TPointType nShiftY = TPointType(m_fi.m_Extent.m_minY < 0 ? - 1* m_fi.m_Extent.m_minY : 0);
				TRect	nExtent;
				nExtent.set((TPointType)m_fi.m_Extent.m_minX + nShiftX, (TPointType)m_fi.m_Extent.m_minY + nShiftY, (TPointType)m_fi.m_Extent.m_maxX + nShiftX, (TPointType)m_fi.m_Extent.m_maxY + nShiftY);
				stream.write(m_nBTreeRootPage);
				stream.write(nShiftX);
				stream.write(nShiftY);
				stream.write(nExtent.m_minX);
				stream.write(nExtent.m_minY);
				stream.write(nExtent.m_maxX);
				stream.write(nExtent.m_maxY);
				header.writeCRC32(stream);
				pPage->setFlag(eFP_CHANGE, true);
				pTran->saveFilePage(pPage);

				TOIDSpatialField field(pTran, m_pAlloc, m_fi);
				field.init(m_nBTreeRootPage);
				return field.save();
			}
			virtual bool load(int64 nAddr, IDBStorage *pStorage)
			{
				return true;
			}

			virtual bool lock()
			{
				return true;
			}
			virtual bool unlock()
			{
				return true;
			}

			TOIDSpatialField* getSpatialOIDField(IDBTransaction* pTransactions, IDBStorage *pStorage)
			{

				TOIDSpatialField * pField = new  TOIDSpatialField(pTransactions, m_pAlloc, m_fi);
				pField->load(m_fi.m_nFieldPage);
				return pField;	
			}

			bool isCanBeRemoving()
			{
				return true;
			}
			IValueField* getValueField(IDBTransaction* pTransactions, IDBStorage *pStorage) 
			{
				return NULL;
			}
			bool release(IValueField* pField)
			{
				return true;
			}

			virtual void setIndexHandler(IDBIndexHandler *pIndexHandler)
			{
				m_pIndexHandler = pIndexHandler;
			}
			virtual IDBIndexHandler* getIndexIndexHandler()
			{
				return m_pIndexHandler;
			}

		private:
			sFieldInfo m_fi;
			CommonLib::alloc_t* m_pAlloc;
			IDBIndexHandler* m_pIndexHandler;
	};


	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU16, uint64,
		embDB::ZPointComp<embDB::ZOrderPoint2DU16> > TBPMapPoint16;	
	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU32, uint64,
		embDB::ZPointComp<embDB::ZOrderPoint2DU32> > TBPMapPoint32;

	typedef embDB::TBPPointSpatialMap<embDB::ZOrderPoint2DU64, 	uint64,	embDB::ZPointComp64, IDBTransaction,
		embDB::BPSpatialPointInnerNodeSimpleCompressor64,
		embDB::BPSpatialPointLeafNodeMapSimpleCompressor64<uint64> > TBPMapPoint64;


	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU16, uint64,
		embDB::ZPointComp<embDB::ZOrderRect2DU16> 	> TBPMapRect16;

	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU32, uint64,
		embDB::ZRect32Comp, IDBTransaction,
		embDB::BPSpatialRectInnerNodeSimpleCompressor< embDB::ZOrderRect2DU32>,	
		embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU32, uint64> > TBPMapRect32;

	typedef embDB::TBPRectSpatialMap<embDB::ZOrderRect2DU64, uint64,
		embDB::ZRect64Comp, IDBTransaction,
		embDB::BPSpatialRectInnerNodeSimpleCompressor<ZOrderRect2DU64 >,	
		embDB:: BPSpatialRectLeafNodeMapSimpleCompressor<embDB::ZOrderRect2DU64, uint64 > > TBPMapRect64;


	typedef OIDSpatialFieldHandler<TBPMapPoint16, uint16, CommonLib::TPoint2Du16> TPoint16Field;
	typedef OIDSpatialFieldHandler<TBPMapPoint32, uint32, CommonLib::TPoint2Du32> TPoint32Field;
	typedef OIDSpatialFieldHandler<TBPMapPoint64, uint64, CommonLib::TPoint2Du64> TPoint64Field;


	typedef OIDSpatialFieldHandler<TBPMapRect16, uint16, CommonLib::TRect2Du16> TRect16Field;
	typedef OIDSpatialFieldHandler<TBPMapRect32, uint32, CommonLib::TRect2Du32> TRect32Field;
	typedef OIDSpatialFieldHandler<TBPMapRect64, uint64, CommonLib::TRect2Du64> TRect64Field;
}


#endif