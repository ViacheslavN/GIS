#include "stdafx.h"
#include "SpatialRectQuery.h"
//#include "tableZRectt8Bit.h"
#include "tableZRectBack4Bit.h"
#define  ZRECT_TABLE_8_BITS

#ifdef ZRECT_TABLE_8_BITS
	#include "tableZRect8Bit.h"
#else
	#include "tableZRect16Bit.h"
#endif
namespace embDB
{


	uint64 getZCompu16(uint16 nVal)
	{
#ifdef ZRECT_TABLE_8_BITS
		uint64 nRet = 0;
		uint16 nMinVal = nVal & 0xFF;
		uint16 nMaxVal = nVal >> 8;

		nRet = TableZRect8Bit[nMinVal];
		nRet |=	TableZRect8Bit[nMaxVal] << 32;
		return nRet;
#else
		return TableZRect16Bit[nVal];
#endif
		
	}
 
	//0x1111111111111111
	uint64 getZValue16(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax)
	{
		uint64 xZMin = getZCompu16(xMin);
		uint64 yZMin = getZCompu16(yMin);
		uint64 xZMax = getZCompu16(xMax);
		uint64 yZMax = getZCompu16(yMax);
		uint64 nVal = xZMin | (yZMin << 1) | (xZMax << 2) | (yZMax << 3);
		return nVal;
	}



	uint16 getFromZu16(uint64 nZVal)
	{
		uint16 nRet = BackTableZRect4Bit[nZVal & 0xf];
		for (uint16 i = 1; i < 16; ++i)
		{
			nRet |= BackTableZRect4Bit[(nZVal >> 4 * i) & 0xf] << i;
		}
		return nRet;
	}
	void getXYFromZValue16(uint16& xMin, uint16& yMin, uint16& xMax, uint16& yMax, uint64 nZValue)
	{
		xMin = getFromZu16(nZValue & 0x1111111111111111);
		yMin = getFromZu16((nZValue >> 1 ) & 0x1111111111111111);
		xMax = getFromZu16((nZValue >> 2 ) & 0x1111111111111111);
		yMax = getFromZu16((nZValue >> 3 ) & 0x1111111111111111);
	}



	void setZValue32(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax, uint64& zValLow, uint64& zValHigh)
	{
		uint16 xLowMin = xMin & 0xFFFF;
		uint16 yLowMin = yMin & 0xFFFF;
		uint16 xLowMax = xMax & 0xFFFF;
		uint16 yLowMax = yMax & 0xFFFF;

		zValLow = getZValue16(xLowMin, yLowMin, xLowMax, yLowMax);

		uint16 xHighMin = xMin >> 16;
		uint16 yHighMin = yMin  >> 16;
		uint16 xHighMax = xMax  >> 16;
		uint16 yHighMax = yMax  >> 16;

		zValHigh = getZValue16(xHighMin, yHighMin, xHighMax, yHighMax);
	}
	void getXYFromZValue32(uint32& xMin, uint32& yMin, uint32& xMax, uint32& yMax, uint64 zValLow, uint64 zValHigh)
	{
		uint16 xLowMin = 0;
		uint16 yLowMin = 0;
		uint16 xLowMax = 0;
		uint16 yLowMax = 0;
		getXYFromZValue16(xLowMin, yLowMin, xLowMax, yLowMax, zValLow);

		uint16 xHighMin = 0;
		uint16 yHighMin = 0;
		uint16 xHighMax = 0;
		uint16 yHighMax = 0;
		getXYFromZValue16(xHighMin, yHighMin, xHighMax, yHighMax, zValHigh);

		xMin = (uint32)xLowMin | ((uint32)xHighMin << 16);
		yMin = (uint32)yLowMin | ((uint32)yHighMin << 16);
		xMax = (uint32)xLowMax | ((uint32)xHighMax << 16);
		yMax = (uint32)yLowMax | ((uint32)yHighMax << 16);
	}
	ZOrderRect2DU16::ZOrderRect2DU16() : m_nZValue(0)
	{

	}
	ZOrderRect2DU16::ZOrderRect2DU16(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax)
	{
		setZOrder(xMin, yMin, xMax, yMax);
	}
	void ZOrderRect2DU16::setZOrder(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax)
	{
		/*uint64 xZMin = getZCompu16(xMin);
		uint64 yZMin = getZCompu16(yMin);
		uint64 xZMax = getZCompu16(xMax);
		uint64 yZMax = getZCompu16(yMax);
		m_nZValue = xZMin | (yZMin << 1) | (xZMax << 2) | (yZMax << 3);*/
		m_nZValue = getZValue16(xMin, yMin, xMax, yMax);
	}

