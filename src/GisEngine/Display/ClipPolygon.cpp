#include "stdafx.h"
#include "ClipPolygon.h"
namespace GisEngine
{

	namespace Display
	{

		//5   4   6
		//1   0   2
		//9   8   10

		ClipPolygon::ClipPolygon() :
			 m_stageBottom(m_stageOut,  0/*Bottom*/)		
			, m_stageLeft(m_stageBottom, 0/*Left*/)		
			, m_stageTop(m_stageLeft, 0/*Top*/)		
			, m_stageRight(m_stageTop, 0/*Right*/)
		{
			Clear();
			//S += (points[i + 1].y + points[i].y) * (points[i + 1].x - points[i].x);
		}
		ClipPolygon::~ClipPolygon()
		{

		}

		void ClipPolygon::Init(GRect clipBox, TVecPoints *pDest)
		{

			Clear();

			m_clipBox = clipBox;
			 
			m_stageBottom.SetPoint(m_clipBox.yMax);
			m_stageTop.SetPoint(m_clipBox.yMin);
			m_stageLeft.SetPoint(m_clipBox.xMin);
			m_stageRight.SetPoint(m_clipBox.xMax);
 
			m_stageOut.SetDestination(pDest);
		}

		void ClipPolygon::Clear()
		{
			m_pOut = nullptr;
			m_nMaxCnt = 0;
			m_pPart = nullptr;
			m_nMaxPart = 0;

			m_stageOut.SetDestination(nullptr);
		}
	 
		void ClipPolygon::AddVertex(const GPoint& pt)
		{
			m_stageRight.HandleVertex(pt);
		}
		void ClipPolygon::EndPolygon()
		{
			m_stageRight.Finalize();
 
		}
	}
}