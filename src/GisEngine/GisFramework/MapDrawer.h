#ifndef GIS_ENGINE_GIS_MAP_DRAWER_H_
#define GIS_ENGINE_GIS_MAP_DRAWER_H_
#include "GisFramework.h"

namespace GisEngine
{
	namespace GisFramework
	{
		class CMapDrawer : public IMapDrawer
		{
			public:
				CMapDrawer();
				~CMapDrawer();
				virtual Display::IDisplayTransformationPtr GetTransformation() const;
				virtual Display::IGraphicsPtr GetMapGraphics() const;
				virtual Display::IGraphicsPtr GetLableGraphics() const;
				virtual Display::IGraphicsPtr GetOutGraphics() const;

				virtual Cartography::IMapPtr GetMap() const;
				virtual void SetMap(Cartography::IMap *pMap);
				virtual void SetSize(int cx , int cy, bool bDraw = true);
				virtual void Update( Display::IGraphics* pGraphics, Display::GPoint *pPoint, Display::GRect* pRect);
				virtual void Redraw(Display::IGraphics* pGraphics = 0);

				virtual void ZoomIn(const Display::GRect& rect);
				virtual void ZoomIn(const GisBoundingBox& bb);
				virtual void SetScale(double scale);

				virtual void StartPan(const Display::GPoint& pt);
				virtual void MovePan(const Display::GPoint& pt);
				virtual void StopPan(const Display::GPoint& pt);
				virtual void StopDraw(bool bWait = true);
			private:
				Display::IDisplayTransformationPtr m_pDispTran;
				Display::IDisplayTransformationPtr m_pDispCalcTran;
				Display::IDisplayPtr			   m_pDisplay;
				Display::IGraphicsPtr			   m_pMapGraphics;
				Display::IGraphicsPtr			   m_pLabelGraphics;
				Display::IGraphicsPtr			   m_pOutGraphics;
				Cartography::IMapPtr			   m_pMap;
		};
	}
}
#endif