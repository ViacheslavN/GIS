#ifndef _EMBEDDED_DATABASE_B_STATIAL_KEY_H_
#define _EMBEDDED_DATABASE_B_STATIAL_KEY_H_
#include "CommonLibrary/general.h"
namespace embDB
{
	template <typename _TPointType >
	struct TPoint2D
	{
	public:
		typedef _TPointType TPointType;
		TPointType m_x;
		TPointType m_y;
	};


	template <typename _TPointType >
	struct TRect2D
	{
	public:
		typedef _TPointType TPointType;

		TPointType m_minX;
		TPointType m_minY;
		TPointType m_maxX;
		TPointType m_maxY;


		TRect2D() : m_minX(0),m_minY(0), m_maxX(-1), m_maxY(-1)
		{}
		TRect2D(TPointType minX, TPointType minY, TPointType maxX, TPointType maxY) :
			m_minX(minX),m_minY(minY), m_maxX(maxX), m_maxY(maxY)
		{}

		TPointType TRect2D::width()  const
		{  
			return m_maxX - m_minX;
		}
		TPointType TRect2D::height()  const
		{  
			return m_maxY - m_minY; 
		}

		bool TRect2D::IsEmpty()  const
		{ 
			return (m_maxX < m_minX || (m_maxY < m_minY)); 
		}

		bool isIntersection(TRect2D& rect)
		{

			if(IsEmpty() || rect.IsEmpty())
				return false;

			if(m_minX >= rect.m_maxX) return false;
			if(m_maxX <= rect.m_minX) return false;
			if(m_minY >= rect.m_maxY) return false;
			if(m_maxY <= rect.m_minY) return false;
			return true;
		}

		void set(TPointType minX, TPointType minY, TPointType maxX, TPointType maxY)
		{
			m_minX = minX;
			m_minY = minY;
			m_maxX = maxX; 
			m_maxY = maxY;
		}
		void expand(TPointType X, TPointType Y)
		{
			if(IsEmpty())
			{
				m_minX = X;
				m_minY = Y;
				m_maxX = X; 
				m_maxY = Y;
				return;
			}
			if(X > m_maxX)
				m_maxX = X;
			else if(X < m_minX)
				m_minX = X;
					
			if(Y > m_maxY)
				m_maxY = Y;
			else if(Y < m_minY)
				m_minY = Y;
		}
		bool isInRectEQ (TRect2D& rect)
		{

			if(m_minX <= rect.m_minX) return false;
			if(m_maxX >= rect.m_maxX) return false;
			if(m_minY <= rect.m_minY) return false;
			if(m_maxY >= rect.m_maxY) return false;
			return true;
		}
		bool isInRect(TRect2D& rect)
		{

			if(m_minX < rect.m_minX) return false;
			if(m_maxX > rect.m_maxX) return false;
			if(m_minY < rect.m_minY) return false;
			if(m_maxY > rect.m_maxY) return false;
			return true;
		}

		bool isPoinInRectEQ (TPointType X, TPointType Y)
		{

			if(m_minX >= X) return false;
			if(m_maxX <= X) return false;
			if(m_minY >= Y) return false;
			if(m_maxY <= Y) return false;
			return true;
		}
		bool isPoinInRect (TPointType X, TPointType Y)
		{

			if(m_minX > X) return false;
			if(m_maxX < X) return false;
			if(m_minY > Y) return false;
			if(m_maxY < Y) return false;
			return true;
		}
	};


	typedef TPoint2D<int16> TPoint2D16;
	typedef TPoint2D<int32> TPoint2D32;
	typedef TPoint2D<int64> TPoint2D64;

	typedef TPoint2D<uint16> TPoint2Du16;
	typedef TPoint2D<uint32> TPoint2Du32;
	typedef TPoint2D<uint64> TPoint2Du64;

	typedef TRect2D<int16> TRect2D16;
	typedef TRect2D<int32> TRect2D32;
	typedef TRect2D<int64> TRect2D64;

	typedef TRect2D<uint16> TRect2Du16;
	typedef TRect2D<uint32> TRect2Du32;
	typedef TRect2D<uint64> TRect2Du64;
	
}
#endif