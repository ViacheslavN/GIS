#include "stdafx.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include <iostream>



template <class TCoord, class TPointType>
void testZRect(TCoord nBegin, TCoord nEnd, TCoord nLen)
{
	for (TCoord x = nBegin; x < nEnd; ++x)
	{
		for (TCoord y = nBegin; y < nEnd; ++y)
		{
			TPointType point(x, y, x + nLen, y + nLen );
			TCoord xMin;
			TCoord yMin;
			TCoord xMax;
			TCoord yMax;
			point.getXY(xMin, yMin, xMax, yMax);
			if(x != xMin || y != yMin || (x + nLen) != xMax|| ( y + nLen) != yMax  )
			{
				std::cout << "Error ZOrder x: " << x << "y: "<< y << "X: "<< xMin <<"Y: " << yMin <<std::endl;
			}
		}
	}
}
template <class TPointType, class TCoord>
struct TQuery
{
	typedef  CommonLib::TRect2D<TPointType>  TRect;

	TRect m_minRect;
	TRect m_maxRect;
	TCoord  m_zMin;
	TCoord  m_zMax;
	short m_nBits;
	void InitFromZOrder()
	{
		m_zMin.getXY(m_minRect.m_minX, m_minRect.m_minY, m_minRect.m_maxX, m_minRect.m_maxY);
		m_zMax.getXY(m_maxRect.m_minX, m_maxRect.m_minY, m_maxRect.m_maxX, m_maxRect.m_maxY);
	}
};

template <class TPointType, class TCoord>
void testSplitZRect(TPointType nMax)
{
	std::vector<TQuery<TPointType, TCoord> > vecSubQueries;
	TQuery<TPointType, TCoord> curQuery;
	{
		TCoord zMin(0, 0, 0, 0);
		TCoord zMax(0, 0, nMax, nMax);
		//embDB::ZOrderPoint2DU64 zMax(1, 1);
		TCoord zPageLast(0, 0, 0, 0);

		curQuery.m_zMax = zMax;
		curQuery.m_zMin = zMin;
		curQuery.m_nBits =  zMin.getBits();
		curQuery.InitFromZOrder();

		TCoord highKey = zMax;
		while (zPageLast < highKey)
		{
			short nCurBit = curQuery.m_nBits;
			assert(nCurBit >= 0);
			while (curQuery.m_zMin.getBit (nCurBit) == curQuery.m_zMax.getBit (nCurBit) && nCurBit > 0)
			{
				nCurBit--;
				assert(nCurBit >= 0);
			}

			TCoord lowKey = curQuery.m_zMin;
			highKey = curQuery.m_zMax;
			highKey.setLowBits(nCurBit);
			lowKey.clearLowBits (nCurBit);

			curQuery.m_nBits = --nCurBit;


			TQuery<TPointType, TCoord>  nLeftQuery;// запрос с меньшими ключами
			nLeftQuery.m_zMin = curQuery.m_zMin;
			nLeftQuery.m_zMax = highKey;
			nLeftQuery.InitFromZOrder();
			nLeftQuery.m_nBits = curQuery.m_nBits;



			TQuery<TPointType, TCoord>  nRightQuery;// запрос с большими ключами
			nRightQuery.m_zMin = lowKey;
			nRightQuery.m_zMax = curQuery.m_zMax;
			nRightQuery.InitFromZOrder();
			nRightQuery.m_nBits = curQuery.m_nBits;

			curQuery = nLeftQuery; 
			vecSubQueries.push_back(nRightQuery);
		}

	}

	TQuery<TPointType, TCoord>& curBack = vecSubQueries.back();
	
}


