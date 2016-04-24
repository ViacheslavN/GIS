#include "stdafx.h"
#include "ShapeLib/shapefil.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "importFromShape.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include <vector>
#include "ZRectU32.h"


typedef embDB::TBPVector<embDB::ZOrderRect2DU32> TVecZOrder;


template<class TZVal>
bool FindRectMinZVal(const TZVal& zVal, 
	const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
{
	if(zVal < zMin || !(zVal < zMax))
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
			zRes = qMin;
			break;
		}

		if(zVal < qMax)
		{
			right = qMax;
			zRes = qMax;
		}
		else /*if(zVal > qMin && zVal < right)*/
		{
			zRes = qMin;
			left = qMin;
			if(!(qMin < zVal))
			{

				if(qMax < zVal)
					break;
			}
		 


		}

	}

	return true;
}

template<class ZOrder, class zOrderComp, class TUnits>
void ReadShape(embDB::TBPVector<ZOrder>& vec, const wchar_t* pszShapeFileName)
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



	double dScale = 0.0000001;
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

		TUnits xMin = TUnits((bb.xMin + dOffsetX) / dScale);
		TUnits yMin = TUnits((bb.yMin + dOffsetY) / dScale);
		TUnits xMax = TUnits((bb.xMax + dOffsetX) / dScale);
		TUnits yMax = TUnits((bb.yMax + dOffsetY) / dScale);

		ZOrder zOrder(xMin, yMin, xMax, yMax);

		TUnits xMin1 = 0;
		TUnits yMin1 = 0;
		TUnits xMax1 = 0;
		TUnits yMax1 = 0;


		zOrder.getXY(xMin1, yMin1, xMax1, yMax1);

		if(xMin != xMin1 || yMin != yMin1 || xMax != xMax1 || yMax != yMax1)
		{
			int dd = 0;
			dd++;
		}

		vec.push_back(zOrder);
	}
	zOrderComp comp;
	vec.quick_sort(comp);

}
template<class ZOrder, class zOrderComp, class TUnits>
void TestFullRectScan(embDB::TBPVector<ZOrder>& vecRect, CommonLib::TRect2D<TUnits>& extent, std::set<uint32> *pSet = NULL)
{
	ZOrder zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	ZOrder zKeyMax(extent.m_maxX, extent.m_maxY, 0xFFFFFFFF, 0xFFFFFFFF);

	//embDB::ZRect32Comp comp;
	zOrderComp   comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size(); ++i)
	{
		if(comp.LE(zKeyMax, vecRect[i]))
			break;

		TUnits xMin,  yMin,  xMax,  yMax;
		ZOrder& zVal = vecRect[i];
		zVal.getXY(xMin,yMin, xMax, yMax);
		CommonLib::TRect2D<TUnits> rectFeature;
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

template<class ZOrder, class zOrderComp, class TUnits>
void TestRectWithSubQuery(embDB::TBPVector<ZOrder>& vecRect, CommonLib::TRect2D<TUnits>& extent, std::set<uint32> *pSet = NULL)
{
	ZOrder zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	ZOrder zKeyMax(extent.m_maxX, extent.m_maxY, 0xFFFFFFFF, 0xFFFFFFFF);


	zOrderComp comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size();)
	{

		if(i == 627)
		{
			int d = 0;
			d++;
		}

		ZOrder& zVal = vecRect[i];
		if(!comp.LE(zVal, zKeyMax))
			break;

		TUnits xMin1,  yMin1,  xMax1,  yMax1;

		zVal.getXY(xMin1,yMin1, xMax1, yMax1);
		CommonLib::TRect2D<TUnits> rectFeature;
		rectFeature.set(xMin1, yMin1, xMax1, yMax1);
		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
			ZOrder zQVal;
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

template<class ZOrder, class zOrderComp, class TUnits>
void TestSpIndexFromShapeFileTmp()
{
	 embDB::TBPVector<ZOrder> vecOrder;
	///ReadShape<ZOrder, zOrderComp, TUnits>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_urban_areas_landscan.shp");

	CommonLib::TRect2D<TUnits> rect;
	/*rect.m_minX = 579557569;
	rect.m_minY	= 498287239;
	rect.m_maxX	= 1738780183;
	rect.m_maxY	= 1198956604;*/

	ReadShape<ZOrder, zOrderComp, TUnits>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_admin_0_scale_rank.shp");
	//ReadShape<ZOrder, zOrderComp, TUnits>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_admin_0_countries_lakes.shp");

	rect.m_minX = 1315813034;
	rect.m_minY	= 775924124;
	rect.m_maxX	= 2556481375;
	rect.m_maxY	= 1308887284;
 

	std::set<uint32> setFullIndex, setSubQueryIndex;

	TestFullRectScan<ZOrder, zOrderComp, TUnits>(vecOrder, rect, &setFullIndex);
	TestRectWithSubQuery<ZOrder, zOrderComp, TUnits>(vecOrder, rect, &setSubQueryIndex);

	for(std::set<uint32>::iterator it = setSubQueryIndex.begin(); it != setSubQueryIndex.end(); ++it)
	{
		setFullIndex.erase((*it));
	}

	for(std::set<uint32>::iterator it = setFullIndex.begin(); it != setFullIndex.end(); ++it)
	{
		ZOrder& zValue = vecOrder[(*it)];
	//	uint32 xMin, yMin, xMax, yMax;
	//	zValue.getXY(xMin, yMin, xMax, yMax);
		
	//	bool bRez = zValue.IsInRect(rect);
		
		int dd = 0; 
		dd++;



	}
	
	int dd = 0;
	dd++;
}

void TestSpIndexFromShapeFile()
{ 
	TestSpIndexFromShapeFileTmp<embDB::ZOrderRect2DU32, embDB::ZRect32Comp, uint32>();
	//TestSpIndexFromShapeFileTmp<sRectU32, ZRectU32Comp, uint32>();
}

