#ifndef _EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_
#define _EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_
#
#include "CommonLibrary/general.h"
#include "CommonLibrary/SpatialKey.h"
namespace embDB
{

	template <class TPoint, class TZOrder, class TRect>
	bool IsZRectOrdertInRect(const TRect& rect, const TZOrder& zOrder)   
	{
		TPoint xMin, xMax, yMin, yMax;
		zOrder.getXY(xMin, yMin,  xMax, yMax);
 		TRect zRect(xMin, yMin, xMax, yMax);
		return rect.isIntersection(zRect) || rect.isInRect(zRect) || zRect.isInRect(rect);
	}

	struct ZOrderRect2DU16
	{
		typedef  int16 TPointTypei;
		typedef  uint16 TPointType;
		typedef  uint64 ZValueType;		
		typedef CommonLib::TRect2D<uint16> TRect;
		static const TPointType coordMax = 0xFFFF;
		static const uint16 SizeInByte = 8;
		ZOrderRect2DU16();
		ZOrderRect2DU16(ZValueType zValue);
		ZOrderRect2DU16(const TRect& rect);
		ZOrderRect2DU16(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax);
		void setZOrder(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax);
		void getXY(uint16& xMin, uint16& yMin, uint16& xMax, uint16& yMax) const;
		void getXY(TRect& rect) const;

		uint64 getBit (int idx)
		{
			return uint64((m_nZValue >> (idx & 0x3f)) & 0x01);
			//return uint64(m_nZValue & ((uint64)0x1 << (idx & 0x3f)));
		}
		void setLowBits(int idx);
		void clearLowBits(int idx);
		short getBits() const
		{
			return 63;
		}

		bool operator < (const ZOrderRect2DU16& zOrder) const
		{
			return m_nZValue < zOrder.m_nZValue;
		}
		bool operator <= (const ZOrderRect2DU16& zOrder) const
		{
			return m_nZValue <= zOrder.m_nZValue;
		}
		bool operator > (const ZOrderRect2DU16& zOrder) const
		{
			return m_nZValue > zOrder.m_nZValue;
		}
		bool operator == (const ZOrderRect2DU16&  Zorder) const
		{
			return m_nZValue == Zorder.m_nZValue;
		}


		ZOrderRect2DU16 operator - (const ZOrderRect2DU16&  Zorder) const
		{
			return ZOrderRect2DU16(m_nZValue - Zorder.m_nZValue);
		}
		ZOrderRect2DU16 operator + (const ZOrderRect2DU16&  Zorder) const
		{
			return ZOrderRect2DU16(m_nZValue + Zorder.m_nZValue);
		}

		const ZOrderRect2DU16& operator += (const ZOrderRect2DU16&  Zorder)
		{
			m_nZValue += Zorder.m_nZValue;
			return *this;
		}

		bool IsInRect(const CommonLib::TRect2Du16& rect) const 
		{		 
			return IsZRectOrdertInRect<uint16, ZOrderRect2DU16, CommonLib::TRect2Du16>(rect, *this);
		}

		uint64 m_nZValue;
	};

	struct ZRect16Comp
	{
		bool LE(const ZOrderRect2DU16& _Left, const ZOrderRect2DU16& _Right) const
		{
		
			return _Left.m_nZValue < _Right.m_nZValue;
		}
		bool EQ(const ZOrderRect2DU16& _Left, const ZOrderRect2DU16& _Right) const
		{
			return _Left.m_nZValue == _Right.m_nZValue;
		}
	};

	struct ZOrderRect2DU32
	{
		typedef  int32 TPointTypei;
		typedef  uint32 TPointType;
		static const uint16 SizeInByte = 16;
		static const TPointType coordMax = 0xFFFFFFFF;	
		typedef CommonLib::TRect2D<uint32> TRect;
		ZOrderRect2DU32();
		ZOrderRect2DU32(const TRect& rect);
		ZOrderRect2DU32(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax);
		void setZOrder(uint32 xMin, uint32 yMin, uint32 xMax, uint32 yMax) ;
		void getXY(uint32& xMin, uint32& yMin, uint32& xMax, uint32& yMax) const;
		void getXY(TRect& rect) const;
		short getBits() const
		{
			return 127;
		}

