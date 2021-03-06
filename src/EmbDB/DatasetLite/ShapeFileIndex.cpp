#include "stdafx.h"
#include "ShapeFileIndex.h"
//#include "../EmbDB/DB/BTreePlus/spatial/Point/PointSpatialBPMapTree.h"
#include "CommonLibrary/SpatialKey.h"
//#include "../EmbDB/DB/BTreePlus/spatial/Point/SpatialPointQuery.h"
#include "../EmbDB/DB/transactions/Transactions.h"
#include "../EmbDB/DB/transactions/DirectTran/DirectTransactions.h"
//#include "../EmbDB/DB/BTreePlus/spatial/Rect/RectSpatialBPMapTree.h"
//#include "../EmbDB/DB/BTreePlus/spatial/Rect/SpatialRectQuery.h"
#include "../EmbDB/storage/storage.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "ShapeFileIndexRect.h"
#include "ShapeFileIndexPoint.h"







namespace DatasetLite
{



	

	IShapeFileIndexPtr IShapeFileIndex::open(const CommonLib::CString& sDbName, int32 nShapeType)
	{ 
		if(nShapeType == SHPT_POINT || nShapeType == SHPT_POINTZ || nShapeType == SHPT_POINTM)
		{
			CShapeFileIndexPoint *pShapeFileIndexPoint = new CShapeFileIndexPoint();

			if(!pShapeFileIndexPoint->Open(sDbName))
			{
				delete pShapeFileIndexPoint;
				pShapeFileIndexPoint = NULL;
			}
			return IShapeFileIndexPtr(pShapeFileIndexPoint);
		}
		else
		{
			CShapeFileIndexRect *pShapeFileIndexRect = new CShapeFileIndexRect();

			if(!pShapeFileIndexRect->Open(sDbName))
			{
				delete pShapeFileIndexRect;
				pShapeFileIndexRect = NULL;
			}
			return IShapeFileIndexPtr(pShapeFileIndexRect);
		}

		
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


	embDB::eSpatialType GetType(uint64 nMaxVal, bool isPoint)
	{
		if(nMaxVal < 0xFFFF)
				return  isPoint ? embDB::stPoint16 : embDB::stRect16;
		else if(nMaxVal < 0xFFFFFFFF)
			return  isPoint ? embDB::stPoint32 : embDB::stRect32;
	 
		return  isPoint ? embDB::stPoint64 : embDB::stRect64;
	}

	IShapeFileIndexPtr IShapeFileIndex::create(const CommonLib::CString& sDbName, uint32 nPageSize,
		const CommonLib::CString& sShapeFileName, GisEngine::GisCommon::Units units , 
		double dOffsetX, double dOffsetY, byte nScaleX, byte nScaleY, CommonLib::bbox bbox)
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

		embDB::eSpatialType DataType = embDB::stUnknown; 

		bool isPoint = false;
		if(shapeType == SHPT_POINT || shapeType == SHPT_POINTZ || shapeType == SHPT_POINTM /*|| 
			shapeType == SHPT_MULTIPOINT || shapeType == SHPT_MULTIPOINTZ || shapeType == SHPT_MULTIPOINTM*/)
		{
			isPoint = true;
		}
		double dMaxX = fabs(bbox.xMax + dOffsetX);
		double dMaxY = fabs(bbox.yMax + dOffsetY);
		double dMaxCoord = max(dMaxX, dMaxY);

		
		if(nScaleX == 0 || nScaleY == 0)
		{

			switch(units)
			{
				case GisEngine::GisCommon::UnitsDecimalDegrees:
					nScaleX = 7;// 0.0000001;
					nScaleY = 7;//0.0000001;
					break;
				case GisEngine::GisCommon::UnitsKilometers:
				case GisEngine::GisCommon::UnitsMiles:
					nScaleX = 3;
					nScaleY = 3;//0.001;
					break;
				case GisEngine::GisCommon::UnitsMeters:
				case GisEngine::GisCommon::UnitsYards:
				case GisEngine::GisCommon::UnitsFeet:
				case GisEngine::GisCommon::UnitsDecimeters:
				case GisEngine::GisCommon::UnitsInches:
					nScaleX = 2;//;
					nScaleY = 2;//0.01;
					break;
				case GisEngine::GisCommon::UnitsMillimeters:
					nScaleX = 1;
					nScaleY = 1;
					break;
				default:
					nScaleX = 4;//0.0001;
					nScaleY = 4;//0.0001;
					break;
			}
		}
		double dScale = 1.0/pow(10.0, nScaleX);
		int64 nMaxVal = int64(dMaxCoord/dScale);
		DataType = GetType(nMaxVal, isPoint);
	

		IShapeFileIndexPtr pShapeFileIndex;

		switch(DataType)
		{
			case embDB::stPoint16:
			case embDB::stPoint32:
			case embDB::stPoint64:
				{
					CShapeFileIndexPoint *pShapeFileIndexPoint =
						new CShapeFileIndexPoint(NULL, nPageSize, bbox, dOffsetX, dOffsetY, nScaleX, nScaleY, units, DataType, shapeType);
					if(pShapeFileIndexPoint->Create(sDbName))
					{
						pShapeFileIndex = pShapeFileIndexPoint;
					}
					else
						delete pShapeFileIndexPoint;
				}
			
				break;

			case embDB::stRect16:
			case embDB::stRect32:
			case embDB::stRect64:
				{
					{
						CShapeFileIndexRect *pShapeFileIndexRect =
							new CShapeFileIndexRect(NULL, nPageSize, bbox, dOffsetX, dOffsetY, nScaleX, nScaleY, units, DataType, shapeType);
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
