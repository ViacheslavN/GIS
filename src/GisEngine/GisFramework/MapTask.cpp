#include "stdafx.h"
#include "MapTask.h"
#include "Display/Screen.h"
#include "MapDrawer.h"
namespace GisEngine
{
	namespace GisFramework
	{
		CMapTask::CMapTask(CMapDrawer* pDrawer) :
			pDrawer_(pDrawer)
			,bDrawing_(false)
		{
			assert(pDrawer_);
			screen_ = (Display::IDisplay*)new  Display::CDisplay();
		}
		CMapTask::~CMapTask()
		{
			
		}

		void CMapTask::Init(Cartography::IMap* pMap, Display::IDisplayTransformation* pTrans, Cartography::eDrawPhase phase, Display::IGraphics *pGraphics)
		{
			pMap_ = pMap;
			pTransformation_ = pTrans;
			draw_phase_ = phase;
			pGraphics_ = pGraphics;
		}
		void CMapTask::SetDraw()
		{
			 trackCancel_.Reset();
		}

		void CMapTask::Draw()
		{
			if(!pMap_.get()||!pTransformation_.get()||!pGraphics_.get())
			{
				return;
			}

			bDrawing_ = true;
			if(draw_phase_ & Cartography::DrawPhaseGeography )
			{
				pGraphics_->Erase(Display::Color(255,255,255,255));
			}

			screen_->SetTransformation(pTransformation_.get());
			screen_->StartDrawing(pGraphics_.get());
			pMap_->PartialDraw(draw_phase_, screen_.get(), &trackCancel_);
			screen_->FinishDrawing();
			bDrawing_ = false;
			if(trackCancel_.Continue())
				pDrawer_->OnFinishedDrawMapTask(this);
		}
		void CMapTask::SetTrackCancel(bool bSet)
		{
			  bSet ? trackCancel_.Reset(): trackCancel_.Cancel();
		}
		void CMapTask::StopDraw(bool bWait)
		{
			  trackCancel_.Cancel();
		}
		bool CMapTask::IsDrawing() const
		{
			 return bDrawing_;
		}

		Cartography::eDrawPhase CMapTask::GetDrawPhase() const
		{
			  return draw_phase_;
		}
		void CMapTask::SetDrawPhase(Cartography::eDrawPhase phase)
		{
			draw_phase_ = phase;
		}
	}
}