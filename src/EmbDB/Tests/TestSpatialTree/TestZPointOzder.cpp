#include "stdafx.h"
#include "CommonLibrary/SpatialKey.h"
#include "../../EmbDB/SpatialPointQuery.h"
#include <iostream>
#include <vector>

template <class TCoord, class TPointType>
void testZPoint(TCoord nBegin, TCoord nEnd)
{
	for (TCoord x = nBegin; x < nEnd; ++x)
	{
		for (TCoord y = nBegin; y < nEnd; ++y)
		{
			TPointType point(x, y);
			TCoord X;
			TCoord Y;
			point.getXY(X, Y);
			if(x != X || y != Y)
			{
				std::cout << "Error ZOrder x: " << x << "y: "<< y << "X: "<< X <<"Y: " << Y <<std::endl;
			}
		}
	}
}
struct TQuery
{
	typedef  CommonLib::TRect2D<uint64>  TRect;
	TRect m_Rect;
	embDB::ZOrderPoint2DU64  m_zMin;
	embDB::ZOrderPoint2DU64  m_zMax;
	short m_nBits;
	void InitFromZOrder()
	{
		m_zMin.getXY(m_Rect.m_minX, m_Rect.m_minY);
		m_zMax.getXY(m_Rect.m_maxX, m_Rect.m_maxY);
	}
};

void testZPoint1()
{
		std::vector<TQuery> vecSubQueries;
		TQuery curQuery;
	{
		embDB::ZOrderPoint2DU64 zMin(0, 0);
		embDB::ZOrderPoint2DU64 zMax((uint64)0xFFFFFFFFFFFFFFFF, (uint64)0xFFFFFFFFFFFFFFFF);
		//embDB::ZOrderPoint2DU64 zMax(1, 1);
		embDB::ZOrderPoint2DU64 zPageLast(0, 0);
		
		curQuery.m_zMax = zMax;
		curQuery.m_zMin = zMin;
		curQuery.m_nBits =  zMin.getBits();;
	

		embDB::ZOrderPoint2DU64  highKey = zMax;
		while (zPageLast < highKey)
		{
			short nCurBit = curQuery.m_nBits;
			assert(nCurBit >= 0);
			while (curQuery.m_zMin.getBit (nCurBit) == curQuery.m_zMax.getBit (nCurBit) && nCurBit > 0)
			{
				nCurBit--;
				assert(nCurBit >= 0);
			}

			embDB::ZOrderPoint2DU64 lowKey = curQuery.m_zMin;
			highKey = curQuery.m_zMax;
			highKey.setLowBits(nCurBit);
			lowKey.clearLowBits (nCurBit);

			curQuery.m_nBits = --nCurBit;


			TQuery nLeftQuery;// запрос с меньшими ключами
			nLeftQuery.m_zMin = curQuery.m_zMin;
			nLeftQuery.m_zMax = highKey;
			nLeftQuery.InitFromZOrder();
			nLeftQuery.m_nBits = curQuery.m_nBits;



			TQuery nRightQuery;// запрос с большими ключами
			nRightQuery.m_zMin = lowKey;
			nRightQuery.m_zMax = curQuery.m_zMax;
			nRightQuery.InitFromZOrder();
			nRightQuery.m_nBits = curQuery.m_nBits;

			curQuery = nLeftQuery; 
			vecSubQueries.push_back(nRightQuery);
		}

	}
 
	TQuery& curBack = vecSubQueries.back();

	embDB::ZOrderPoint2DU16 zMin(1, 0);
	embDB::ZOrderPoint2DU16 zMax(3, 6);
	embDB::ZOrderPoint2DU16 zM1(3, 6);
	embDB::ZOrderPoint2DU16 zM2(3, 0);
	int curBitNum = 31;
	int32 zValMin ;
	int32 zValMax;
	while (zMin.getBit (curBitNum) == zMax.getBit (curBitNum))
	{
		zValMin = zMin.getBit (curBitNum);
		zValMax = zMax.getBit (curBitNum);
		curBitNum--;
	}
	zValMin = zMin.getBit (curBitNum);
	zValMax = zMax.getBit (curBitNum);
	embDB::ZOrderPoint2DU16 lowKey = zMin;
	embDB::ZOrderPoint2DU16 highKey = zMax;
	highKey.setLowBits(curBitNum);
	lowKey.setLowBits (curBitNum);

	uint16 nX1, nY1;
	highKey.getXY(nX1, nY1);
	uint16 nX2, nY2;
	lowKey.getXY(nX2, nY2);

	//testZPoint<uint16, embDB::ZOrderPoint2DU16>(0, 65535);
	//testZPoint<uint32, embDB::ZOrderPoint2DU32>(1000000, 1000000 + 10000);
	//testZPoint<uint32, embDB::ZOrderPoint2DU32>(0xFFFFFFFF-10000, 0xFFFFFFFF);
	//testZPoint<uint64, embDB::ZOrderPoint2DU64>(1000000, 1000000 + 1000);
	//testZPoint<uint64, embDB::ZOrderPoint2DU64>(0xFFFFFFFFFFFFFFFF - 10000 , 0xFFFFFFFFFFFFFFFF);
}