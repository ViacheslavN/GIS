#include "stdafx.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include "../../EmbDB/RectSpatialBPMapTree.h"
#include "../../EmbDB/Transactions.h"
#include "../../EmbDB/DirectTransactions.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include "../../EmbDB/BPVector.h"
#include "CommonLibrary/String.h"
#include "CommonLibrary/DebugTime.h"
#include <iostream>
#include <set>

#include <vector>

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


template<class TZVal>
struct sQuery
{
	TZVal zMin;
	TZVal zMax;
	long nBits;
};

template<class TZVal>
bool FindRectMinZVal1(const TZVal& zVal, 
	const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
{
	if(zVal < zMin || zVal > zMax)
	{
		assert(false);
		return false;
	}

	//short nBits = zRes.getBits();

	TZVal left = zMin;
	TZVal right = zMax;
	zRes = zMax;

	sQuery<TZVal> curQ;
	curQ.zMin = zMin;
	curQ.zMax = zMax;
	curQ.nBits = zRes.getBits();
	std::vector<sQuery<TZVal>> vecQ;
	while(zVal <curQ.zMax )
	{


		TZVal qMin = left;
		TZVal qMax = right;

		while (curQ.zMin.getBit (curQ.nBits) == curQ.zMax.getBit (curQ.nBits))
		{

			curQ.nBits--;
			if(curQ.nBits < 0)
			{
				//	uint16 xMin, yMin, xMax, yMax;
				//	zRes.getXY(xMin, yMin, xMax, yMax);

				int i = 0;
				i++;
				return true;

			}
			//assert(nBits >= 0);
		}



		sQuery<TZVal> nNexSubQuery;
		nNexSubQuery.zMin = curQ.zMin;
		nNexSubQuery.zMax = curQ.zMax;
		nNexSubQuery.zMin.clearLowBits (curQ.nBits);

		curQ.zMax.setLowBits(curQ.nBits);

		nNexSubQuery.nBits = --curQ.nBits;
 		vecQ.push_back(nNexSubQuery);

		if(!(zVal <curQ.zMax))
		{
			int dd = 0;
			dd++;
		}
 
	}

	return true;
}


void TestFullRectScan(embDB::TBPVector<embDB::ZOrderRect2DU16>& vecRect, CommonLib::TRect2D<short>& extent, uint16 xMax, uint16 yMax, std::set<int> *pSet = NULL);
void TestRectWithSubQuery(embDB::TBPVector<embDB::ZOrderRect2DU16>& vecRect, CommonLib::TRect2D<short>& extent, uint16 xMax, uint16 yMax, std::set<int> *pSet = NULL);

void TestRectZorder()
{

	embDB::ZOrderRect2DU32 zMin, zMax, zNext, zRes, zRes1;


	CommonLib::TRect2D<uint32> rect;
	rect.m_minX = 2551553;
	rect.m_minY	= 2734546;
	rect.m_maxX	= 2558215;
	rect.m_maxY	= 2738573;




	zMin.m_nZValue[0] = 14179799259754791044;
	zMin.m_nZValue[1] = 12616776;


	zMax.m_nZValue[0] = 17207406246237106143;
	zMax.m_nZValue[1] = 14757395258970795487;

	zNext.m_nZValue[0] = 6503758189072004739;
	zNext.m_nZValue[1] = 15770975;

	zRes1.m_nZValue[0] = 12114932510826323909;
	zRes1.m_nZValue[1] = 15770975;
	
	if(zRes1.IsInRect(rect))
	{
		int dd = 0;
		dd++;
	}

	FindRectMinZVal1(zNext, zMin, zMax, zRes);




	embDB::TBPVector<embDB::ZOrderRect2DU16> vecRect;

	int Xmax = 14;
	int Ymax = 14;

	int qXmin = 3;
	int qXmax = 11;

	int qYmin = 3;
	int qYmax = 11;

/*	int Xmax = 8;
	int Ymax = 8;

	int qXmin = 1;
	int qXmax = 5;

	int qYmin = 1;
	int qYmax = 5;*/

	int nStep = 2;

	for (short x = 0; x < Xmax ; x += nStep)
	{
		for (short y = 0; y < Ymax ; y += nStep)
		{
			embDB::ZOrderRect2DU16 zVal(x, y, x +nStep, y +nStep );
			vecRect.push_back(zVal);
		}
	}

	embDB::ZPointComp<embDB::ZOrderRect2DU16> comp;
	CommonLib::TRect2D<short>  extent(qXmin, qYmin, qXmax, qYmax);
	vecRect.quick_sort(comp);
	std::set<int> fullset, subset;
	TestFullRectScan(vecRect, extent, Xmax , Ymax, &fullset );
	TestRectWithSubQuery(vecRect, extent, Xmax , Ymax, &subset );

	for (auto it = subset.begin(); it != subset.end(); ++it)
	{
		fullset.erase(*it);
	}

	int d = 0;
	d++;
};

void TestRectZorder(int Xmax, int Ymax, int qXmin, int qYmin, int qXmax, int qYmax)
{
	embDB::TBPVector<embDB::ZOrderRect2DU16> vecRect;



	for (short x = 0; x < Xmax + 10; x += 10)
	{
		for (short y = 0; y < Ymax  + 10; y += 10)
		{
			embDB::ZOrderRect2DU16 zVal(x, y, x + 10, y +10);
			vecRect.push_back(zVal);
		}
	}

	embDB::ZPointComp<embDB::ZOrderRect2DU16> comp;
	CommonLib::TRect2D<short>  extent(qXmin, qYmin, qXmax, qYmax);
	vecRect.quick_sort(comp);

	TestFullRectScan(vecRect, extent, 0xFFFF, 0xFFFF);
	TestRectWithSubQuery(vecRect, extent, 0xFFFF, 0xFFFF);
}

void TestFullRectScan(embDB::TBPVector<embDB::ZOrderRect2DU16>& vecRect, CommonLib::TRect2D<short>& extent, uint16 xMax, uint16 yMax,  std::set<int> *pSet)
{
	embDB::ZOrderRect2DU16 zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	embDB::ZOrderRect2DU16 zKeyMax(extent.m_maxX, extent.m_maxY, xMax, yMax);

	embDB::ZPointComp<embDB::ZOrderRect2DU16> comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size(); ++i)
	{
		if(comp.LE(zKeyMax, vecRect[i]))
			break;

		uint16 xMin,  yMin,  xMax,  yMax;
		embDB::ZOrderRect2DU16& zVal = vecRect[i];
		zVal.getXY(xMin,yMin, xMax, yMax);
		CommonLib::TRect2D<short> rectFeature;
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


void TestRectWithSubQuery(embDB::TBPVector<embDB::ZOrderRect2DU16>& vecRect, CommonLib::TRect2D<short>& extent, uint16 xMax, uint16 yMax,  std::set<int> *pSet )
{
	embDB::ZOrderRect2DU16 zKeyMin(extent.m_minX, extent.m_minY, 0, 0);
	embDB::ZOrderRect2DU16 zKeyMax(extent.m_maxX, extent.m_maxY, xMax, yMax);





	embDB::ZPointComp<embDB::ZOrderRect2DU16> comp;
	short nType;
	int nIndex = vecRect.lower_bound(zKeyMin,nType, comp);

	int nInRect = 0;
	int nInOut = 0;
	int nTotal = 0;
	for (size_t i = nIndex; i < vecRect.size();)
	{

		embDB::ZOrderRect2DU16& zVal = vecRect[i];
		if(!comp.LE(zVal, zKeyMax))
			break;

		uint16 xMin1,  yMin1,  xMax1,  yMax1;
		
		zVal.getXY(xMin1,yMin1, xMax1, yMax1);
		CommonLib::TRect2D<short> rectFeature;
		rectFeature.set(xMin1, yMin1, xMax1, yMax1);
		if(!extent.isIntersection(rectFeature) && !extent.isInRect(rectFeature))
		{
			++nInOut;
			embDB::ZOrderRect2DU16 zQVal;
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