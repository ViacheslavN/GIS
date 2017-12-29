#include "stdafx.h"
#include "GraphTypes.h"
#include "DisplayMath.h"
#include "Point.h"
#include "LineGenerator.h"
namespace GisEngine
{

	namespace Display
	{
		CLineGenerator::CLineGenerator() : m_capType(CapTypeButt), m_JoinType(JoinTypeMiter), m_InnerJoinType(InnerJoinTypeMiter)
			, m_dWidth(0.5),
			m_dWidth_abs(0.5),
			m_dWidthEps(0.5 / 1024.0),
			m_nWidth_sign(1),
			m_dMiter_limit(4.0),
			m_dInner_miter_limit(1.01)
		{

		}
		CLineGenerator::~CLineGenerator()
		{

		}

		void CLineGenerator::SetWidth(double dWidth)
		{
			m_dWidth = dWidth * 0.5;
			if (m_dWidth < 0)
			{
				m_dWidth_abs = -m_dWidth;
				m_nWidth_sign = -1;
			}
			else
			{
				m_dWidth_abs = m_dWidth;
				m_nWidth_sign = 1;
			}
			m_dWidthEps = m_dWidth / 1024.0;
		}
		void CLineGenerator::add_vertex(double x, double y)
		{
			m_out.push_back(GPoint(x, y));
		}
		void CLineGenerator::CalcCap(const GPoint& pt0, const GPoint& pt1)
		{


			double len = calc_distance(pt0, pt1);

			double dx1 = (pt1.y - pt0.y) / len;
			double dy1 = (pt1.x - pt0.x) / len;
			double dx2 = 0;
			double dy2 = 0;

			dx1 *= m_dWidth;
			dy1 *= m_dWidth;

			if (m_capType != CapTypeRound)
			{
				if (m_capType == CapTypeSquare)
				{
					dx2 = dy1 * m_nWidth_sign;
					dy2 = dx1 * m_nWidth_sign;
				}
				add_vertex(pt0.x - dx1 - dx2, pt0.y + dy1 - dy2);
				add_vertex(pt0.x + dx1 - dx2, pt0.y - dy1 - dy2);
			}
			else
			{
				double da = acos(m_dWidth_abs / (m_dWidth_abs + 0.125 / m_dApprox_scale)) * 2;
				double a1;
				int i;
				int n = int(PI / da);

				da = PI / (n + 1);
				add_vertex(pt0.x - dx1, pt0.y + dy1);
				if (m_nWidth_sign > 0)
				{
					a1 = atan2(dy1, -dx1);
					a1 += da;
					for (i = 0; i < n; i++)
					{
						add_vertex(pt0.x + cos(a1) * m_dWidth, pt0.y + sin(a1) * m_dWidth);
						a1 += da;
					}
				}
				else
				{
					a1 = atan2(-dy1, dx1);
					a1 -= da;
					for (i = 0; i < n; i++)
					{
						add_vertex(pt0.x + cos(a1) * m_dWidth, pt0.y + sin(a1) * m_dWidth);
						a1 -= da;
					}
				}
				add_vertex(pt0.x + dx1, pt0.y - dy1);
			}
		}


		void CLineGenerator::GenerateLine(const TVecPoints& inVecLine)
		{

			uint32 nSize = inVecLine.size();

			if (inVecLine.size() < 2)
				return;

			m_out.clear();

			uint32 m_nCurrPoint = 0;
			bool bIsLoop = inVecLine[0] == inVecLine[inVecLine.size() - 1];
			eFlags 	nStatus = bIsLoop ? eInLine : eCapIn;

			while (nStatus != eStop)
			{

				switch (nStatus)
				{
				case eCapIn:
					CalcCap(inVecLine[0], inVecLine[1]);
					nStatus = eInLine;
					m_nCurrPoint = 1;
					break;
				case eCapOut:
					CalcCap(inVecLine[inVecLine.size() - 1], inVecLine[inVecLine.size() - 2]);
					nStatus = eOutLine;
					break;
				case eInLine:
					if (bIsLoop)
					{
						if (m_nCurrPoint >= inVecLine.size())
						{
							nStatus = eStop;
							break;
						}
					}
					else
					{
						if (m_nCurrPoint >= inVecLine.size() - 1)
						{
							nStatus = eCapOut;
							break;
						}
					}
					{

						uint32 nPrevIndex = (m_nCurrPoint + nSize - 1) % nSize;
						uint32 nNextIndex = (m_nCurrPoint + 1) % nSize;
						CalcJoin(inVecLine[nPrevIndex], inVecLine[m_nCurrPoint], inVecLine[nNextIndex]);
						++m_nCurrPoint;
					}
					break;
				case eOutLine:
				{

					if (m_nCurrPoint <= unsigned(bIsLoop == 0))
					{
						nStatus = eStop;
						break;
					}

					--m_nCurrPoint;

					uint32 nPrevIndex = (m_nCurrPoint + nSize - 1) % nSize;
					uint32 nNextIndex = (m_nCurrPoint + 1) % nSize;
					CalcJoin(inVecLine[nNextIndex], inVecLine[m_nCurrPoint], inVecLine[nPrevIndex]);

				}
				break;
				case eStop:
					break;
				default:
					break;
				}


			}
		}