	void ZOrderRect2DU16::getXY(uint16& xMin, uint16& yMin, uint16& xMax, uint16& yMax)
	{
		/*xMin = getFromZu16(m_nZValue & 0x1111111111111111);
		yMin = getFromZu16((m_nZValue >> 1 ) & 0x1111111111111111);
		xMax = getFromZu16((m_nZValue >> 2 ) & 0x1111111111111111);
		yMax = getFromZu16((m_nZValue >> 3 ) & 0x1111111111111111);*/
		getXYFromZValue16(xMin, yMin, xMax, yMax, m_nZValue);
	}


	void ZOrderRect2DU16::splitByBits(int idx)
	{					  
		uint64 bitMask = 0x8888888888888888 >> (63 - idx);
	
		uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
		//m_nZValue |= bitMask;
		m_nZValue -= bit;
		m_nZValue |= (bitMask & (bit - 1));
	}
	void ZOrderRect2DU16::clearLowBits(int idx)
	{
		uint64 bitMask = 0x8888888888888888 >> (63 - idx);
		//m_nZValue &= (~bitMask);
		uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
		m_nZValue |= bit;
		m_nZValue &= ~(bitMask & (bit - 1));
	}

	ZOrderRect2DU32::ZOrderRect2DU32()
	{
		 m_nZValue[0] = 0;
		 m_nZValue[1] = 0;
	}
	ZOrderRect2DU32::ZOrderRect2DU32(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax)
	{
		setZOrder(xMin, yMin, xMax, yMax);
	}




	void ZOrderRect2DU32::setZOrder(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax)
	{
		/*uint16 xLowMin = xMin & 0xFFFF;
		uint16 yLowMin = yMin & 0xFFFF;
		uint16 xLowMax = xMax & 0xFFFF;
		uint16 yLowMax = yMax & 0xFFFF;
		 
		m_nZValue[0] = getZvalu16(xLowMin, yLowMin, xLowMax, yLowMax);
		
		uint16 xHighMin = xMin >> 16;
		uint16 yHighMin = yMin  >> 16;
		uint16 xHighMax = xMax  >> 16;
		uint16 yHighMax = yMax  >> 16;

		m_nZValue[1] = getZvalu16(xHighMin, yHighMin, xHighMax, yHighMax);*/
		setZValue32(xMin, yMin, xMax, yMax, m_nZValue[0], m_nZValue[1]);

	}

	

	void ZOrderRect2DU32::getXY(uint32& xMin, uint32& yMin, uint32& xMax, uint32& yMax)
	{
		/*uint16 xLowMin;
		uint16 yLowMin;
		uint16 xLowMax;
		uint16 yLowMax;
		getXYFromZValue16(xLowMin, yLowMin, xLowMax, yLowMax, m_nZValue[0]);

		uint16 xHighMin;
		uint16 yHighMin;
		uint16 xHighMax;
		uint16 yHighMax;
		getXYFromZValue16(xHighMin, yHighMin, xHighMax, yHighMax, m_nZValue[1]);

		xMin = (uint32)xLowMin | ((uint32)(xHighMin << 16));
		yMin = (uint32)yLowMin | ((uint32)(yHighMin << 16));
		xMax = (uint32)xLowMax | ((uint32)(xHighMax << 16));
		yMax = (uint32)yLowMax | ((uint32)(yHighMax << 16));*/
		getXYFromZValue32(xMin, yMin, xMax, yMax, m_nZValue[0], m_nZValue[1]);
	}

	void ZOrderRect2DU32::splitByBits(int idx)
	{

		if(idx > 63)
		{
			int subIndex = idx - 64;
			uint64 bit = uint64 (1) << uint64 (subIndex & 0x3f);
			uint64 bitMask = 0x8888888888888888 >> (63 - subIndex);
			m_nZValue[1] -= bit;
			m_nZValue[1] |= (bitMask& (bit - 1));
	
			bitMask = 0x8888888888888888 >> (3 - subIndex % 4);
			m_nZValue[0] |= (bitMask& (bit - 1));
		}
		else
		{
			uint64 bitMask = 0x8888888888888888 >> (63 - idx);
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue[0] -= bit;
			m_nZValue[0] |= (bitMask& (bit - 1));
		}

	}




