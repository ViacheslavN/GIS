#include "stdafx.h"
#include "ShapeLib/shapefil.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "importFromShape.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include <vector>


typedef embDB::TBPVector<embDB::ZOrderRect2DU32> TVecZOrder;


template<class TZVal>
bool FindRectMinZVal(const TZVal& zVal, 
	const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
{
	if(zVal < zMin || zVal > zMax)
	{
		assert(false);
		return false;
	}

	short nBits = zRes.getBits();

	TZVal left = zMin;
	TZVal right = zMax;
	zRes = zMax;
	while(nBits >= 0)
	{


		TZVal qMin = left;
		TZVal qMax = right;

		while (qMin.getBit (nBits) == qMax.getBit (nBits))
		{

			nBits--;
			if(nBits < 0)
			{
				//	uint16 xMin, yMin, xMax, yMax;
				//	zRes.getXY(xMin, yMin, xMax, yMax);

				int i = 0;
				i++;
				return true;

			}
			//assert(nBits >= 0);
		}
		qMin.clearLowBits(nBits);
		qMax.setLowBits(nBits);
		--nBits;
		if(qMin < qMax)
		{

			int d = 0;
			d++;
		}

		if(zVal < qMax)
		{
			right = qMax;
			zRes = qMax;
		}
		else /*if(zVal > qMin && zVal < right)*/
		{
			zRes = qMin;
			if(qMin > zVal)
			{

				if(qMax < zVal)
					break;
			}
			else
			{
				left = qMin;

			}


		}

	}

	return true;
}


void ReadShape(TVecZOrder& vec, const wchar_t* pszShapeFileName)
{

	SHPGuard shp;
	DBFGuard dbf;

	CommonLib::CString sFilePath = CommonLib::FileSystem::FindFilePath(pszShapeFileName);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszShapeFileName);
	CommonLib::CString shpFilePath = sFilePath + sFileName + L".shp";
	CommonLib::CString dbfFilePath = sFilePath + sFileName + L".dbf";
	CommonLib::CString prjFileName = sFilePath + sFileName + L".prj";


	shp.file = ShapeLib::SHPOpen(shpFilePath.cstr(), "rb");
	if(!shp.file)
		return;
	dbf.file = ShapeLib::DBFOpen(dbfFilePath.cstr(), "rb");
	if(!dbf.file)
		return; 




	int objectCount;
	int shapeType;
	double minBounds[4];
	double maxBounds[4];
	ShapeLib::SHPGetInfo(shp.file, &objectCount, &shapeType, &minBounds[0], &maxBounds[0]);
	CommonLib::bbox bounds;

	if(objectCount > 0)
	{
		bounds.type = CommonLib::bbox_type_normal;
		bounds.xMin = minBounds[0];
		bounds.xMax = maxBounds[0];
		bounds.yMin = minBounds[1];
		bounds.yMax = maxBounds[1];
		bounds.zMin = minBounds[2];
		bounds.zMax = maxBounds[2];
		bounds.mMin = minBounds[3];
		bounds.mMax = maxBounds[3];
	}
	double dOffsetX, dOffsetY;
	if(bounds.xMin < 0)
		dOffsetX = fabs(bounds.xMin);
	else
		dOffsetX = -1 *bounds.xMin;

	if(bounds.yMin < 0)
		dOffsetY = fabs(bounds.yMin);
	else
		dOffsetY = -1 *bounds.yMin;



	double dScale = 0.01;
	CommonLib::CGeoShape shape;
	shape.AddRef();
	ShapeLib::SHPObject*   pCacheObject = NULL;
	for(size_t row = 0; row < objectCount; ++row)
	{
		pCacheObject = ShapeLib::SHPReadObject(shp.file, row);
		SHPObjectToGeometry(pCacheObject, shape);


		//CommonLib::MemoryStream steram;
		// shape.write(&steram);
		//nShapeRowSize += steram.pos();
		if(pCacheObject)
		{
			ShapeLib::SHPDestroyObject(pCacheObject);
			pCacheObject = 0;
		}

		CommonLib::bbox bb = shape.getBB();

		uint32 xMin = uint32((bb.xMin + dOffsetX) / dScale);
		uint32 yMin = uint32((bb.yMin + dOffsetY) / dScale);
		uint32 xMax = uint32((bb.xMax + dOffsetX) / dScale);
		uint32 yMax = uint32((bb.yMax + dOffsetY) / dScale);

		embDB::ZOrderRect2DU32 zOrder(xMin, yMin, xMax, yMax);

		vec.push_back(zOrder);
	}
	embDB::ZRect32Comp comp;
	vec.quick_sort(comp);

}

