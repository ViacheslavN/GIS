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
			TShapeIndexBase(CommonLib::alloc_t* pAlloc) : 
			  m_dOffsetX(0),  m_dOffsetY(0),  m_dScaleX(1.),  m_dScaleY(1.), m_Units(GisEngine::GisCommon::UnitsUnknown),
				  m_Type(embDB::dtUnknown), m_ShapeType(CommonLib::shape_type_null), m_pAlloc(pAlloc), m_nRootTreePage(1),
				  m_ObjectCount(0), m_nPageSize(8192)
			{
				if(!m_pAlloc)
					m_pAlloc = &m_alloc;

			}
			~TShapeIndexBase(){}
			
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
			virtual bool insert(ShapeLib::SHPObject* pObject)
			{
				if(!m_SpTree.get())
					return false;
				return m_SpTree->insert(pObject);
			}
		protected:
			bool Create(const CommonLib::CString& sDbName, size_t nPageSize, const CommonLib::CString & sShapeFileName, ShapeLib::SHPHandle shFile )
			{
			
				m_nPageSize = nPageSize;
			 
				m_pStorage.reset(new embDB::CStorage(m_pAlloc));

				if(!m_pStorage->open(sDbName.cwstr(), false, false,  true, false, m_nPageSize))
				{
					return false;
				}

				CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(sShapeFileName);
				CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(sShapeFileName);
				CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";

			
		 		double minBounds[4];
				double maxBounds[4];
				SHPGetInfo(shFile, &m_ObjectCount, &m_ShapeType, &minBounds[0], &maxBounds[0]);
			 
				if(m_ObjectCount > 0)
				{
					m_bbox.type = CommonLib::bbox_type_normal;
					m_bbox.xMin = minBounds[0];
					m_bbox.xMax = maxBounds[0];
					m_bbox.yMin = minBounds[1];
					m_bbox.yMax = maxBounds[1];
					m_bbox.zMin = minBounds[2];
					m_bbox.zMax = maxBounds[2];
					m_bbox.mMin = minBounds[3];
					m_bbox.mMax = maxBounds[3];
				}

				GisEngine::GisGeometry::CSpatialReferenceProj4* pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(prjFileName, GisEngine::GisGeometry::eSPRefTypePRJFilePath);
				if(!pSpatialReference->IsValid())
				{
					pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(m_bbox);
				}
				if(pSpatialReference->IsValid())
				{
					m_Units = pSpatialReference->GetUnits();
				}

				delete pSpatialReference;
				m_Type =  embDB::dtShape64;

			 

				if(m_bbox.xMin < 0)
					m_dOffsetX = fabs(m_bbox.xMin);
				if(m_bbox.yMin < 0)
					m_dOffsetY = fabs(m_bbox.yMin);

				if(m_Units == GisEngine::GisCommon::UnitsDecimalDegrees)
				{
					m_dScaleX = 0.0000001;
					m_dScaleY = 0.0000001;
					m_Type = embDB::dtShape32;
				}
				else if(m_Units == GisEngine::GisCommon::UnitsMeters)
				{
					/*uint32 nInt32Max = 0xFFFFFFFF;
					int64 ndistX = fabs(bounds.xMax - bounds.xMin);
					int64 ndistY = fabs(bounds.yMax - bounds.yMin);
					*/
					m_dScaleX = 0.001;
					m_dScaleY = 0.001;
				}

				if(m_ShapeType == SHPT_POINT || m_ShapeType == SHPT_POINTZ || m_ShapeType == SHPT_POINTM || 
					m_ShapeType == SHPT_MULTIPOINT || m_ShapeType == SHPT_MULTIPOINTZ || m_ShapeType == SHPT_MULTIPOINTM)
				{
					if(m_Type == embDB::dtShape32)
						m_Type = embDB::dtPoint32;
					else
						m_Type = embDB::dtPoint64;
				}
				else
				{
					if(m_Type == embDB::dtShape32)
						m_Type = embDB::dtRect32;
					else
						m_Type = embDB::dtRect64;
				}
		 


				embDB::FilePagePtr pPage = m_pStorage->getNewPage();
				embDB::FilePagePtr pTreePage = m_pStorage->getNewPage(true);
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

				if(!m_pStorage->open(sDbName.cwstr(), false, false,  false, false, m_nPageSize))
				{
					return false;
				}
				m_pStorage->setFileSize(-1);
				embDB::FilePagePtr pPage = m_pStorage->getFilePage(0);
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
				m_Type = (embDB::eDataTypes)stream.readIntu32();
				stream.read(m_nPageSize);
				stream.read(m_nRootTreePage); 

				return true;
			}

		protected:
			double m_dOffsetX;
			double m_dOffsetY;
			double m_dScaleX;
			double m_dScaleY;
			embDB::eDataTypes m_Type;
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