#include "stdafx.h"
#include "SpatialPointQuery.h"
#include "BackTableZPoint16Bit.h"
namespace embDB
{
	void ZOrderPoint2DU16::getXY(uint16& x, uint16& y)
	{
		uint32 zY = m_nZValue & MASKSPoint[0];
		uint32 zX = (m_nZValue >> 1) & MASKSPoint[0];

		x = BackTableZpoint16[zX & 0xFFFF] | (BackTableZpoint16[(zX >> 16) & 0xFFFF]) << 8;
		y = BackTableZpoint16[zY & 0xFFFF] | (BackTableZpoint16[(zY >> 16) & 0xFFFF]) << 8;
		
	}

	uint32 getX32FromZ(uint64 zX)
	{
		return BackTableZpoint16[zX & 0xFFFF] | (BackTableZpoint16[(zX >> 16) & 0xFFFF]) << 8
			| (BackTableZpoint16[(zX >> 32) & 0xFFFF]) << 16 | (BackTableZpoint16[(zX >> 48) & 0xFFFF]) << 24;
	}
	void ZOrderPoint2DU32::getXY(uint32& x, uint32& y)
	{
		uint64 zY = m_nZValue & MASKSPoint64[0];
		uint64 zX = (m_nZValue >> 1) & MASKSPoint64[0];
		x = getX32FromZ(zX);
		y = getX32FromZ(zY);
	}


	void ZOrderPoint2DU64::getXY(uint64& x,  uint64& y)
	{
		uint64 zYMin = m_nZValue[0] & MASKSPoint64[0];
		uint64 zXMin = (m_nZValue[0] >> 1) & MASKSPoint64[0];

		uint64 zYMax = m_nZValue[1] & MASKSPoint64[0];
		uint64 zXMax = (m_nZValue[1] >> 1) & MASKSPoint64[0];

		uint64 nXmax = getX32FromZ(zXMax);
		uint64 nXmin = getX32FromZ(zXMin);

		uint64 nYmax = getX32FromZ(zYMax);
		uint64 nYmin = getX32FromZ(zYMin);

		x = nXmin | (nXmax << 32);
		y = nYmin | (nYmax << 32);
	}

	void ZOrderPoint2DU64::splitByBits(int idx)
	{

		if(idx > 63)
		{
			int subIndex = idx - 64;
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - subIndex);
			m_nZValue[1] |= bitMask;
			uint64 bit = uint64 (1) << uint64 (subIndex & 0x3f);
			m_nZValue[1] -= bit;
			bitMask = 0xAAAAAAAAAAAAAAAA >> (idx % 2 ? 0 : 1);
			m_nZValue[0] |= bitMask;
		}
		else
		{
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
			m_nZValue[0] |= bitMask;
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue[0] -= bit;
		}

	}

	void ZOrderPoint2DU64::clearLowBits(int idx)
	{
		if(idx > 63)
		{
			int subIndex =  idx - 64;
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - subIndex);
			m_nZValue[1] &= ~bitMask;
			uint64 bit = uint64 (1) << uint64 (subIndex & 0x3f);
			m_nZValue[1] |=  bit;
			bitMask = 0xAAAAAAAAAAAAAAAA >> (idx % 2 ? 0 : 1);
			m_nZValue[0] &= ~bitMask;
		}
		else
		{
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
			m_nZValue[0] &= ~bitMask;
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue[0] |=  bit;
		}
	}
}
