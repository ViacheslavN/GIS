#include "stdafx.h"
#include "ShapeLib/shapefil.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "importFromShape.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include <vector>
#include "ZRectU32.h"
#include <fstream>
#include <stack>

typedef embDB::TBPVector<embDB::ZOrderRect2DU32> TVecZOrder;



	template<class TZOrder>
	class SubQuery
	{
	public:
			TZOrder m_zMin;
			TZOrder m_zMax;
			short m_nBits;


			SubQuery() : m_nBits(0)
			{

			}
	};



template<class TZVal>
bool FindRectMinZValFile(const TZVal& zVal, 
	const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
{
	if(zVal < zMin || zVal > zMax)
	{
		assert(false);
		return false;
	}

	std::ofstream fileZOrder;
	fileZOrder.open("D:\\zOrderSplit");

	short nBits = zRes.getBits();

	TZVal left = zMin;
	TZVal right = zMax;
	zRes = zMax;


	fileZOrder << "zMin   " << zMin.m_nZValue[1] << "  " << zMin.m_nZValue[0] <<
		"zNext   " << zVal.m_nZValue[1] << "  " << zVal.m_nZValue[0] <<
		"  zMax  " << 	zMax.m_nZValue[1] << "  " << zMax.m_nZValue[0] << "\n" ;
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

		fileZOrder <<"Split Query bits: " << nBits <<" ";
		fileZOrder << "L: " << left.m_nZValue[1] << "  " << left.m_nZValue[0] << "    ";
		fileZOrder << "Qmax: "<< qMax.m_nZValue[1] << "  " << qMax.m_nZValue[0] << "    ";
		fileZOrder << "Qmin: "<<qMin.m_nZValue[1] << "  " << qMin.m_nZValue[0] << "    ";
		fileZOrder << "R: " <<right.m_nZValue[1] << "  " << right.m_nZValue[0] << "\n";
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
		else 
		{
			zRes = qMin;
			left = qMin;
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

		fileZOrder <<"New Query             ";
		fileZOrder << "L: "<< left.m_nZValue[1] << "  " << left.m_nZValue[0] << "    ";
		fileZOrder << "R: "<<right.m_nZValue[1] << "  " << right.m_nZValue[0] << "\n";

	}
	fileZOrder.close();
	return true;
}


/*

	void CreateSubQuery()
		{
			if(m_pCurNode->count() == 0)
			{
				assert(false);
				return;
			}
			TPointKey zPageLast = m_pCurLeafNode->key(m_pCurLeafNode->count() - 1);
						
			while (zPageLast < m_CurrentSpatialQuery.m_zMax)
			{

		
				
				assert(m_CurrentSpatialQuery.m_nBits >= 0);
				while (m_CurrentSpatialQuery.m_zMin.getBit (m_CurrentSpatialQuery.m_nBits) == m_CurrentSpatialQuery.m_zMax.getBit (m_CurrentSpatialQuery.m_nBits))
				{
					
					m_CurrentSpatialQuery.m_nBits--;
					assert(m_CurrentSpatialQuery.m_nBits >= 0);
				}
				
				TSubQuery nNexSubQuery;
				nNexSubQuery.m_zMin = m_CurrentSpatialQuery.m_zMin;
				nNexSubQuery.m_zMax = m_CurrentSpatialQuery.m_zMax;
				nNexSubQuery.m_zMin.clearLowBits (m_CurrentSpatialQuery.m_nBits);
		
				m_CurrentSpatialQuery.m_zMax.setLowBits(m_CurrentSpatialQuery.m_nBits);


				nNexSubQuery.m_nBits = --m_CurrentSpatialQuery.m_nBits;
				//nNexSubQuery.m_ID = ++m_ID;
				if(nNexSubQuery.m_zMin < m_CurrentSpatialQuery.m_zMax)
				{
					int dd = 0;
					dd++;
				}
	
				if(nNexSubQuery.m_zMin > nNexSubQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}
				if(m_CurrentSpatialQuery.m_zMin > m_CurrentSpatialQuery.m_zMax)
				{
					int dd =0;
					dd++;
				}

				m_Queries.push(nNexSubQuery);
			}

		}
*/


template<class TZVal,class TSubQuery, class TStackQuery>
void CreateSubQuery(const TZVal& zVal, TSubQuery& CurrentSpatialQuery, TStackQuery& m_Queries)
{

	/*TSubQuery m_CurrentSpatialQuery;
	m_CurrentSpatialQuery.m_zMin = zMin;
	m_CurrentSpatialQuery.m_zMax = zMax;
	m_CurrentSpatialQuery.m_nBits = zMax.getBits() ;*/
	while (zVal < CurrentSpatialQuery.m_zMax)
	{



		assert(CurrentSpatialQuery.m_nBits >= 0);
		while (CurrentSpatialQuery.m_zMin.getBit (CurrentSpatialQuery.m_nBits) == CurrentSpatialQuery.m_zMax.getBit (CurrentSpatialQuery.m_nBits))
		{

			CurrentSpatialQuery.m_nBits--;
			assert(CurrentSpatialQuery.m_nBits >= 0);
		}

		TSubQuery nNexSubQuery;
		nNexSubQuery.m_zMin = CurrentSpatialQuery.m_zMin;
		nNexSubQuery.m_zMax = CurrentSpatialQuery.m_zMax;
		nNexSubQuery.m_zMin.clearLowBits (CurrentSpatialQuery.m_nBits);

		CurrentSpatialQuery.m_zMax.setLowBits(CurrentSpatialQuery.m_nBits);


		nNexSubQuery.m_nBits = --CurrentSpatialQuery.m_nBits;
		//nNexSubQuery.m_ID = ++m_ID;
		if(nNexSubQuery.m_zMin < CurrentSpatialQuery.m_zMax)
		{
			int dd = 0;
			dd++;
		}

		if(nNexSubQuery.m_zMin > nNexSubQuery.m_zMax)
		{
			int dd =0;
			dd++;
		}
		if(CurrentSpatialQuery.m_zMin > CurrentSpatialQuery.m_zMax)
		{
			int dd =0;
			dd++;
		}

		m_Queries.push(nNexSubQuery);
	}

	/*TSubQuery sQuery = m_Queries.top();

	zRes = sQuery.m_zMin;
	return true;*/
}




template<class TZVal>
bool FindRectMinZVal1(const TZVal& zVal, 
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

	TZVal m_zMin;
	TZVal m_zMax;

	while (zVal < right)
	{



		assert(nBits >= 0);
		while (left.getBit (nBits) == right.getBit (nBits))
		{

			nBits--;
			assert(nBits >= 0);
		}

 
		m_zMin = left;
		m_zMax = right;
		m_zMin.clearLowBits (nBits);

		right.setLowBits(nBits);


		--nBits;
		
	}
	zRes = m_zMin;
	return true;
}


template<class TPointKey>
bool FindRectMinZVal(const TPointKey& zVal, 
	const TPointKey& zMin, const TPointKey& zMax, TPointKey& zRes)
{
	if(zVal < zMin || zVal > zMax)
		return false;

	short nBits = zRes.getBits();

	TPointKey left = zMin;
	TPointKey right = zMax;
	zRes = zMax;
	while(nBits >= 0)
	{


		TPointKey qMin = left;
		TPointKey qMax = right;

		while (qMin.getBit (nBits) == qMax.getBit (nBits))
		{

			nBits--;
			assert(nBits >= 0);

		}
		qMin.clearLowBits(nBits);
		qMax.setLowBits(nBits);

		if(zVal < qMax)
		{
			right = qMax;
			zRes = qMax;
		}
		else 
		{
			if(qMin > zVal)
			{
				zRes = qMin;
				break;
			}
			left = qMin;
			zRes = qMin;

		}
		nBits--;
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

		if(xMin == 0)
		{
			int dd = 0;
			dd++;
		}

		if(xMin != xMin1 || yMin != yMin1 || xMax != xMax1 || yMax != yMax1)
		{
			int dd = 0;
			dd++;
			if(xMin != xMin1)
				dd++;
			if(yMin != yMin1)
				dd++;
			if(xMax != xMax1)
				dd++;
			if(yMax != yMax1)
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
	ZOrder zKeyMax(extent.m_maxX, extent.m_maxY,ZOrder::coordMax, ZOrder::coordMax);


	typedef SubQuery<ZOrder>  TSubQuery;
	typedef std::stack<TSubQuery> TStackQuery;



	zOrderComp comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
 

	TStackQuery m_Query;
	for (size_t i = nIndex; i < vecRect.size();)
	{

		if(i == 1433)
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

		if(xMin1 == 0)
		{
			int dd = 0;
			dd++;
		}
		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
			ZOrder zQVal;
			ZOrder zQVal1;
			ZOrder zQVal2;
			
		//	FindRectMinZValQuery<ZOrder, TSubQuery, TStackQuery>(zVal, zKeyMin, zKeyMax, zQVal2, m_Query);
			//FindRectMinZVal1(zVal, zKeyMin, zKeyMax, zQVal1);
			FindRectMinZVal(zVal, zKeyMin, zKeyMax, zQVal);
	 
			size_t index  = vecRect.lower_bound(zQVal,nType,comp);
			if(index < vecRect.size() && index > 0)
			{
				for (int32 ii = index - 1; ii >=0; --ii)
				{
					if(!(vecRect[ii] < vecRect[index]))
					{
						int dd = 0;
						dd++;
					}
				}
			}
			//size_t index1 = vecRect.lower_bound(zQVal1,nType,comp);
			size_t index2 = vecRect.lower_bound(zQVal2,nType,comp);
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
void TestRectWithSplitSubQuery(embDB::TBPVector<ZOrder>& vecRect, CommonLib::TRect2D<TUnits>& extent, std::set<uint32> *pSet = NULL)
{
	ZOrder zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	ZOrder zKeyMax(extent.m_maxX, extent.m_maxY,ZOrder::coordMax, ZOrder::coordMax);


	typedef SubQuery<ZOrder>  TSubQuery;
	typedef std::stack<TSubQuery> TStackQuery;

	TSubQuery m_CurrentSpatialQuery;
	m_CurrentSpatialQuery.m_zMin = zKeyMin;
	m_CurrentSpatialQuery.m_zMax = zKeyMax;
	m_CurrentSpatialQuery.m_nBits = zKeyMax.getBits(); 



	zOrderComp comp;
	short nType;
	int nIndex = vecRect.lower_bound(m_CurrentSpatialQuery.m_zMin, nType, comp);

	if(!(nIndex < vecRect.size()))
		return;


	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;


	ZOrder& zVal = vecRect[nIndex];
	if(!comp.LE(zVal, zKeyMax))
		return;

	TStackQuery m_Queries;
	CreateSubQuery<ZOrder, TSubQuery, TStackQuery>(zVal, m_CurrentSpatialQuery, m_Queries);


	
	for (size_t i = nIndex; i < vecRect.size();)
	{

		ZOrder& zVal = vecRect[i];
		if(!comp.LE(zVal, zKeyMax))
			break;

		if(m_Queries.empty())
		{
			break;
		}

		TUnits xMin1,  yMin1,  xMax1,  yMax1;

		zVal.getXY(xMin1,yMin1, xMax1, yMax1);
		CommonLib::TRect2D<TUnits> rectFeature;
		rectFeature.set(xMin1, yMin1, xMax1, yMax1);

		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
		}
		else
		{

			if(pSet)
				pSet->insert(i);
			nInRect++;
		}

		if(m_CurrentSpatialQuery.m_zMax < zVal)
		{
 

			while(true)
			{
				m_CurrentSpatialQuery = m_Queries.top();
				m_Queries.pop();

				nIndex = vecRect.lower_bound(m_CurrentSpatialQuery.m_zMin, nType, comp);
				if(!(nIndex < vecRect.size()))
					break;

				zVal = vecRect[nIndex];
				CreateSubQuery<ZOrder, TSubQuery, TStackQuery>(zVal, m_CurrentSpatialQuery, m_Queries);

				if(i < nIndex)
				{
					break;
				}
			}
			

			if(!(nIndex < vecRect.size()))
				break;

			
			i = nIndex;
		}
		else
			i++;
		nTotal++;
	}

	std::cout<< "TestWithSplitSubQuery InRect " << nInRect << " InOut " << nInOut << " Total " << nTotal << std::endl;
}




template<class TValue, class TZOrder, class TVector, class TComp>
void FillVector(TValue xMin, TValue yMin, TValue xMax, TValue yMax, TValue nWx, TValue nWy, int32 xOff, int32 yOff, TVector& vec)
{

	for (TValue x = xMin; x < xMax;  x += nWx)
	{
		for (TValue y = yMin; y < yMax;  y += nWy)
		{

			vec.push_back(TZOrder(x, y, x + nWx, y + nWy));

			y += yOff;
			
		}

		x += xOff;
	}

	TComp zComp;
	vec.quick_sort(zComp);

}


template<class ZOrder, class zOrderComp, class TUnits>
void TestSpIndexFromShapeFileTmp()
{
 
	 embDB::TBPVector<ZOrder> vecOrder;
	/*ReadShape<ZOrder, zOrderComp, TUnits>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_urban_areas_landscan.shp");

	CommonLib::TRect2D<TUnits> rect;
	rect.m_minX = 579557569;
	rect.m_minY	= 498287239;
	rect.m_maxX	= 1738780183;
	rect.m_maxY	= 1198956604;*/

/*
	CommonLib::TRect2D<TUnits> rect;
	ReadShape<ZOrder, zOrderComp, TUnits>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_admin_0_scale_rank.shp");

	rect.m_minX = 1315813034;
	rect.m_minY	= 775924124;
	rect.m_maxX	= 2556481375;
	rect.m_maxY	= 1308887284;*/

	CommonLib::TRect2D<TUnits> rect;
	FillVector<TUnits, ZOrder,  embDB::TBPVector<ZOrder>, zOrderComp>(1000, 1253, 10000, 10000, 20, 50, -1, -1, vecOrder);

	rect.m_minX = 1451;
	rect.m_minY	= 2345;
	rect.m_maxX	= 1985;
	rect.m_maxY	= 4137;
 

	std::set<uint32> setFullIndex, setSubQueryIndex, setSplitSubQuery;

	TestFullRectScan<ZOrder, zOrderComp, TUnits>(vecOrder, rect, &setFullIndex);
	TestRectWithSubQuery<ZOrder, zOrderComp, TUnits>(vecOrder, rect, &setSubQueryIndex);
	TestRectWithSplitSubQuery<ZOrder, zOrderComp, TUnits>(vecOrder, rect, &setSplitSubQuery);

	for(std::set<uint32>::iterator it = setSubQueryIndex.begin(); it != setSubQueryIndex.end(); ++it)
	{
		setFullIndex.erase((*it));
	}

	for(std::set<uint32>::iterator it = setFullIndex.begin(); it != setFullIndex.end(); ++it)
	{
		ZOrder& zValue = vecOrder[(*it)];
		TUnits xMin, yMin, xMax, yMax;
		zValue.getXY(xMin, yMin, xMax, yMax);
		
		bool bRez = zValue.IsInRect(rect);
		
		int dd = 0; 
		dd++;



	}
	
	int dd = 0;
	dd++;
}

void TestSpIndexFromShapeFile()
{ 
	//TestSpIndexFromShapeFileTmp<embDB::ZOrderRect2DU32, embDB::ZRect32Comp, uint32>();
	//TestSpIndexFromShapeFileTmp<embDB::ZOrderRect2DU16, embDB::ZRect16Comp, uint16>();

	TestSpIndexFromShapeFileTmp<embDB::ZOrderRect2DU32, embDB::ZRect32Comp, uint32>();
	TestSpIndexFromShapeFileTmp<sRectU32, ZRectU32Comp, uint32>();
	TestSpIndexFromShapeFileTmp<embDB::ZOrderRect2DU64, embDB::ZRect64Comp, uint64>();
	/*
	embDB::TBPVector<sRectU32> vecOrder1;
	ReadShape<sRectU32, ZRectU32Comp, uint32>(vecOrder1, L"D:\\db\\10m_cultural\\ne_10m_admin_0_scale_rank.shp");


	embDB::TBPVector<embDB::ZOrderRect2DU32> vecOrder;
	ReadShape<embDB::ZOrderRect2DU32, embDB::ZRect32Comp, uint32>(vecOrder, L"D:\\db\\10m_cultural\\ne_10m_admin_0_scale_rank.shp");

	int nDiff = 0;

	sRectU32 test1(0, 1, 5, 4);
	//embDB::ZOrderRect2DU16 test(4, 1, 5, 0);
	embDB::ZOrderRect2DU16 test(0, 1, 5, 4);
	sRectU32 zValZ1last;
	embDB::ZOrderRect2DU32 zValLast;
	for (uint32 i = 0; i < vecOrder.size(); ++i)
	{
		sRectU32 zVal1 = vecOrder1[i];
		embDB::ZOrderRect2DU32 zVal = vecOrder[i];

	

		


		if(i != 0)
		{
			zValZ1last = zVal1;
			zValLast = zVal;
		}
		else
		{
			if(!(zValZ1last < zVal1))
			{
				int dd = 0;
				dd++;
			}

			if(!(zValLast < zVal))
			{
				int dd = 0;
				dd++;
			}
		}

		uint32 nMinX = 0, nMinY = 0, nMaxX = 0, nMaxY = 0;
		uint32 nMinX1 = 0, nMinY1 = 0, nMaxX1 = 0, nMaxY1 = 0;

		zVal.getXY(nMinX, nMinY, nMaxX, nMaxY);
		zVal1.getXY(nMinX1, nMinY1, nMaxX1, nMaxY1);
		int dd = 0;
		if(nMinX != nMinX1)
			dd++;
		if(nMinY != nMinY1)
			dd++;
		if(nMaxX != nMaxX1)
			dd++;
		if(nMaxY != nMaxY1)
			dd++;

		if(dd)
		{

			nDiff++;




		}


	}

	int dd = 0;
	dd++;
	*/

}

