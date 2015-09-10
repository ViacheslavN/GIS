#include "stdafx.h"
#include "MapDrawer.h"
namespace GisEngine
{
	namespace GisFramework
	{
		CMapDrawer::CMapDrawer()
		{

		}
		CMapDrawer::~CMapDrawer()
		{

		}

		Display::IDisplayTransformationPtr CMapDrawer::GetTransformation() const
		{
			return m_pDispCalcTran;
		}
		Display::IGraphicsPtr CMapDrawer::GetMapGraphics() const
		{
			return m_pMapGraphics;
		}
		Display::IGraphicsPtr CMapDrawer::GetLableGraphics() const
		{
			return m_pLabelGraphics;
		}
		Display::IGraphicsPtr CMapDrawer::GetOutGraphics() const
		{
			return m_pOutGraphics;
		}

		Cartography::IMapPtr CMapDrawer::GetMap() const
		{
			return m_pMap;
		}
		void CMapDrawer::SetMap(Cartography::IMap *pMap)
		{
			m_pMap = pMap;
		}
		void CMapDrawer::SetSize(int cx , int cy, bool bDraw)
		{

		}
		void CMapDrawer::Update( Display::IGraphics* pGraphics, Display::GPoint *pPoint, Display::GRect* pRect)
		{

		}
		void CMapDrawer::Redraw(Display::IGraphics* pGraphics)
		{

		}

		void CMapDrawer::ZoomIn(const Display::GRect& rect)
		{

		}
		void CMapDrawer::ZoomIn(const GisBoundingBox& bb)
		{

		}
		void CMapDrawer::SetScale(double scale)
		{

		}

		void CMapDrawer::StartPan(const Display::GPoint& pt)
		{

		}
		void CMapDrawer::MovePan(const Display::GPoint& pt)
		{

		}
		void CMapDrawer::StopPan(const Display::GPoint& pt)
		{

		}
		void CMapDrawer::StopDraw(bool bWait)
		{

		}
	}
}