	void ZOrderRect2DU32::clearLowBits(int idx)
	{
		if(idx > 63)
		{
			int subIndex =  idx - 64;
			uint64 bitMask = 0x8888888888888888 >> (63 - subIndex);
			m_nZValue[1] &= ~bitMask;
			uint64 bit = uint64 (1) << uint64 (subIndex & 0x3f);
			m_nZValue[1] |=  bit;
			bitMask = 0x8888888888888888 >> (3 - subIndex % 4);
			m_nZValue[0] &= ~(bitMask& (bit - 1));
		}
		else
		{
			uint64 bitMask = 0x8888888888888888 >> (63 - idx);
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue[0] |= bit;
			m_nZValue[0] &= ~(bitMask & (bit - 1));
		}
	}


	ZOrderRect2DU64::ZOrderRect2DU64()
	{
		m_nZValue[0] = m_nZValue[1] = m_nZValue[2] = m_nZValue[3] = 0;
	}
	ZOrderRect2DU64::ZOrderRect2DU64(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax)
	{
		setZOrder(xMin, yMin, xMax, yMax);
	}
	void ZOrderRect2DU64::setZOrder(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax)
	{
		uint32 xLowMin = xMin & 0xFFFFFFFF;
		uint32 yLowMin = yMin & 0xFFFFFFFF;
		uint32 xLowMax = xMax & 0xFFFFFFFF;
		uint32 yLowMax = yMax & 0xFFFFFFFF;

		setZValue32(xLowMin, yLowMin, xLowMax, yLowMax, m_nZValue[0], m_nZValue[1]);

		uint32 xHighMin = xMin >> 32;
		uint32 yHighMin = yMin >> 32;
		uint32 xHighMax = xMax >> 32;
		uint32 yHighMax = yMax >> 32;

		 setZValue32(xHighMin, yHighMin, xHighMax, yHighMax, m_nZValue[2], m_nZValue[3]);
	}
	void ZOrderRect2DU64::getXY(uint64& xMin, uint64& yMin, uint64& xMax, uint64& yMax)
	{
		uint32 xLowMin = 0;
		uint32 yLowMin = 0;
		uint32 xLowMax = 0;
		uint32 yLowMax = 0;

		getXYFromZValue32(xLowMin, yLowMin, xLowMax, yLowMax, m_nZValue[0], m_nZValue[1]);


		uint32 xHighMin = 0;
		uint32 yHighMin = 0;
		uint32 xHighMax = 0;
		uint32 yHighMax = 0;
		getXYFromZValue32(xHighMin, yHighMin, xHighMax, yHighMax, m_nZValue[2], m_nZValue[3]);


		xMin = (uint64)xLowMin | ((uint64)xHighMin << 32);
		yMin = (uint64)yLowMin | ((uint64)yHighMin << 32);
		xMax = (uint64)xLowMax | ((uint64)xHighMax << 32);
		yMax = (uint64)yLowMax | ((uint64)yHighMax << 32);
	}
	uint64 ZOrderRect2DU64::getBit (int idx)
	{
		int nBlock = (idx & 0xff) >> 6;
		int nSubIndex = idx - (64 * nBlock);
		return (m_nZValue[nBlock] >> (nSubIndex & 0x3f));
	}

	void ZOrderRect2DU64::splitByBits(int idx)
	{

		/*
		uint64 bitMask = 0x8888888888888888 >> (63 - idx);
		uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
		m_nZValue[0] -= bit;
		m_nZValue[0] |= (bitMask& (bit - 1));
		*/

		int nBlock = (idx & 0xff) >> 6;
		int nSubIndex = idx - (64 * nBlock);
		uint64 bitMask = 0x8888888888888888 >> (63 - nSubIndex);
		uint64 bit = uint64 (1) << uint64 (nSubIndex & 0x3f);
		m_nZValue[nBlock] -= bit;
		m_nZValue[nBlock] |= (bitMask & (bit - 1));
	
		
		bitMask = 0x8888888888888888 >> (3 - nSubIndex % 4);
		for (int i = nBlock - 1; i >= 0; --i)
		{
			m_nZValue[i] |= (bitMask & (bit - 1));
		}
	}

	void ZOrderRect2DU64::clearLowBits(int idx)
	{


		int nBlock = (idx & 0xff) >> 6;
		int nSubIndex = idx - (64 * nBlock);
		uint64 bitMask = 0x8888888888888888 >> (63 - nSubIndex);
		uint64 bit = uint64 (1) << uint64 (nSubIndex & 0x3f);
		m_nZValue[nBlock] |=  bit;
		m_nZValue[nBlock] &= ~(bitMask& (bit - 1));
			
		bitMask = 0x8888888888888888 >> (3 - nSubIndex % 4);
		for (int i = nBlock - 1; i >= 0; --i)
		{
			m_nZValue[i] &= ~(bitMask & (bit - 1));
		}
	}
}