#include "stdafx.h"
#include "ClipPolygon.h"
namespace GisEngine
{

	namespace Display
	{

		//5   4   6
		//1   0   2
		//9   8   10

		ClipPolygon::ClipPolygon() 
		{
			Clear();
			//S += (points[i + 1].y + points[i].y) * (points[i + 1].x - points[i].x);
		}
		ClipPolygon::~ClipPolygon()
		{

		}

		void ClipPolygon::Init(GRect clipBox, GPoint *pOut, uint32 nMaxCnt, uint32 *pPart, uint32 nMaxPart)
		{
			m_clipBox = clipBox;
			m_pOut = pOut;
			m_nMaxCnt = nMaxCnt;
			m_pPart = pPart;
			nMaxPart = nMaxPart;

			Clear();
		}

		void ClipPolygon::Clear()
		{
			m_dSquare = 0;
			m_currState = eEmpty;
			m_blockWise = false;
			m_bNoPointIntersect = false;
			m_nParts = 0;
			m_nPoints = 0;
		}
		void ClipPolygon::AddEdge(const GPoint& pt1, const GPoint& pt2)
		{
		 
			m_dSquare += (pt2.y + pt1.y) * (pt2.x - pt1.x);


		}

		void ClipPolygon::BeginPart()
		{

		}
		
	}
}