		void CLineGenerator::calc_arc(
			double x, double y,
			double dx1, double dy1,
			double dx2, double dy2)
		{
			double a1 = atan2(dy1 * m_nWidth_sign, dx1 * m_nWidth_sign);
			double a2 = atan2(dy2 * m_nWidth_sign, dx2 * m_nWidth_sign);
			double da = a1 - a2;
			int i, n;

			da = acos(m_dWidth_abs / (m_dWidth_abs + 0.125 / m_dApprox_scale)) * 2;

			add_vertex(x + dx1, y + dy1);
			if (m_nWidth_sign > 0)
			{
				if (a1 > a2) a2 += 2 * PI;
				n = int((a2 - a1) / da);
				da = (a2 - a1) / (n + 1);
				a1 += da;
				for (i = 0; i < n; i++)
				{
					add_vertex(x + cos(a1) * m_dWidth, y + sin(a1) * m_dWidth);
					a1 += da;
				}
			}
			else
			{
				if (a1 < a2) a2 -= 2 * PI;
				n = int((a1 - a2) / da);
				da = (a1 - a2) / (n + 1);
				a1 -= da;
				for (i = 0; i < n; i++)
				{
					add_vertex(x + cos(a1) * m_dWidth, y + sin(a1) * m_dWidth);
					a1 -= da;
				}
			}
			add_vertex(x + dx2, y + dy2);
		}


		void CLineGenerator::calc_miter(const GPoint& v0,
			const GPoint& v1,
			const GPoint& v2,
			double dx1, double dy1,
			double dx2, double dy2,
			eJoinType lj,
			double mlimit,
			double dbevel)
		{
			double xi = v1.x;
			double yi = v1.y;
			double di = 1;
			double lim = m_dWidth_abs * mlimit;
			bool miter_limit_exceeded = true; // Assume the worst
			bool intersection_failed = true; // Assume the worst

			if (calc_intersection(v0.x + dx1, v0.y - dy1,
				v1.x + dx1, v1.y - dy1,
				v1.x + dx2, v1.y - dy2,
				v2.x + dx2, v2.y - dy2,
				&xi, &yi))
			{
				// Calculation of the intersection succeeded
				//---------------------
				di = calc_distance(v1.x, v1.y, xi, yi);
				if (di <= lim)
				{
					// Inside the miter limit
					//---------------------
					add_vertex(xi, yi);
					miter_limit_exceeded = false;
				}
				intersection_failed = false;
			}
			else
			{
				// Calculation of the intersection failed, most probably
				// the three points lie one straight line. 
				// First check if v0 and v2 lie on the opposite sides of vector: 
				// (v1.x, v1.y) -> (v1.x+dx1, v1.y-dy1), that is, the perpendicular
				// to the line determined by vertices v0 and v1.
				// This condition determines whether the next line segments continues
				// the previous one or goes back.
				//----------------
				double x2 = v1.x + dx1;
				double y2 = v1.y - dy1;
				if ((cross_product(v0.x, v0.y, v1.x, v1.y, x2, y2) < 0.0) ==
					(cross_product(v1.x, v1.y, v2.x, v2.y, x2, y2) < 0.0))
				{
					// This case means that the next segment continues 
					// the previous one (straight line)
					//-----------------
					add_vertex(v1.x + dx1, v1.y - dy1);
					miter_limit_exceeded = false;
				}
			}

			if (miter_limit_exceeded)
			{
				// Miter limit exceeded
				//------------------------
				switch (lj)
				{
				case JoinTypeMiterRevert:
					// For the compatibility with SVG, PDF, etc, 
					// we use a simple bevel join instead of
					// "smart" bevel
					//-------------------
					add_vertex(v1.x + dx1, v1.y - dy1);
					add_vertex(v1.x + dx2, v1.y - dy2);
					break;

				case JoinTypeMiterRound:
					calc_arc(v1.x, v1.y, dx1, -dy1, dx2, -dy2);
					break;

				default:
					// If no miter-revert, calculate new dx1, dy1, dx2, dy2
					//----------------
					if (intersection_failed)
					{
						mlimit *= m_nWidth_sign;
						add_vertex(v1.x + dx1 + dy1 * mlimit,
							v1.y - dy1 + dx1 * mlimit);
						add_vertex(v1.x + dx2 - dy2 * mlimit,
							v1.y - dy2 - dx2 * mlimit);
					}
					else
					{
						double x1 = v1.x + dx1;
						double y1 = v1.y - dy1;
						double x2 = v1.x + dx2;
						double y2 = v1.y - dy2;
						di = (lim - dbevel) / (di - dbevel);
						add_vertex(x1 + (xi - x1) * di, y1 + (yi - y1) * di);
						add_vertex(x2 + (xi - x2) * di, y2 + (yi - y2) * di);
					}
					break;
				}
			}
		}

