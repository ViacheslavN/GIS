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
			, m_stageRight(m_stageTop, 0/*Right*/), m_bFirst(true), m_dS(0.)

		{
			//S += (points[i + 1].y + points[i].y) * (points[i + 1].x - points[i].x);
		}
		ClipPolygon::~ClipPolygon()
		{

		}

		void ClipPolygon::SetPointDst( TVecPoints *pDest)
		{
			m_stageOut.SetDestination(pDest);
		}

		void ClipPolygon::SetClipBox(const GRect& clipBox)
		{	 

			m_stageBottom.SetPoint(clipBox.yMax);
			m_stageTop.SetPoint(clipBox.yMin);
			m_stageLeft.SetPoint(clipBox.xMin);
			m_stageRight.SetPoint(clipBox.xMax);
		}

		void ClipPolygon::Clear()
		{
			m_stageOut.SetDestination(nullptr);
		}
	 
		void ClipPolygon::AddVertex(const GPoint& pt, bool bAllPointInBox)
		{
			if (bAllPointInBox)
			{
				m_stageOut.AddVertex(pt);
				return;
			}

			m_stageRight.HandleVertex(pt);
			if (m_bFirst)
			{
				m_bFirst = false;
			}
			else
			{
				m_dS += (pt.y + m_nPrev.y) * (pt.x - m_nPrev.x);
			}
			m_nPrev = pt;
		}
		void ClipPolygon::BeginPolygon()
		{
			m_bFirst = true;
			m_dS = 0.;

			m_stageRight.BeginPolygon();
		}
		void ClipPolygon::EndPolygon(bool bAllPointInBox)
		{
			if (bAllPointInBox)
				return;

			m_stageRight.Finalize(); 
			m_stageOut.CheckOut(m_dS >= 0);

			

		
		}
	}
}