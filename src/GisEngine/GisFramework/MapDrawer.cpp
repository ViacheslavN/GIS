#include "stdafx.h"
#include "MapDrawer.h"
#include "Display/RectClipper.h"
#include "Display/DisplayTransformation2D.h"
#include "Display/GraphicsAgg.h"
namespace GisEngine
{
	namespace GisFramework
	{
		CMapDrawer::CMapDrawer():
			m_Timer(1000), 
				m_nWidht(0), m_nHeight(0), m_nFlags(0), m_DrawThread(this)
		{
			m_Timer.OnTimer += CommonLib::Delegate(this , &CMapDrawer::OnTimer);

			m_Clipper = new GisEngine::Display::CRectClipper(&m_ClipAlloc);

#ifdef _WIN32
			HDC dc = ::GetDC(NULL);
			m_dDpi = ::GetDeviceCaps(dc, LOGPIXELSX);
			ReleaseDC(NULL ,dc);
#endif

			m_OrgPoint.x = 0;
			m_OrgPoint.y = 0;
		}


		void CMapDrawer::SetOnInvalidate(OnInvalidate* pFunck, bool bAdd)
		{
			if(bAdd)
				OnInvalidateEvent += pFunck;
			else
				OnInvalidateEvent -= pFunck;
		}
		void CMapDrawer::SetOnFinishMapDrawing(OnFinishMapDrawing* pFunck, bool bAdd)
		{
			if(bAdd)
				OnFinishMapDrawingEvent += pFunck;
			else
				OnFinishMapDrawingEvent -= pFunck;
		}
		CMapDrawer::~CMapDrawer()
		{
			m_Timer.OnTimer -= CommonLib::Delegate(this , &CMapDrawer::OnTimer);
		}

		Display::IDisplayTransformationPtr CMapDrawer::GetTransformation() const
		{
			return m_pDispTran;
		}
		Display::IDisplayTransformationPtr CMapDrawer::GetCalcTransformation() const
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
			if(m_pDispTran.get())
			{
				m_pDispTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());

				m_pDispCalcTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispCalcTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
			}
		}
		void CMapDrawer::SetSize(int cx , int cy, bool bDraw)
		{
			m_nWidht = cx;
			m_nHeight = cy;

			 Display::GRect wnd_rect(0, 0, (Display::GUnits)m_nWidht , (Display::GUnits)m_nHeight);

			if(!m_pDispTran.get() && m_pMap.get())
			{
				m_pDispTran = (Display::IDisplayTransformation*)new Display::CDisplayTransformation2D(m_dDpi, m_pMap->GetMapUnits(), wnd_rect);
				m_pDispTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
 				m_pDispTran->SetDeviceClipRect(wnd_rect);
				m_pDispTran->SetDeviceRect(wnd_rect);

				m_pDispTran->SetClipper(m_Clipper.get());


				m_pDispCalcTran = (Display::IDisplayTransformation*)new Display::CDisplayTransformation2D(m_dDpi, m_pMap->GetMapUnits(), wnd_rect);
				m_pDispCalcTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispCalcTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
				m_pDispCalcTran->SetDeviceClipRect(wnd_rect);
				m_pDispCalcTran->SetDeviceRect(wnd_rect);

				m_pDispCalcTran->SetClipper(m_Clipper.get());
			}

			m_pMapGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);
			m_pLabelGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);
			m_pOutGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);

			 

			if(bDraw && m_pMap.get())
			{

			}
		}
		void CMapDrawer::Update( Display::IGraphics* pGraphics, Display::GPoint *pPoint, Display::GRect* pRect)
		{
			if(!pGraphics)
				return;


			Display::GRect OutRect;
			Display::GPoint OutPoint;
			if(pRect)
				OutRect = *pRect;
			else 
				OutRect.set(0, 0, (Display::GUnits)m_nWidht, (Display::GUnits)m_nHeight);

			if(pPoint)
				OutPoint = *pPoint;
			else 
			{
				OutPoint.x = 0;
				OutPoint.y = 0;
			}


			if(!m_pOutGraphics.get())
			{
				pGraphics->Erase(Display::Color::Color(255, 255, 255, 255), &OutRect);
				return;
			}
			CommonLib::CSSection::scoped_lock lock(m_cs);

			if(IsFlag(PanState)||IsFlag(StoppingPan))
			{
				if(!IsFlag(FinsedDrawMap))
				{
					pGraphics->Erase(Display::Color::Color(255, 255, 255, 255));
					//TO DO Pan
					
				}
				else 
				{
					pGraphics->Copy(m_pOutGraphics.get(), OutPoint, OutRect, false);
				}
			}
			else 
			{
				if(!IsFlag(FinsedDrawMap) )
				{
					m_pOutGraphics->Copy(m_pMapGraphics.get(), Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)m_nHeight, (Display::GUnits)m_nHeight), false);
				}
				pGraphics->Copy(m_pOutGraphics.get(), OutPoint, OutRect, false);
			}
		}
		void CMapDrawer::Redraw(Display::IGraphics* pGraphics)
		{
			if(!m_pMap.get() || !m_pDispTran.get())
				return;

			StopDraw();
			CopyTrans();

			Display::GPoint pt(0, 0);
			m_pDispTran->DeviceToMap(&pt, &m_OrgPoint, 1);
			
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
			m_Timer.Stop();
		}

		void CMapDrawer::CopyTrans()
		{
			m_pDispTran->SetMapPos(m_pDispCalcTran->GetMapPos() , m_pDispCalcTran->GetScale());
			m_pDispTran->SetRotation(m_pDispCalcTran->GetRotation());
		}
		void CMapDrawer::OnTimer(CommonLib::CTimer *pTimer)
		{
			 OnInvalidateEvent.fire((const Display::GPoint*)0, (Display::GRect*)0, false);
		}

		void CMapDrawer::AddFlags(int add_flag, int remove_flag)
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			m_nFlags |= add_flag;
			m_nFlags &= ~remove_flag;
		}
		void CMapDrawer::SetFlag(int flag)
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			m_nFlags = flag;
		}
		bool CMapDrawer::IsFlag(int flag)
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			return (m_nFlags & flag) == flag;
		}
	}
}