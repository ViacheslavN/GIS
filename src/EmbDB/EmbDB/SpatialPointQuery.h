#ifndef _EMBEDDED_DATABASE_B_STATIAL_POINT_QUERY_H_
#define _EMBEDDED_DATABASE_B_STATIAL_POINT_QUERY_H_
#include "CommonLibrary/general.h"
#include "SpatialKey.h"
namespace embDB
{


	static const uint32 MASKSPoint[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
	static const uint32 SHIFTSPoint[] = {1, 2, 4, 8, 16, 32};

	 
	//5 0xFFFF0000FFFF     <<16 //111111111111111100000000000000001111111111111111
	//4 0xFF00FF00FF00FF   <<8  //11111111000000001111111100000000111111110000000011111111
	//3 0xF0F0F0F0F0F0F0F  <<4  //111100001111000011110000111100001111000011110000111100001111
	//2 0x3333333333333333 <<2  //11001100110011001100110011001100110011001100110011001100110011
	//1 0x5555555555555555 <<1  //101010101010101010101010101010101010101010101010101010101010101
	static const uint64 MASKSPoint64[] = {0x5555555555555555 , 0x3333333333333333 ,  0xF0F0F0F0F0F0F0F , 0xFF00FF00FF00FF ,0xFFFF0000FFFF};
 
	

	static uint32 getShiftValue16(uint16 nVal)
	{
		uint32  x = nVal;
		x = (x | (x << SHIFTSPoint[3])) & MASKSPoint[3];
		x = (x | (x << SHIFTSPoint[2])) & MASKSPoint[2];
		x = (x | (x << SHIFTSPoint[1])) & MASKSPoint[1];
		x = (x | (x << SHIFTSPoint[0])) & MASKSPoint[0];
		return x;
	}

	static uint64 getShiftValue32(uint32 nVal)
	{
		uint64  x = nVal;
		x = (x | (x << SHIFTSPoint[4])) & MASKSPoint64[4];
		x = (x | (x << SHIFTSPoint[3])) & MASKSPoint64[3];
		x = (x | (x << SHIFTSPoint[2])) & MASKSPoint64[2];
		x = (x | (x << SHIFTSPoint[1])) & MASKSPoint64[1];
		x = (x | (x << SHIFTSPoint[0])) & MASKSPoint64[0];
		return x;
	}


	struct ZOrderPoint2DU16
	{
	
		typedef int16  TPointTypei;
		typedef uint16 TPointType;
		typedef uint32 ZValueType;		
		static const uint16 SizeInByte = 4;
		ZOrderPoint2DU16() : m_nZValue(0)
		{}
		ZOrderPoint2DU16(uint32 nZorder) : m_nZValue(nZorder)
		{}
		ZOrderPoint2DU16(uint16 nX, uint16 nY) : m_nZValue(0)
		{
			setZOrder(nX, nY);
		}
		void setZOrder(uint16 nXPos, uint16 nYPos)
		{
			uint32  x = nXPos;  // Interleave lower 16 bits of x and y, so the bits of x
			uint32  y = nYPos;  // are in the even positions and bits from y in the odd;

			x = getShiftValue16(nXPos);
			y = getShiftValue16(nYPos);
			m_nZValue = y | (x << 1);
		}

		bool operator < (const ZOrderPoint2DU16& zOrder)
		{
			return m_nZValue < zOrder.m_nZValue;
		}
		bool operator <= (const ZOrderPoint2DU16& zOrder)
		{
			return m_nZValue <= zOrder.m_nZValue;
		}
		bool operator > (const ZOrderPoint2DU16& zOrder)
		{
			return m_nZValue > zOrder.m_nZValue;
		}
		bool operator = (uint32 nZorder)
		{
			 m_nZValue = nZorder;
		}
		bool operator == (const ZOrderPoint2DU16&  Zorder)
		{
			return m_nZValue == Zorder.m_nZValue;
		}
		short getBits() const
		{
			return 31;
		}
		uint32 getBit (int idx)
		{
			return uint32(m_nZValue>>(idx&0x1f));
		}

		void splitByBits(int idx)
		{
			uint32 bitMask = 0xAAAAAAAA >> (31 - idx);
			m_nZValue |= bitMask;
			uint32 bit = uint32 (1) << uint32 (idx & 0x1f);
			m_nZValue -= bit;
		}

		void clearLowBits(int idx)
		{
			uint32 bitMask = 0xAAAAAAAA >> (31 - idx);
			m_nZValue &= ~bitMask;
			uint32 bit = uint32 (1) << uint32 (idx & 0x3f);
			m_nZValue |= bit;
		}
		
		void getXY(uint16& x,  uint16& y);
		uint32 m_nZValue;
	};
	//typename ZOrderPoint2D<uint16> PointZOrderU16;
	

	static void SetZOrderInt32(uint64& nZValue, uint32 nXPos, uint32 nYPos)
	{
		uint64  x = nXPos;  // Interleave lower 16 bits of x and y, so the bits of x
		uint64  y = nYPos;  // are in the even positions and bits from y in the odd;

		x = getShiftValue32(nXPos);
		y = getShiftValue32(nYPos);


		nZValue = y | (x << 1);
	}
 
	struct ZOrderPoint2DU32 
	{
		typedef int32  TPointTypei;
		typedef uint32 TPointType;
		typedef uint64 ZValueType;
		static const uint16 SizeInByte = 8;
		ZOrderPoint2DU32() : m_nZValue(0)
		{}
		ZOrderPoint2DU32(uint64 nZorder) : m_nZValue(nZorder)
		{}
		ZOrderPoint2DU32(uint32 nX, uint32 nY) : m_nZValue(0)
		{
			setZOrder(nX, nY);
		}
		void setZOrder(uint32 nXPos, uint32 nYPos)
		{
			SetZOrderInt32(m_nZValue, nXPos, nYPos);
		}
		uint64 getBit (int idx) const
		{
			return uint64(m_nZValue >> (idx & 0x3f));
		}
		void getXY(uint32& x,  uint32& y);
		
		void splitByBits(int idx)
		{
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
			m_nZValue |= bitMask;
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue -= bit;
		}
		void clearLowBits(int idx)
		{
			uint64 bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
			m_nZValue &= ~bitMask;
			uint64 bit = uint64 (1) << uint64 (idx & 0x3f);
			m_nZValue |= bit;
		}
		bool operator < (const ZOrderPoint2DU32& zOrder)
		{
			return m_nZValue < zOrder.m_nZValue;
		}
		bool operator <= (const ZOrderPoint2DU32& zOrder)
		{
			return m_nZValue <= zOrder.m_nZValue;
		}
		bool operator > (const ZOrderPoint2DU32& zOrder)
		{
			return m_nZValue > zOrder.m_nZValue;
		}
		bool operator = (uint64 nZorder)
		{
			m_nZValue = nZorder;
		}
		bool operator == (const ZOrderPoint2DU32&  Zorder)
		{
			return m_nZValue == Zorder.m_nZValue;
		}
		short getBits() const
		{
			return 63;
		}
		uint64 m_nZValue;
	};

	template <class PontType>
	struct ZPointComp
	{
		bool LE(const PontType& _Left, const PontType& _Right){
			return (_Left.m_nZValue < _Right.m_nZValue);
		}
		bool EQ(const PontType& _Left, const PontType& _Right){
			return (_Left.m_nZValue == _Right.m_nZValue);
		}
	};

 
	struct ZOrderPoint2DU64
	{
		typedef uint64 TPointType;
		typedef int64  TPointTypei;
		static const uint16 SizeInByte = 8;
		ZOrderPoint2DU64()
		{
			m_nZValue[0] = 0;
			m_nZValue[1] = 0;
		}
		ZOrderPoint2DU64(uint64 nXPos, uint64 nYPos)
		{
			//m_nZValue[0] = nZorderMax;
			//m_nZValue[1] = nZorderMin;
			setZOrder(nXPos, nYPos);
		}
		void setZOrder(uint64 nXPos, uint64 nYPos)
		{
			uint64 nXmax = nXPos >> 32;
			uint64 nXmin = nXPos & 0xFFFFFFFF;
			
			uint64 nYmax = nYPos >> 32;
			uint64 nYmin = nYPos & 0xFFFFFFFF;

			SetZOrderInt32(m_nZValue[1], (uint32)nXmax, (uint32)nYmax);
			SetZOrderInt32(m_nZValue[0], (uint32)nXmin, (uint32)nYmin);
		}
		void getXY(uint64& x, uint64& y);
		
		uint64 m_nZValue[2];

		uint64 getBit (int idx) const
		{
			int subIndex = idx > 63 ? idx - 64 : idx;
			return (m_nZValue[idx > 63 ? 1 : 0] >> (subIndex & 0x3f));
		}

		void splitByBits(int idx);
		void clearLowBits(int idx);
		
		bool operator < (const ZOrderPoint2DU64& zOrder)
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];
		}
		bool operator <= (const ZOrderPoint2DU64& zOrder)
		{
			if(m_nZValue[1] <= zOrder.m_nZValue[1] )
				return m_nZValue[0] <= zOrder.m_nZValue[0] ;
			return false;
		}
		bool operator > (const ZOrderPoint2DU64& zOrder)
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				 return m_nZValue[1] > zOrder.m_nZValue[1];
			return m_nZValue[0] > zOrder.m_nZValue[0];
		}
		bool operator == (const ZOrderPoint2DU64&  zOrder)
		{
			return m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
		}
		short getBits() const
		{
			return 127;
		}
	};


	struct ZPointComp64
	{
		bool LE(const ZOrderPoint2DU64& _Left, const ZOrderPoint2DU64& _Right)
		{
			if(_Left.m_nZValue[1] != _Right.m_nZValue[1])
				return _Left.m_nZValue[1] < _Right.m_nZValue[1];
			return _Left.m_nZValue[0] < _Right.m_nZValue[0];
		}
		bool EQ(const ZOrderPoint2DU64& _Left, const ZOrderPoint2DU64& _Right)
		{
			return (_Left.m_nZValue[0] == _Right.m_nZValue[0] && _Left.m_nZValue[1] == _Right.m_nZValue[1]);
		}
	};

	/*template<typename _TPointType>
	struct ZOrderRect2D
	{
		typedef _TPointType TPointType;
	};

	template<>
	struct ZOrderRect2D<uint16>
	{
		ZOrderRect2D()
		{
			m_nZValue = 0;
		}
		void setZOrder(uint16 nXMin, uint16 nXMax, uint16 nYMin, uint16 nYMax)
		{
			
		}


		uint64 m_nZValue;
	};
	template<>
	struct ZOrderRect2D<uint32>
	{
		uint64 m_nZValue[2];
	};

	template<>
	struct ZOrderRect2D<uint64>
	{
		uint64 m_nZValue[4];
	};*/


}
#endif