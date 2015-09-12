#ifndef GIS_ENGINE_GIS_FRAEMWORK_MAP_TASK_
#define GIS_ENGINE_GIS_FRAEMWORK_MAP_TASK_
#include "DrawTask.h"
#include "Cartography/Cartography.h"
#include "Display/Display.h"
#include "Common/TrackCancel.h"
namespace GisEngine
{
	namespace GisFramework
	{

		class CMapDrawer;
		class CMapTask : public IDrawTask
		{
		public:

			CMapTask(CMapDrawer* pDrawer);
			~CMapTask();

			void Init(Cartography::IMap* pMap, Display::IDisplayTransformation* pTrans, Cartography::eDrawPhase, Display::IGraphics *pGraphics);
			void SetDraw();

			virtual void Draw();
			virtual void SetTrackCancel(bool bSet);
			virtual void StopDraw(bool bWait = true);
			virtual bool IsDrawing() const;

			Cartography::eDrawPhase GetDrawPhase() const;
			void SetDrawPhase(Cartography::eDrawPhase phase);


		private:
			CMapDrawer *pDrawer_;
			Cartography::IMapPtr pMap_;
			Display::IGraphicsPtr pGraphics_;
			Display::IDisplayTransformationPtr pTransformation_;
			Cartography::eDrawPhase draw_phase_; 
			Display::IDisplayPtr screen_;
			GisCommon::CTrackCancel trackCancel_;
			bool bDrawing_;
		};
	}
}

#endif