template <class TPointType, class TCoord>
void testSplitZRect1(TCoord& zMin, TCoord& zMax)
{

	TQuery<TPointType, TCoord> curQuery;

		//embDB::ZOrderPoint2DU64 zMax(1, 1);
		TCoord zPageLast(0, 0, 0, 0);

		curQuery.m_zMax = zMax;
		curQuery.m_zMin = zMin;
		curQuery.m_nBits =  zMin.getBits();
		curQuery.InitFromZOrder();

		TCoord highKey = zMax;

			short nCurBit = curQuery.m_nBits;
			assert(nCurBit >= 0);
			while (curQuery.m_zMin.getBit (nCurBit) == curQuery.m_zMax.getBit (nCurBit) && nCurBit > 0)
			{
				nCurBit--;
				assert(nCurBit >= 0);
			}

			TCoord lowKey = curQuery.m_zMin;
			highKey = curQuery.m_zMax;
			highKey.setLowBits(nCurBit);
			lowKey.clearLowBits (nCurBit);

			curQuery.m_nBits = --nCurBit;


			TQuery<TPointType, TCoord>  nLeftQuery;// запрос с меньшими ключами
			nLeftQuery.m_zMin = curQuery.m_zMin;
			nLeftQuery.m_zMax = highKey;
			nLeftQuery.InitFromZOrder();
			nLeftQuery.m_nBits = curQuery.m_nBits;



			if(nLeftQuery.m_zMin > nLeftQuery.m_zMax)
			{
				std::cout << "Left order Error " <<std::endl;
			}


			TQuery<TPointType, TCoord>  nRightQuery;// запрос с большими ключами
			nRightQuery.m_zMin = lowKey;
			nRightQuery.m_zMax = curQuery.m_zMax;
			nRightQuery.InitFromZOrder();
			nRightQuery.m_nBits = curQuery.m_nBits;

			if(nRightQuery.m_zMin > nRightQuery.m_zMax)
			{

				std::cout << "Right order Error " <<std::endl;
			}
	
}


/*

Min

minX 0				maxX = 10
minY 2040			maxY = 2050


Max


minX   500			maxX = 2047
minY   500			maxY = 3071


*/


void testZRect1()
{
	//testZRect<uint16, embDB::ZOrderRect2DU16>(0xFFFF - 2000 , 0xFFFF - 100, 100);
	//testZRect<uint32, embDB::ZOrderRect2DU32>(0xFFFFFFFF - 2000 , 0xFFFFFFFF - 100, 100);
	//testZRect<uint32, embDB::ZOrderRect2DU32>(0 , 2000, 100);
	//testZRect<uint64, embDB::ZOrderRect2DU64>(0xFFFFFFFFFFFFFFFF - 2000 , 0xFFFFFFFFFFFFFFFF - 100, 100);
	//testZRect<uint64, embDB::ZOrderRect2DU64>(0 , 2000, 100);

	//testSplitZRect<uint16, embDB::ZOrderRect2DU16>(0xFFFF);
	//testSplitZRect<uint32, embDB::ZOrderRect2DU32>(0xFFFFFFFF);
	//testSplitZRect<uint64, embDB::ZOrderRect2DU64>(0xFFFFFFFFFFFFFFFF);
	/*embDB::ZOrderRect2DU16 z1(1,4,4,7);
	embDB::ZOrderRect2DU16 z2(5,1,12,8);
	embDB::ZOrderRect2DU16 z3(14,6,16,9);
	embDB::ZOrderRect2DU16 z4(0,1,2,1);
	embDB::ZOrderRect2DU16 z5(1,8,4,10);
	embDB::ZOrderRect2DU16 z6(4,2,6,2);
	embDB::ZOrderRect2DU16 z7(0,0,16,16);
	embDB::ZOrderRect2DU16 z8(0,0,15,15);
	embDB::ZOrderRect2DU16 z9(0,0,6,2);

	embDB::ZOrderRect2DU16 zLow(0,0,5,1);
	embDB::ZOrderRect2DU16 zHight(12,8,16,16);*/

	embDB::ZOrderRect2DU16 zMin(0,2040,10,2050);
	embDB::ZOrderRect2DU16 zMax(500,500,2047,3071);
	testSplitZRect1<uint16, embDB::ZOrderRect2DU16>(zMin, zMax);

}