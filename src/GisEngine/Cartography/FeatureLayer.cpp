#include "stdafx.h"
#include "FeatureLayer.h"
#include "GeoDatabase/QueryFilter.h"
#include "Display/DisplayUtils.h"

namespace GisEngine
{
	namespace Cartography
	{
		CFeatureLayer::CFeatureLayer()
		{
			m_nLayerSymbolID = FeatureLayerID;
		}
		CFeatureLayer::~CFeatureLayer()
		{

		}



		void CFeatureLayer::DrawEx(eDrawPhase phase, Display::IDisplay* pDisplay, GisCommon::ITrackCancel* trackCancel)
		{

			if(!IsValid())
				return;

			double oldRefScale = pDisplay->GetTransformation()->GetReferenceScale();
			if(m_hasReferenceScale)
				pDisplay->GetTransformation()->SetReferenceScale(m_hasReferenceScale);

			if((phase & DrawPhaseGeography))
			{

				GisBoundingBox bb = pDisplay->GetTransformation()->GetFittedBounds();
				Display::GRect oldClipRect = pDisplay->GetTransformation()->GetDeviceClipRect();
			}
		}


		void CFeatureLayer::CalcBB(Display::IDisplay* pDisplay, GisBoundingBox& bb)
		{
			Display::GRect wndRC = pDisplay->GetTransformation()->GetDeviceRect();

			Display::GUnits dx = Display::SymbolSizeToDeviceSize(pDisplay->GetTransformation().get(), m_dDrawingWidth, m_bDrawingWidthScaleDependent);
			if(dx < 2)
				dx = 2;

			double map_x = pDisplay->GetTransformation()->DeviceToMapMeasure(dx);

			bb.xMin -= map_x;
			bb.xMax += map_x;

			bb.yMin -= map_x;
			bb.yMax += map_x;

			wndRC.inflate(dx, dx);
			pDisplay->GetTransformation()->SetDeviceClipRect(wndRC);
		}
	}
}
