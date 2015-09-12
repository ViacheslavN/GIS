#ifndef GIS_ENGINE_GIS_FRAEMWORK_MAP_DRAWER_H_
#define GIS_ENGINE_GIS_FRAEMWORK_MAP_DRAWER_H_
#include "GisFramework.h"
#include "CommonLibrary/Timer.h"
#include "Display/ClipRectAlloc.h"
#include "DrawThread.h"
#include "MapTask.h"
namespace GisEngine
{
	namespace GisFramework
	{
		class CMapTask;
		class CMapDrawer : public IMapDrawer
		{
			public:
				CMapDrawer();
				~CMapDrawer();
				virtual Display::IDisplayTransformationPtr GetTransformation() const;
				virtual Display::IDisplayTransformationPtr GetCalcTransformation() const;
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


				virtual void SetOnInvalidate(OnInvalidate* pFunck, bool bAdd);
				virtual void SetOnFinishMapDrawing(OnFinishMapDrawing* pFunck, bool bAdd);
				virtual void OnFinishedDrawMapTask(CMapTask *pTask);

				CommonLib::Event3<const Display::GPoint*, const Display::GRect*, bool> OnInvalidateEvent;
				CommonLib::Event1<bool> OnFinishMapDrawingEvent;

			private:
				 void Init();
				 void OnTimer(CommonLib::CTimer *pTimer);
				 void CopyTrans();

				
				 void AddFlags(int add_flag = 0, int remove_flag = 0);
				 void SetFlag(int flag);
				 bool IsFlag(int flag);

			private:
				Display::IDisplayTransformationPtr m_pDispTran;
				Display::IDisplayTransformationPtr m_pDispCalcTran;
				Display::IDisplayPtr			   m_pDisplay;
				Display::IGraphicsPtr			   m_pMapGraphics;
				Display::IGraphicsPtr			   m_pLabelGraphics;
				Display::IGraphicsPtr			   m_pOutGraphics;
				GisEngine::Display::IClipPtr m_Clipper;
				GisEngine::Display::CClipRectAlloc m_ClipAlloc;

				Cartography::IMapPtr			   m_pMap;
				CommonLib::CTimer m_Timer;
				double m_dDpi;
				uint32 m_nWidht;
				uint32 m_nHeight;
				uint32 m_nFlags;
				GisXYPoint m_OrgPoint;
				CommonLib::CSSection m_cs;

				CDrawThread m_DrawThread;
				bool m_bCancel;

				CMapTask m_mapTask;
		};
	}
}
#endif