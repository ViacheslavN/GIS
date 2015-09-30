#include "stdafx.h"
#include "ShapeFileIndex.h"
#include "../EmbDB/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
#include "../EmbDB/SpatialPointQuery.h"
#include "../EmbDB/Transactions.h"
#include "../EmbDB/DirectTransactions.h"
#include "../EmbDB/PoinMapInnerCompressor64.h"
#include "../EmbDB/PoinMapLeafCompressor64.h"
#include "../EmbDB/RectSpatialBPMapTree.h"
#include "../EmbDB/SpatialRectQuery.h"
#include "../EmbDB/storage.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "ShapeFileIndexRect.h"
#include "ShapeFileIndexPoint.h"







namespace DatasetLite
{



	

	IShapeFileIndexPtr IShapeFileIndex::open(const CommonLib::CString& sDbName)
	{ 
		CShapeFileIndexRect *pShapeFileIndexRect = new CShapeFileIndexRect();

		if(!pShapeFileIndexRect->Open(sDbName))
		{
			delete pShapeFileIndexRect;
			pShapeFileIndexRect = NULL;
		}
		return IShapeFileIndexPtr(pShapeFileIndexRect);
	}
	struct SHPGuard
	{
		ShapeLib::SHPHandle file;
		SHPGuard()
			: file(NULL)
		{}
		~SHPGuard()
		{
			clear();
		}
		void clear()
		{
			if(file)
				ShapeLib::SHPClose(file);
			file = NULL;
		}
	};


	embDB::eDataTypes GetType(uint64 nMaxVal, bool isPoint)
	{
		if(nMaxVal < 0xFFFF)
				return  isPoint ? embDB::dtPoint16 : embDB::dtRect16;
		else if(nMaxVal < 0xFFFFFFFF)
			return  isPoint ? embDB::dtPoint32 : embDB::dtRect32;
	 
		return  isPoint ? embDB::dtPoint64 : embDB::dtRect64;
	}

	IShapeFileIndexPtr IShapeFileIndex::create(const CommonLib::CString& sDbName, size_t nPageSize,
		const CommonLib::CString& sShapeFileName, GisEngine::GisCommon::Units units , 
		double dOffsetX, double dOffsetY, double dScaleX, double dScaleY, CommonLib::bbox bbox)
	{ 
		SHPGuard shpFile;
		shpFile.file = ShapeLib::SHPOpen(sShapeFileName.cstr(), "rb");
		if(!shpFile.file)
			return IShapeFileIndexPtr();


		CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(sShapeFileName);
		CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(sShapeFileName);
		CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";


	 
		int nObjectCount = 0;
		int shapeType = 0;
		double minBounds[4];
		double maxBounds[4];
		ShapeLib::SHPGetInfo(shpFile.file, &nObjectCount, &shapeType, &minBounds[0], &maxBounds[0]);

		if(nObjectCount > 0 && bbox.type == CommonLib::bbox_type_null)
		{
			bbox.type = CommonLib::bbox_type_normal;
			bbox.xMin = minBounds[0];
			bbox.xMax = maxBounds[0];
			bbox.yMin = minBounds[1];
			bbox.yMax = maxBounds[1];
			bbox.zMin = minBounds[2];
			bbox.zMax = maxBounds[2];
			bbox.mMin = minBounds[3];
			bbox.mMax = maxBounds[3];
		
		}	
		if(units ==  GisEngine::GisCommon::UnitsUnknown)
		{
			GisEngine::GisGeometry::CSpatialReferenceProj4* pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(prjFileName, GisEngine::GisGeometry::eSPRefTypePRJFilePath);
			if(!pSpatialReference->IsValid())
			{
				pSpatialReference = new GisEngine::GisGeometry::CSpatialReferenceProj4(bbox);
			}
			if(pSpatialReference->IsValid())
			{
				units = pSpatialReference->GetUnits();
			}
			delete pSpatialReference;
		}

		if(dOffsetX == 0)
		{
			if(bbox.xMin < 0)
				dOffsetX = fabs(bbox.xMin);
			else
				dOffsetX = -1 *bbox.xMin;
		}

		if(dOffsetY == 0)
		{
			if(bbox.yMin < 0)
				dOffsetY = fabs(bbox.yMin);
			else
				dOffsetY = -1 *bbox.yMin;
		}

		embDB::eDataTypes DataType = embDB::dtUnknown; 

		bool isPoint = false;
		if(shapeType == SHPT_POINT || shapeType == SHPT_POINTZ || shapeType == SHPT_POINTM /*|| 
			shapeType == SHPT_MULTIPOINT || shapeType == SHPT_MULTIPOINTZ || shapeType == SHPT_MULTIPOINTM*/)
		{
			isPoint = true;
		}
		double dMaxX = fabs(bbox.xMax + dOffsetX);
		double dMaxY = fabs(bbox.yMax + dOffsetY);
		double dMaxCoord = max(dMaxX, dMaxY);
		if(dScaleX == 0 || dScaleY == 0)
		{

			switch(units)
			{
				case GisEngine::GisCommon::UnitsDecimalDegrees:
					dScaleX = 0.0000001;
					dScaleY = 0.0000001;
					break;
				case GisEngine::GisCommon::UnitsKilometers:
				case GisEngine::GisCommon::UnitsMiles:
					dScaleX = 0.001;
					dScaleY = 0.001;
					break;
				case GisEngine::GisCommon::UnitsMeters:
				case GisEngine::GisCommon::UnitsYards:
				case GisEngine::GisCommon::UnitsFeet:
				case GisEngine::GisCommon::UnitsDecimeters:
				case GisEngine::GisCommon::UnitsInches:
					dScaleX = 0.01;
					dScaleY = 0.01;
					break;
				case GisEngine::GisCommon::UnitsMillimeters:
					dScaleX = 1;
					dScaleY = 1;
					break;
				default:
					dScaleX = 0.0001;
					dScaleY = 0.0001;
					break;
			}
		}
		int64 nMaxVal = int64(dMaxCoord/dScaleX);
		DataType = GetType(nMaxVal, isPoint);
	

		IShapeFileIndexPtr pShapeFileIndex;

		switch(DataType)
		{
			case embDB::dtPoint16:
			case embDB::dtPoint32:
			case embDB::dtPoint64:
				{
					CShapeFileIndexPoint *pShapeFileIndexPoint =
						new CShapeFileIndexPoint(NULL, nPageSize, bbox, dOffsetX, dOffsetY, dScaleX, dScaleY, units, DataType, shapeType);
					if(pShapeFileIndexPoint->Create(sDbName))
					{
						pShapeFileIndex = pShapeFileIndexPoint;
					}
					else
						delete pShapeFileIndexPoint;
				}
			
				break;

			case embDB::dtRect16:
			case embDB::dtRect32:
			case embDB::dtRect64:
				{
					{
						CShapeFileIndexRect *pShapeFileIndexRect =
							new CShapeFileIndexRect(NULL, nPageSize, bbox, dOffsetX, dOffsetY, dScaleX, dScaleY, units, DataType, shapeType);
						if(pShapeFileIndexRect->Create(sDbName))
						{
							pShapeFileIndex = pShapeFileIndexRect;
						}
						else
							delete pShapeFileIndexRect;
					}
				}
				break;
		}

		if(pShapeFileIndex.get())
		{
			for (int i = 0; i < nObjectCount; ++i)
			{
				ShapeLib::SHPObject* pShObject = ShapeLib::SHPReadObject(shpFile.file, i);
				pShapeFileIndex->insert(pShObject, i);
			}
			pShapeFileIndex->commit();
		}
		
		return pShapeFileIndex;
	}

}
