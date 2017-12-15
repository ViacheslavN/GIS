#include "stdafx.h"
#include "ClipLine.h"
/*
		left	central	right
top		1001	1000	1010
central	0001	0000	0010
bottom	0101	0100	0110

*/
namespace GisEngine
{

	namespace Display
	{
		ClipLine::ClipLine() : m_bFirst(true), m_nBeginPos(0)
		{

		}
		ClipLine::~ClipLine()
		{

		}

		void ClipLine::SetClipBox(const GRect& clipBox)
		{
			m_clipBox = clipBox;
		}
		void ClipLine::SetPointDst(TVecPoints *pPoints, TVecParts *pParts)
		{
			m_pVecPoints = pPoints;
			m_pVecParts = pParts;
		}
		void ClipLine::BeginLine(bool bAllPointInBox)
		{
			m_nBeginPos = m_pVecPoints->size();
			m_bFirst = true;
		}

		void ClipLine::AddVertex(const GPoint& pt, bool bAllPointInBox)
		{
			if (bAllPointInBox)
			{
				m_pVecPoints->push_back(pt);
				return;
			}
			OutCode curCode = GetOutCode(pt);
		/*	if (curCode == INSIDE)
			{
				m_pVecPoints->push_back(pt);
			}*/
			if (m_bFirst)
			{
				m_nPrevCode = curCode;
				m_bFirst = false;
				m_prevPoint = pt;

				if (curCode == INSIDE)
				{
					m_pVecPoints->push_back(pt);
				}
				return;
			}
			if (!(m_nPrevCode | curCode))
			{
				m_pVecPoints->push_back(pt);
				m_prevPoint = pt;
			}
			else if(!(m_nPrevCode & curCode))
			{

				bool bIN = false;
				if (m_nPrevCode && !curCode)
					bIN = true;

				GPoint ptInt;
				OutCode outcodeOut = curCode ? curCode : m_nPrevCode;

				if (outcodeOut & TOP) {           // point is above the clip rectangle
					ptInt.x = m_prevPoint.x + (pt.x - m_prevPoint.x) * (m_clipBox.yMax - m_prevPoint.y) / (pt.y - m_prevPoint.y);
					ptInt.y = m_clipBox.yMax;
				}
				else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
					ptInt.x = m_prevPoint.x + (pt.x - m_prevPoint.x) * (m_clipBox.yMin - m_prevPoint.y) / (pt.y - m_prevPoint.y);
					ptInt.y = m_clipBox.yMin;
				}
				else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
					ptInt.y = m_prevPoint.y + (pt.y - m_prevPoint.y) * (m_clipBox.xMax - m_prevPoint.x) / (pt.x - m_prevPoint.x);
					ptInt.x = m_clipBox.xMax;
				}
				else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
					ptInt.y = m_prevPoint.y + (pt.y - m_prevPoint.y) * (m_clipBox.xMin - m_prevPoint.x) / (pt.x - m_prevPoint.x);
					ptInt.x = m_clipBox.xMin;
				}

				if (bIN)
				{
					m_pVecPoints->push_back(ptInt);
					m_pVecPoints->push_back(pt);
				}
				else
				{
					m_pVecPoints->push_back(ptInt);
					ClosePart();
				}
					 

			}



			m_nPrevCode = curCode;
			m_prevPoint = pt;
			/*
			while (true) {
				if (!(outcode0 | outcode1)) { // Bitwise OR is 0. Trivially accept and get out of loop
					accept = true;
					break;
				}
				else if (outcode0 & outcode1) { // Bitwise AND is not 0. (implies both end points are in the same region outside the window). Reject and get out of loop
					break;
				}
				else {
					// failed both tests, so calculate the line segment to clip
					// from an outside point to an intersection with clip edge
					double x, y;

					// At least one endpoint is outside the clip rectangle; pick it.
					OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

					// Now find the intersection point;
					// use formulas:
					//   slope = (y1 - y0) / (x1 - x0)
					//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
					//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
					if (outcodeOut & TOP) {           // point is above the clip rectangle
						x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
						y = ymax;
					}
					else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
						x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
						y = ymin;
					}
					else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
						y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
						x = xmax;
					}
					else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
						y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
						x = xmin;
					}

					// Now we move outside point to intersection point to clip
					// and get ready for next pass.
					if (outcodeOut == outcode0) {
						x0 = x;
						y0 = y;
						outcode0 = ComputeOutCode(x0, y0);
					}
					else {
						x1 = x;
						y1 = y;
						outcode1 = ComputeOutCode(x1, y1);
					}
				}
			}*/


		}
		void ClipLine::EndLine(bool bAllPointInBox)
		{
			m_bFirst = true;
			ClosePart();
		}

		ClipLine::OutCode ClipLine::GetOutCode(const GPoint& pt)
		{
			OutCode code = INSIDE;

			if (pt.x < m_clipBox.xMin)
				code |= LEFT;
			else if (pt.x > m_clipBox.xMax)
				code |= RIGHT;
			if (pt.y < m_clipBox.yMin)
				code |= BOTTOM;
			else if (pt.y > m_clipBox.yMax)
				code |= TOP;

			return code;
		}
		void ClipLine::ClosePart()
		{
			uint32 nEnd = m_pVecPoints->size();
			if (m_nBeginPos != nEnd)
			{
				m_pVecParts->push_back(nEnd - m_nBeginPos);
				m_nBeginPos = nEnd;
			}

		}
	}
}