void TestFullRectScan(TVecZOrder& vecRect, CommonLib::TRect2D<uint32>& extent, std::set<int> *pSet = NULL)
{
	embDB::ZOrderRect2DU32 zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	embDB::ZOrderRect2DU32 zKeyMax(extent.m_maxX, extent.m_maxY, 0xFFFFFFFF, 0xFFFFFFFF);

	embDB::ZRect32Comp comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size(); ++i)
	{
		if(comp.LE(zKeyMax, vecRect[i]))
			break;

		uint32 xMin,  yMin,  xMax,  yMax;
		embDB::ZOrderRect2DU32& zVal = vecRect[i];
		zVal.getXY(xMin,yMin, xMax, yMax);
		CommonLib::TRect2D<uint32> rectFeature;
		rectFeature.set(xMin, yMin, xMax, yMax);
		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
		}
		else
		{
			nInRect++;
			if(pSet)
				pSet->insert(i);
		}
		nTotal++;
	}

	std::cout<< "TestFullScan InRect " << nInRect << " InOut " << nInOut << " Total " << nTotal << std::endl;
}
void TestRectWithSubQuery(TVecZOrder& vecRect, CommonLib::TRect2D<uint32>& extent, std::set<int> *pSet = NULL)
{
	embDB::ZOrderRect2DU32 zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	embDB::ZOrderRect2DU32 zKeyMax(extent.m_maxX, extent.m_maxY, 0xFFFFFFFF, 0xFFFFFFFF);


	embDB::ZRect32Comp comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size();)
	{

		embDB::ZOrderRect2DU32& zVal = vecRect[i];
		if(!comp.LE(zVal, zKeyMax))
			break;

		uint32 xMin1,  yMin1,  xMax1,  yMax1;

		zVal.getXY(xMin1,yMin1, xMax1, yMax1);
		CommonLib::TRect2D<uint32> rectFeature;
		rectFeature.set(xMin1, yMin1, xMax1, yMax1);
		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
			embDB::ZOrderRect2DU32 zQVal;
			FindRectMinZVal(zVal, zKeyMin, zKeyMax, zQVal);

			size_t index  = vecRect.lower_bound(zQVal,nType,comp);
			if(index <= i)
			{

				size_t indexQ  = vecRect.lower_bound(zQVal,nType,comp);
				size_t indexV  = vecRect.lower_bound(zVal,nType,comp);

				int d = 0;
				d++;

			}
			i = index;
		}
		else
		{
			if(pSet)
				pSet->insert(i);
			nInRect++;
			if(i == 26)
			{
				int d = 0;
				d++;
			}
			++i;
		}
		nTotal++;
	}

	std::cout<< "TestWithSubQuery InRect " << nInRect << " InOut " << nInOut << " Total " << nTotal << std::endl;
}

void TestSpIndexFromShapeFile()
{
	TVecZOrder vecOrder;
	ReadShape(vecOrder, L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData\\building.shp");

	CommonLib::TRect2D<uint32> rect;
	rect.m_minX = 2551553;
	rect.m_minY	= 2734546;
	rect.m_maxX	= 2558215;
	rect.m_maxY	= 2738573;

	TestFullRectScan(vecOrder, rect);
	TestRectWithSubQuery(vecOrder, rect);
}