#ifndef _EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_
#define _EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_
#
#include "CommonLibrary/general.h"
#include "SpatialKey.h"
namespace embDB
{
	struct ZOrderRect2DU16
	{
		typedef  int16 TPointTypei;
		typedef  uint16 TPointType;
		typedef  uint64 ZValueType;		
		static const TPointType coordMax = 0xFFFF;
		ZOrderRect2DU16();
		ZOrderRect2DU16(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax);
		void setZOrder(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax);
		void getXY(uint16& xMin, uint16& yMin, uint16& xMax, uint16& yMax);


		uint64 getBit (int idx)
		{
			return uint64(m_nZValue >> (idx & 0x3f));
			//return uint64(m_nZValue & ((uint64)0x1 << (idx & 0x3f)));
		}
		void splitByBits(int idx);
		void clearLowBits(int idx);
		short getBits() const
		{
			return 63;
		}

		bool operator < (const ZOrderRect2DU16& zOrder)
		{
			return m_nZValue < zOrder.m_nZValue;
		}
		bool operator <= (const ZOrderRect2DU16& zOrder)
		{
			return m_nZValue <= zOrder.m_nZValue;
		}
		bool operator > (const ZOrderRect2DU16& zOrder)
		{
			return m_nZValue > zOrder.m_nZValue;
		}
		bool operator == (const ZOrderRect2DU16&  Zorder)
		{
			return m_nZValue == Zorder.m_nZValue;
		}
		uint64 m_nZValue;
	};

	struct ZOrderRect2DU32
	{
		typedef  int32 TPointTypei;
		typedef  uint32 TPointType;
		static const uint16 SizeInByte = 16;
		static const TPointType coordMax = 0xFFFFFFFF;	
		ZOrderRect2DU32();
		ZOrderRect2DU32(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax);
		void setZOrder(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax);
		void getXY(uint32& xMin, uint32& yMin, uint32& xMax, uint32& yMax);
		short getBits() const
		{
			return 127;
		}

		bool operator < (const ZOrderRect2DU32& zOrder)
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];
		}
		bool operator <= (const ZOrderRect2DU32& zOrder)
		{
			if(m_nZValue[1] <= zOrder.m_nZValue[1] )
				return m_nZValue[0] <= zOrder.m_nZValue[0] ;
			return false;
		}
		bool operator > (const ZOrderRect2DU32& zOrder)
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] > zOrder.m_nZValue[1];
			return m_nZValue[0] > zOrder.m_nZValue[0];
		}
		bool operator == (const ZOrderRect2DU32&  zOrder)
		{
			return m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
		}

		uint64 getBit (int idx)
		{
			int subIndex = idx > 63 ? idx - 64 : idx;
			return (m_nZValue[idx > 63 ? 1 : 0] >> (subIndex & 0x3f));
		}

		void splitByBits(int idx);
		void clearLowBits(int idx);

		uint64 m_nZValue[2];
	};



 
	struct ZRect32Comp
	{
		bool LE(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right)
		{
			if(_Left.m_nZValue[1] != _Right.m_nZValue[1] )
				return _Left.m_nZValue[1] < _Right.m_nZValue[1];
			return _Left.m_nZValue[0] < _Right.m_nZValue[0];
		}
		bool EQ(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right)
		{
			return _Left.m_nZValue[1] == _Right.m_nZValue[1] && _Left.m_nZValue[0] == _Right.m_nZValue[0];
		}
	};

	struct ZOrderRect2DU64
	{
		typedef  int64 TPointTypei;
		typedef  uint64 TPointType;
		static const uint16 SizeInByte = 32;
	    static const TPointType coordMax = 0xFFFFFFFFFFFFFFFF;
		ZOrderRect2DU64();
		ZOrderRect2DU64(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax);
		void setZOrder(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax);
		void getXY(uint64& xMin, uint64& yMin, uint64& xMax, uint64& yMax);


		short getBits() const
		{
			return 255;
		}

		bool operator < (const ZOrderRect2DU64& zOrder)
		{
			if(m_nZValue[3] != zOrder.m_nZValue[3] )
				return m_nZValue[3] < zOrder.m_nZValue[3];
			if(m_nZValue[2] != zOrder.m_nZValue[2] )
				return m_nZValue[2] < zOrder.m_nZValue[2];
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];
		}
		bool operator <= (const ZOrderRect2DU64& zOrder)
		{
			if(m_nZValue[3] == zOrder.m_nZValue[3] && m_nZValue[2] == zOrder.m_nZValue[2] && m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0])
				return true;

			if(m_nZValue[3] != zOrder.m_nZValue[3] )
				return m_nZValue[3] < zOrder.m_nZValue[3];
			if(m_nZValue[2] != zOrder.m_nZValue[2] )
				return m_nZValue[2] < zOrder.m_nZValue[2];
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];
		}
		bool operator > (const ZOrderRect2DU64& zOrder)
		{
			if(m_nZValue[3] != zOrder.m_nZValue[3] )
				return m_nZValue[3] > zOrder.m_nZValue[3];
			if(m_nZValue[2] != zOrder.m_nZValue[2] )
				return m_nZValue[2] > zOrder.m_nZValue[2];
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] > zOrder.m_nZValue[1];
			return m_nZValue[0] > zOrder.m_nZValue[0];
		}
		bool operator == (const ZOrderRect2DU64&  zOrder)
		{
			return m_nZValue[3] == zOrder.m_nZValue[3] && m_nZValue[2] == zOrder.m_nZValue[2] && m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
		}

		uint64 getBit (int idx);
		void splitByBits(int idx);
		void clearLowBits(int idx);


		uint64 m_nZValue[4];
	};

	struct ZRect64Comp
	{
		bool LE(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right)
		{
			if(_Left.m_nZValue[3] != _Right.m_nZValue[3] )
				return _Left.m_nZValue[3] < _Right.m_nZValue[3];
			if(_Left.m_nZValue[2] != _Right.m_nZValue[2] )
				return _Left.m_nZValue[2] < _Right.m_nZValue[2];
			if(_Left.m_nZValue[1] != _Right.m_nZValue[1] )
				return _Left.m_nZValue[1] < _Right.m_nZValue[1];
			return _Left.m_nZValue[0] < _Right.m_nZValue[0];
		}
		bool EQ(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right)
		{
			return _Left.m_nZValue[3] == _Right.m_nZValue[3] &&  _Left.m_nZValue[2] == _Right.m_nZValue[2] && _Left.m_nZValue[1] == _Right.m_nZValue[1] &&
				_Left.m_nZValue[1] == _Right.m_nZValue[1] && _Left.m_nZValue[0] == _Right.m_nZValue[0];
		}
	};

}
#endif  _EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_