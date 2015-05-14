#include "stdafx.h"
#include "Point.h"
namespace GisEngine
{
	namespace Display
	{
		GPoint::GPoint() : m_x(0), m_y(0)
		{}
		GPoint::GPoint(GUnits X, GUnits Y) :m_x(X), m_y(Y)
		{}
		GPoint::GPoint(const GPoint& gp) : m_x(gp.m_x), m_y(gp.m_y)
		{}

		bool    GPoint::operator==(const GPoint& gp) const
		{
			return m_x == gp.m_x && m_y == gp.m_y;
		}
		bool    GPoint::operator!=(const GPoint& gp) const
		{
			return m_x != gp.m_x || m_y != gp.m_y;
		}
		const GPoint & GPoint::operator+=(const GPoint& gp)
		{
			m_x += gp.m_x;
			m_y += gp.m_y; 
			return *this;
		}
		const GPoint &GPoint::operator-=(const GPoint& gp )
		{
			m_x -= gp.m_x;
			m_y -= gp.m_y;
			return *this;
		}
	}
}