		bool operator < (const ZOrderRect2DU32& zOrder) const
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];
		}
		bool operator <= (const ZOrderRect2DU32& zOrder) const
		{
			if(m_nZValue[1] <= zOrder.m_nZValue[1] )
				return m_nZValue[0] <= zOrder.m_nZValue[0] ;
			return false;
		}
		bool operator > (const ZOrderRect2DU32& zOrder) const
		{
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] > zOrder.m_nZValue[1];
			return m_nZValue[0] > zOrder.m_nZValue[0];
		}
		bool operator == (const ZOrderRect2DU32&  zOrder) const
		{
			return m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
		}

		uint64 getBit (int idx)
		{
			int subIndex = idx > 63 ? idx - 64 : idx;
			return (m_nZValue[idx > 63 ? 1 : 0] >> (subIndex & 0x3f));
		}

		void setLowBits(int idx);
		void clearLowBits(int idx);

		bool IsInRect(const CommonLib::TRect2Du32& rect) const 
		{		 
			return IsZRectOrdertInRect<uint32, ZOrderRect2DU32, CommonLib::TRect2Du32>(rect, *this);
		}


		ZOrderRect2DU32 operator - (const ZOrderRect2DU32&  Zorder) const;
		ZOrderRect2DU32 operator + (const ZOrderRect2DU32&  Zorder) const;
		ZOrderRect2DU32& operator += (const ZOrderRect2DU32&  Zorder);
		

		uint64 m_nZValue[2];
	};



 
	struct ZRect32Comp
	{
		bool LE(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right) const
		{
			if(_Left.m_nZValue[1] != _Right.m_nZValue[1] )
				return _Left.m_nZValue[1] < _Right.m_nZValue[1];
			return _Left.m_nZValue[0] < _Right.m_nZValue[0];
		}
		bool EQ(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right) const
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
		typedef CommonLib::TRect2D<uint64> TRect;
		ZOrderRect2DU64();
		ZOrderRect2DU64(const TRect& rect);
		ZOrderRect2DU64(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax);
		void setZOrder(uint64 xMin, uint64 yMin, uint64 xMax, uint64 yMax);
		void getXY(uint64& xMin, uint64& yMin, uint64& xMax, uint64& yMax) const;
		void getXY(TRect& rect) const;

		short getBits() const
		{
			return 255;
		}

		bool less(const ZOrderRect2DU64& zOrder) const
		{
			for(int i = 3; i >= 0; i--)
			{
				if(m_nZValue[i]>zOrder.m_nZValue[i])
					return false;
				if(m_nZValue[i]<zOrder.m_nZValue[i])
					return true;
			}
			return false;
		}

		bool operator < (const ZOrderRect2DU64& zOrder) const
		{
		/*	if(m_nZValue[3] != zOrder.m_nZValue[3] )
				return m_nZValue[3] < zOrder.m_nZValue[3];
			if(m_nZValue[2] != zOrder.m_nZValue[2] )
				return m_nZValue[2] < zOrder.m_nZValue[2];
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] < zOrder.m_nZValue[1];
			return m_nZValue[0] < zOrder.m_nZValue[0];*/

			for(int i = 3; i >= 0; i--)
			{
				if(m_nZValue[i]>zOrder.m_nZValue[i])
					return false;
				if(m_nZValue[i]<zOrder.m_nZValue[i])
					return true;
			}
			return false;
		}
		bool operator <= (const ZOrderRect2DU64& zOrder) const
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
		bool operator > (const ZOrderRect2DU64& zOrder) const
		{

			return !less(zOrder);

			/*if(m_nZValue[3] != zOrder.m_nZValue[3] )
				return m_nZValue[3] > zOrder.m_nZValue[3];
			if(m_nZValue[2] != zOrder.m_nZValue[2] )
				return m_nZValue[2] > zOrder.m_nZValue[2];
			if(m_nZValue[1] != zOrder.m_nZValue[1] )
				return m_nZValue[1] > zOrder.m_nZValue[1];
			return m_nZValue[0] > zOrder.m_nZValue[0];*/
		}
		bool operator == (const ZOrderRect2DU64&  zOrder) const
		{
			return m_nZValue[3] == zOrder.m_nZValue[3] && m_nZValue[2] == zOrder.m_nZValue[2] && m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
		}

		uint64 getBit (int idx);
		void setLowBits(int idx);
		void clearLowBits(int idx);


		bool IsInRect(const CommonLib::TRect2Du64& rect) const 
		{		 
			return IsZRectOrdertInRect<uint64, ZOrderRect2DU64, CommonLib::TRect2Du64>(rect, *this);
		}


		ZOrderRect2DU64 operator - (const ZOrderRect2DU64&  zOrder) const;
		ZOrderRect2DU64 operator + (const ZOrderRect2DU64&  zOrder) const;
		ZOrderRect2DU64& operator += (const ZOrderRect2DU64&  zOrder);


		uint64 m_nZValue[4];
	};

	struct ZRect64Comp
	{
		bool LE(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right) const
		{
			/*if(_Left.m_nZValue[3] != _Right.m_nZValue[3] )
				return _Left.m_nZValue[3] < _Right.m_nZValue[3];
			if(_Left.m_nZValue[2] != _Right.m_nZValue[2] )
				return _Left.m_nZValue[2] < _Right.m_nZValue[2];
			if(_Left.m_nZValue[1] != _Right.m_nZValue[1] )
				return _Left.m_nZValue[1] < _Right.m_nZValue[1];
			return _Left.m_nZValue[0] < _Right.m_nZValue[0];*/

			for(int i = 3; i >= 0; i--)
			{
				if(_Left.m_nZValue[i]>_Right.m_nZValue[i])
					return false;
				if(_Left.m_nZValue[i]<_Right.m_nZValue[i])
					return true;
			}
			return false;


		}
		bool EQ(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right) const
		{
			return _Left.m_nZValue[3] == _Right.m_nZValue[3] &&  _Left.m_nZValue[2] == _Right.m_nZValue[2] && _Left.m_nZValue[1] == _Right.m_nZValue[1] &&
				_Left.m_nZValue[1] == _Right.m_nZValue[1] && _Left.m_nZValue[0] == _Right.m_nZValue[0];
		}
	};

}
#endif  //_EMBEDDED_DATABASE_B_STATIAL_RECT_QUERY_H_