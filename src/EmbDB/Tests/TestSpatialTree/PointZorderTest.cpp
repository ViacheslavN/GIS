#include "stdafx.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include "../../EmbDB/RectSpatialBPMapTree.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "../../EmbDB/BPVector.h"
#include "CommonLibrary/str_t.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>


template<class TZVal>
bool FindMinZVal(const TZVal& zVal, 
		const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
{
	if(zVal < zMin || zVal > zMax)
		return false;

	short nBits = zRes.getBits();

	TZVal left = zMin;
	TZVal right = zMax;
	zRes = zMax;
	while(nBits >= 0)
	{


		embDB::ZOrderPoint2DU16 qMin = left;
		embDB::ZOrderPoint2DU16 qMax = right;

		while (qMin.getBit (nBits) == qMax.getBit (nBits))
		{

			nBits--;
 
			 assert(nBits >= 0);
		}
 
		qMin.clearLowBits(nBits);
		qMax.setLowBits(nBits);

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

void TestFullScan(embDB::TBPVector<embDB::ZOrderPoint2DU16>& vecPoint, embDB::TRect2D<short>& extent);
void TestWithSubQuery(embDB::TBPVector<embDB::ZOrderPoint2DU16>& vecPoint, embDB::TRect2D<short>& extent);
void TestPointZorder()
{
	 
	embDB::ZOrderPoint2DU16 nLow(0, 0);
	embDB::ZOrderPoint2DU16 nHigh(3, 3);
	
	embDB::ZOrderPoint2DU16 nVal(2, 3);

	nLow.m_nZValue = 24;
	nHigh.m_nZValue = 192;
	nVal.m_nZValue = 32;
	embDB::ZOrderPoint2DU16 nRes;
	FindMinZVal(nVal, nLow, nHigh, nRes);

	embDB::TBPVector<embDB::ZOrderPoint2DU16> vecPoint;

	int Xmax = 1000;
	int Ymax =1000;

	int qXmin = 200;
	int qXmax = 800;

	int qYmin = 400;
	int qYmax = 800;

	for (short x = 0; x < Xmax + 1; ++x)
	{
		for (short y = 0; y < Ymax  + 1; ++y)
		{
			embDB::ZOrderPoint2DU16 zVal(x, y);
			vecPoint.push_back(zVal);
		}
	}

	embDB::ZPointComp<embDB::ZOrderPoint2DU16> comp;
	embDB::TRect2D<short>  extent(qXmin, qYmin, qXmax, qYmax);
	vecPoint.quick_sort(comp);
 
	std::cout<< "Search" << std::endl;
	TestFullScan(vecPoint, extent);
	TestWithSubQuery(vecPoint, extent);
};

void TestFullScan(embDB::TBPVector<embDB::ZOrderPoint2DU16>& vecPoint, embDB::TRect2D<short>& extent)
{
	embDB::ZOrderPoint2DU16 zMin(extent.m_minX, extent.m_minY);
	embDB::ZOrderPoint2DU16 zMax(extent.m_maxX, extent.m_maxY);
	embDB::ZPointComp<embDB::ZOrderPoint2DU16> comp;
	short nType;
	int nIndex = vecPoint.lower_bound(zMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecPoint.size(); ++i)
	{
		if(comp.LE(zMax, vecPoint[i]))
			break;

		unsigned short x, y;
		embDB::ZOrderPoint2DU16 zVal = vecPoint[i];
		vecPoint[i].getXY(x, y);

		if(extent.isPoinInRect(x, y))
			nInRect++;
		else
			nInOut++;
	
		nTotal++;
	}

	std::cout<< "TestFullScan InRect " << nInRect << " InOut " << nInOut << " Total " << nTotal << std::endl;
}

void TestWithSubQuery(embDB::TBPVector<embDB::ZOrderPoint2DU16>& vecPoint, embDB::TRect2D<short>& extent)
{
	embDB::TBPVector<embDB::ZOrderPoint2DU16> vecQueryPoint;

	for (short x = extent.m_minX; x < extent.m_maxX + 1; ++x)
	{
		for (short y = extent.m_minY; y < extent.m_maxY + 1; ++y)
		{
			embDB::ZOrderPoint2DU16 zVal(x, y);
			vecQueryPoint.push_back(zVal);
		}
	}



	embDB::ZPointComp<embDB::ZOrderPoint2DU16> comp;
	vecQueryPoint.quick_sort(comp);


	embDB::ZOrderPoint2DU16 zMin(extent.m_minX, extent.m_minY);
	embDB::ZOrderPoint2DU16 zMax(extent.m_maxX, extent.m_maxY);
	embDB::ZOrderPoint2DU16 zLast;

	embDB::ZOrderPoint2DU16 zOutMin = zMin;
	short nType = 0;
	int nIndex = vecPoint.lower_bound(zMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecPoint.size();)
	{
		if(comp.LE(zMax, vecPoint[i]))
			break;

		unsigned short x, y;
		embDB::ZOrderPoint2DU16 zVal = vecPoint[i];
		vecPoint[i].getXY(x, y);

		if(extent.isPoinInRect(x, y))
		{
			nInRect++;
			++i;
			zLast = zVal;
		}
		else
		{
			nInOut++;
			int nQIndex =  vecQueryPoint.lower_bound(zVal,nType, comp);
			embDB::ZOrderPoint2DU16 zQVal = nQIndex == vecQueryPoint.size() ? vecQueryPoint.back() : vecQueryPoint[nQIndex];
	 

			embDB::ZOrderPoint2DU16 zQValFind;
			FindMinZVal(zVal, zMin, zMax, zQValFind);
			if(zQVal.m_nZValue != zQValFind.m_nZValue)
			{
				unsigned short x1, y1;
				zOutMin.getXY(x1, y1);

				int d = 0;
				d++;

				
			}

			unsigned short x1, y1;
			zQValFind.getXY(x1, y1);

			zOutMin = embDB::ZOrderPoint2DU16(x1, extent.m_minY);

			
		 

			int index = vecPoint.lower_bound(/*zQVal*/zQValFind,nType,comp);

			if(index <= i)
			{
				int d = 0;
				d++;
			}
			i = index;
		}

	 
		nTotal++;
	}

	std::cout<< "TestWithSubQuery InRect " << nInRect << " InOut " << nInOut << " Total " << nTotal << std::endl;
}