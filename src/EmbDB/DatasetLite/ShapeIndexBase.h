#ifndef _DATASET_LITE_SHAPE_FILE_INDEX_BASE_H_
#define _DATASET_LITE_SHAPE_FILE_INDEX_BASE_H_
#include "SpatialDataset.h"
#include "ShapeCursor.h"
#include "SpatialTree.h"
#include "../EmbDB/storage.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
namespace DatasetLite
{

	class IStatialTree;
	template<class I>
	class TShapeIndexBase : public I 
	{
		public:
			TShapeIndexBase(CommonLib::alloc_t* pAlloc, uint32 nPageSize, const CommonLib::bbox& bbox, double dOffsetX, double dOffsetY, double dScaleX, 
				double dScaleY, GisEngine::GisCommon::Units units, embDB::eSpatialType type, int nShapeType ) : 
					m_pAlloc(pAlloc),m_nPageSize(nPageSize), m_bbox(bbox), m_dOffsetX(dOffsetX),  m_dOffsetY(dOffsetY),  m_dScaleX(dScaleX),  m_dScaleY(dScaleY), m_Units(units),
					m_Type(type), m_ShapeType(nShapeType), m_nRootTreePage(1)
			{
				if(!m_pAlloc)
					m_pAlloc = &m_alloc;

			}

			TShapeIndexBase(CommonLib::alloc_t* pAlloc) : 
				m_pAlloc(pAlloc),m_nPageSize(8192), m_dOffsetX(0),  m_dOffsetY(0),  m_dScaleX(0),  m_dScaleY(0), m_Units(GisEngine::GisCommon::UnitsUnknown),
					m_Type(embDB::stUnknown), m_ShapeType(0), m_nRootTreePage(1)
			{
					if(!m_pAlloc)
						m_pAlloc = &m_alloc;
			}
			~TShapeIndexBase()
			{
				if(m_pStorage.get() && m_pStorage->isValid())
					m_pStorage->close();
			}
			
			virtual IShapeCursorPtr spatialQuery(const CommonLib::bbox& extent)
			{
				if(!m_SpTree.get())
					return IShapeCursorPtr();

				 CommonLib::bbox bb = extent;
				 if(bb.xMin < m_bbox.xMin)
					 bb.xMin = m_bbox.xMin;
				 if(bb.yMin < m_bbox.yMin)
					 bb.yMin = m_bbox.yMin;

				 if(bb.xMax > m_bbox.xMax)
					 bb.xMax = m_bbox.xMax;
				 if(bb.yMax > m_bbox.yMax)
					 bb.yMax = m_bbox.yMax;

				return m_SpTree->search(bb);
			}
			virtual const CommonLib::bbox& extent() const
			{
				return m_bbox;
			}
			virtual bool insert(ShapeLib::SHPObject* pObject, int nRow = -1)
			{
				if(!m_SpTree.get())
					return false;
				return m_SpTree->insert(pObject, nRow);
			}
		protected:
			bool Create(const CommonLib::CString& sDbName )
			{
		
				m_pStorage.reset(new embDB::CStorage(m_pAlloc));

				if(!m_pStorage->open(sDbName.cwstr(), false, false,  true, false))
				{
					return false;
				}

				embDB::FilePagePtr pPage = m_pStorage->getNewPage(8192);
				embDB::FilePagePtr pTreePage = m_pStorage->getNewPage(8192, true);
				m_pStorage->saveFilePage(pTreePage.get());
				CommonLib::FxMemoryWriteStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());	
				stream.write(m_bbox.xMin);
				stream.write(m_bbox.yMin);
				stream.write(m_bbox.xMax);
				stream.write(m_bbox.yMax);
				stream.write(m_ShapeType);
				stream.write(uint32(m_Units));
				stream.write(m_dOffsetX);
				stream.write(m_dOffsetY);
				stream.write(m_dScaleX);
				stream.write(m_dScaleY);
				stream.write((uint32)m_Type);
				stream.write(m_nPageSize);
				stream.write(m_nRootTreePage); //Root SP Tree Page
		
				m_pStorage->saveFilePage(pPage.get());
				return true;
			
			}
			bool Open(const CommonLib::CString& sDbName)
			{

				m_pStorage.reset(new embDB::CStorage(m_pAlloc));

				if(!m_pStorage->open(sDbName.cwstr(), false, false,  false, false))
				{
					return false;
				}
				m_pStorage->setFileSize(-1);
				embDB::FilePagePtr pPage = m_pStorage->getFilePage(0, 8192);
				CommonLib::FxMemoryReadStream stream;
				stream.attach(pPage->getRowData(), pPage->getPageSize());	
				stream.read(m_bbox.xMin);
				stream.read(m_bbox.yMin);
				stream.read(m_bbox.xMax);
				stream.read(m_bbox.yMax);
				stream.read(m_ShapeType);
				m_Units = (GisEngine::GisCommon::Units)stream.readIntu32();
				stream.read(m_dOffsetX);
				stream.read(m_dOffsetY);
				stream.read(m_dScaleX);
				stream.read(m_dScaleY);
				m_Type = (embDB::eSpatialType)stream.readIntu32();
				stream.read(m_nPageSize);
				stream.read(m_nRootTreePage); 

				return true;
			}
			
		protected:
			double m_dOffsetX;
			double m_dOffsetY;
			double m_dScaleX;
			double m_dScaleY;
			embDB::eSpatialType m_Type;
			int m_ShapeType;
			uint32 m_nPageSize;
			CommonLib::bbox m_bbox;
			GisEngine::GisCommon::Units m_Units;
			CommonLib::simple_alloc_t m_alloc;
			CommonLib::alloc_t* m_pAlloc;
			std::auto_ptr<IStatialTree> m_SpTree;
			std::auto_ptr<embDB::CStorage> m_pStorage;
			int64 m_nRootTreePage;
			int m_ObjectCount;
	};

}

#endif