		void CLineGenerator::CalcJoin(const GPoint& pt0, const GPoint& pt1, const GPoint& pt2)
		{

			double len1 = calc_distance(pt0, pt1);
			double len2 = calc_distance(pt1, pt2);


			double dx1 = m_dWidth * (pt1.y - pt0.y) / len1;
			double dy1 = m_dWidth * (pt1.x - pt0.x) / len1;
			double dx2 = m_dWidth * (pt2.y - pt1.y) / len2;
			double dy2 = m_dWidth * (pt2.x - pt1.x) / len2;


			double cp = cross_product(pt0.x, pt0.y, pt1.x, pt1.y, pt2.x, pt2.y);
			if (cp != 0 && (cp > 0) == (m_dWidth > 0))
			{
				// Inner join
				//---------------
				double limit = ((len1 < len2) ? len1 : len2) / m_dWidth_abs;
				if (limit < m_dInner_miter_limit)
				{
					limit = m_dInner_miter_limit;
				}

				switch (m_InnerJoinType)
				{
				default: // inner_bevel
					add_vertex(pt1.x + dx1, pt1.y - dy1);
					add_vertex(pt1.x + dx2, pt1.y - dy2);
					break;

				case InnerJoinTypeMiter:
					calc_miter(pt0, pt1, pt2, dx1, dy1, dx2, dy2, JoinTypeMiterRevert, limit, 0);
					break;

				case InnerJoinTypeJag:
				case InnerJoinTypeRound:
					cp = (dx1 - dx2) * (dx1 - dx2) + (dy1 - dy2) * (dy1 - dy2);
					if (cp < len1 * len1 && cp < len2 * len2)
					{
						calc_miter(pt0, pt1, pt2, dx1, dy1, dx2, dy2, JoinTypeMiterRevert, limit, 0);
					}
					else
					{
						if (m_InnerJoinType == InnerJoinTypeJag)
						{
							add_vertex(pt1.x + dx1, pt1.y - dy1);
							add_vertex(pt1.x, pt1.y);
							add_vertex(pt1.x + dx2, pt1.y - dy2);
						}
						else
						{
							add_vertex(pt1.x + dx1, pt1.y - dy1);
							add_vertex(pt1.x, pt1.y);
							calc_arc(pt1.x, pt1.y, dx2, -dy2, dx1, -dy1);
							add_vertex(pt1.x, pt1.y);
							add_vertex(pt1.x + dx2, pt1.y - dy2);
						}
					}
					break;
				}
			}
			else
			{
				// Outer join
				//---------------

				// Calculate the distance between v1 and 
				// the central point of the bevel line segment
				//---------------
				double dx = (dx1 + dx2) / 2;
				double dy = (dy1 + dy2) / 2;
				double dbevel = sqrt(dx * dx + dy * dy);

				if (m_JoinType == JoinTypeRound || m_JoinType == JoinTypeBevel)
				{
					// This is an optimization that reduces the number of points 
					// in cases of almost collinear segments. If there's no
					// visible difference between bevel and miter joins we'd rather
					// use miter join because it adds only one point instead of two. 
					//
					// Here we calculate the middle point between the bevel points 
					// and then, the distance between v1 and this middle point. 
					// At outer joins this distance always less than stroke width, 
					// because it's actually the height of an isosceles triangle of
					// v1 and its two bevel points. If the difference between this
					// width and this value is small (no visible bevel) we can 
					// add just one point. 
					//
					// The constant in the expression makes the result approximately 
					// the same as in round joins and caps. You can safely comment 
					// out this entire "if".
					//-------------------
					if (m_dApprox_scale * (m_dWidth_abs - dbevel) < m_dWidthEps)
					{
						if (calc_intersection(pt0.x + dx1, pt0.y - dy1,
							pt1.x + dx1, pt1.y - dy1,
							pt1.x + dx2, pt1.y - dy2,
							pt2.x + dx2, pt2.y - dy2,
							&dx, &dy))
						{
							add_vertex(dx, dy);
						}
						else
						{
							add_vertex(pt1.x + dx1, pt1.y - dy1);
						}
						return;
					}
				}

				switch (m_JoinType)
				{
				case JoinTypeMiter:
				case JoinTypeMiterRevert:
				case JoinTypeMiterRound:
					calc_miter(pt0, pt1, pt2, dx1, dy1, dx2, dy2, m_JoinType, m_dMiter_limit, dbevel);
					break;

				case JoinTypeRound:
					calc_arc(pt1.x, pt1.y, dx1, -dy1, dx2, -dy2);
					break;

				default: // Bevel join
					add_vertex(pt1.x + dx1, pt1.y - dy1);
					add_vertex(pt1.x + dx2, pt1.y - dy2);
					break;
				}
			}
		}
	}
}