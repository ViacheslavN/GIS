#include "stdafx.h"
#include "MapDrawer.h"
#include "Display/RectClipper.h"
#include "Display/DisplayTransformation2D.h"
#include "Display/GraphicsAgg.h"
#include "MapTask.h"
namespace GisEngine
{
	namespace GisFramework
	{
		CMapDrawer::CMapDrawer():
			m_Timer(1000), 
				m_nWidht(0), m_nHeight(0), m_nFlags(0), m_DrawThread(this), m_bCancel(false),
				m_mapTask(this)
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
			Init();
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

			Init();

			if(bDraw && m_pMap.get())
			{
				Display::GPoint pt(-m_nWidht, -m_nHeight);
				m_pDispTran->DeviceToMap(&pt, &m_OrgPoint, 1);
				Redraw();
			}
		}
		void CMapDrawer::Init()
		{
			if(!m_pMap.get() ||!m_nWidht ||!m_nHeight )
				return;

			 Display::GRect wnd_rect(0, 0, (Display::GUnits)m_nWidht , (Display::GUnits)m_nHeight);

			if(!m_pDispTran.get())
			{
				m_pDispTran = (Display::IDisplayTransformation*)new Display::CDisplayTransformation2D(m_dDpi, m_pMap->GetMapUnits(), wnd_rect);
				m_pDispTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
				m_pDispTran->SetDeviceClipRect(wnd_rect);
				m_pDispTran->SetDeviceRect(wnd_rect);

			//	m_pDispTran->SetClipper(m_Clipper.get());


				m_pDispCalcTran = (Display::IDisplayTransformation*)new Display::CDisplayTransformation2D(m_dDpi, m_pMap->GetMapUnits(), wnd_rect);
				m_pDispCalcTran->SetSpatialReference(m_pMap->GetSpatialReference().get());
				m_pDispCalcTran->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
				m_pDispCalcTran->SetDeviceClipRect(wnd_rect);
				m_pDispCalcTran->SetDeviceRect(wnd_rect);

			//	m_pDispCalcTran->SetClipper(m_Clipper.get());
			}
			else
			{
				m_pDispTran->SetDeviceClipRect(wnd_rect);
				m_pDispTran->SetDeviceRect(wnd_rect);

				m_pDispCalcTran->SetDeviceClipRect(wnd_rect);
				m_pDispCalcTran->SetDeviceRect(wnd_rect);

			}
			m_pMapGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);
			m_pLabelGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);
			m_pOutGraphics = new Display::CGraphicsAgg(m_nWidht, m_nHeight, false);

			m_mapTask.Init(m_pMap.get(),  m_pDispTran.get(), (Cartography::eDrawPhase)((int)Cartography::DrawPhaseGeography|(int)Cartography::DrawPhaseSelection), m_pMapGraphics.get() );
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

			m_mapTask.SetDraw();

			if(pGraphics)
			{
				m_pMapGraphics->Copy(pGraphics, Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)m_nWidht, (Display::GUnits)m_nHeight), false);
				m_pOutGraphics->Copy(pGraphics, Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)m_nWidht, (Display::GUnits)m_nHeight), false);
				//pLableGraphics_->Copy(pGraphics, Display::GPoint(0, 0),  Display::GRect(0, 0, width_, height_), false);
			}
			else
			{
				//pMapGraphics_->Erase(ERASECOLOR);
				m_pOutGraphics->Erase(Display::Color(255,255,255,255));
			}
			m_mapTask.SetDrawPhase((Cartography::eDrawPhase)((int)Cartography::DrawPhaseGeography|(int)Cartography::DrawPhaseSelection));



			m_DrawThread.SetTask(&m_mapTask);
			SetFlag(DrawMap);
			m_bCancel = false;
			m_Timer.Start();
			
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
			m_mapTask.StopDraw(bWait);
   		    m_DrawThread.StopDraw(true, bWait);

			if(!IsFlag(FinsedDrawMap))
			{
			 SetFlag(0);
			}
			m_bCancel = true;
		}
		void CMapDrawer::OnFinishedDrawMapTask(CMapTask *pTask)
		{
			CommonLib::CSSection::scoped_lock lock(m_cs);
			if(pTask->GetDrawPhase() & Cartography::DrawPhaseAnnotation)
			{
				//timer_.Stop();
				//pOutGraphics_->Copy(pLableGraphics_.get(), Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)width_, (Display::GUnits)height_), false);
				AddFlags(FinsedDrawLable, DrawLable);
				OnInvalidateEvent.fire((const Display::GPoint*)0, (const Display::GRect*)0, false);
				OnFinishMapDrawingEvent.fire(m_bCancel);
				//    ShowResult();

			}
			else
			{
				m_Timer.Stop();
				AddFlags(FinsedDrawMap, DrawMap);
				/*if(IsFlag(PanAfterMap))
				{
					AddFlags(StoppingPan, PanAfterMap);
					m_pOutGraphics->Copy(pMapGraphics_.get(), Display::GPoint(0, 0),  Display::GRect(0, 0, (Display::GUnits)width_, (Display::GUnits)height_), false);
					pan_task_.SetTaskRect(pMapGraphics_.get());
					pan_task_.SetTrackCancel(true);
					DrawThread_.SetTask(&pan_task_);
					return;
				}
				if(IsFlag(PAN_STATE))
				{
					Display::GPoint pt(0, 0);
					int pan_x = pan_task_.GetPanOffsetX();
					int pan_y = pan_task_.GetPanOffsetY();
					pOutGraphics_->Erase(ERASECOLOR);
					Display::GRect rect( (Display::GUnits)-pan_x, (Display::GUnits)-pan_y, Display::GUnits(width_ - pan_x), Display::GUnits(height_- pan_y));
					pOutGraphics_->Copy(pMapGraphics_.get(), pt, rect, false);
					OnInvalidate((const Display::GPoint*)0, (const Display::GRect*)0, false);
					return;
				}
				if(IsFlag(STOPPING_PAN))
				{
					AddFlags(0, STOPPING_PAN);
					OnFinishedPan();
					return;
				}*/
				m_pOutGraphics->Copy(m_pMapGraphics.get(), Display::GPoint(0, 0),  Display::GRect(0, 0,(Display::GUnits) m_nWidht, (Display::GUnits)m_nHeight), false);
				//pan_task_.Erase();
				//    SaveGraphics(pMapGraphics_.get(), L"\\Storage Card\\MapFase.bmp");
				//StartLableDraw(true);
				OnInvalidateEvent.fire((const Display::GPoint*)0, (const Display::GRect*)0, false);
			}

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