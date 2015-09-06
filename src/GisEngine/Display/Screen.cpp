#include "stdafx.h"
#include "Screen.h"

namespace GisEngine
{
	namespace Display
	{
		CDisplay::CDisplay( IDisplayTransformation* pTD)
		{
			m_pDisplayTransformation = pTD;
		}
		CDisplay::~CDisplay()
		{

		}
		void CDisplay::SetClipGeometry( const CommonLib::CGeoShape& clipGeom )
		{

		}
		void CDisplay::SetClipRect( const GisBoundingBox& clipGeom )
		{

		}

		void CDisplay::StartDrawing( IGraphics *pGraphics )
		{
			m_pGraphics = pGraphics;
			m_pGraphics->StartDrawing();
		}
		void CDisplay::FinishDrawing()
		{
			m_pGraphics->EndDrawing();
			m_pGraphics = 0;
		}
		IGraphicsPtr CDisplay::GetGraphics()
		{
			return m_pGraphics;
		}

		IDisplayTransformationPtr CDisplay::GetTransformation()
		{
			return m_pDisplayTransformation;
		}
		void CDisplay::SetTransformation( IDisplayTransformation* pDisplayTransformation )
		{
			m_pDisplayTransformation = pDisplayTransformation;
		}

		void CDisplay::Lock()
		{
			if(m_pGraphics.get())
				m_pGraphics->Lock();
		}
		void CDisplay::UnLock()
		{
			if(m_pGraphics.get())
				m_pGraphics->UnLock();
		}
	}
}