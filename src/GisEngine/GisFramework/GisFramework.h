#ifndef GIS_ENGINE_GIS_FRAEMWORK_H_
#define GIS_ENGINE_GIS_FRAEMWORK_H_

#include <set>
#include "Common/Common.h"
#include "CommonLibrary/IRefCnt.h"
#include "Display/Display.h"
#include "Cartography/Cartography.h"

namespace GisEngine
{
	namespace GisFramework
	{

		struct IMapDrawer;

		COMMON_LIB_REFPTR_TYPEDEF(IMapDrawer);
		
		enum eMapDrawerFlags
		{

			DrawMap = 1,
			DrawLable = 2,
			PanState      = 4,
			StoppingPan= 8,
			FinishedPan= 16,
			PanAfterMap = 32,
			FinsedDrawMap  = 64,
			FinsedDrawLable  = 128
		};

		typedef CommonLib::delegate3_t<const Display::GPoint*, const Display::GRect*, bool> OnInvalidate;
		typedef CommonLib::delegate1_t<bool> OnFinishMapDrawing;

		struct IMapDrawer : public CommonLib::AutoRefCounter
		{
			public:
				IMapDrawer(){}
				virtual ~IMapDrawer(){}

				virtual Display::IDisplayTransformationPtr GetTransformation() const = 0;
				virtual Display::IDisplayTransformationPtr GetCalcTransformation() const  = 0;
				virtual Display::IGraphicsPtr GetMapGraphics() const = 0;
				virtual Display::IGraphicsPtr GetLableGraphics() const = 0;
				virtual Display::IGraphicsPtr GetOutGraphics() const = 0;

				virtual Cartography::IMapPtr GetMap() const = 0;
				virtual void SetMap(Cartography::IMap *pMap) = 0;
 

				virtual void SetSize(int cx , int cy, bool bDraw = true) = 0;
				virtual void Update( Display::IGraphics* pGraphics, Display::GPoint *pPoint, Display::GRect* pRect)= 0;
				virtual void Redraw(Display::IGraphics* pGraphics = 0)= 0;

				virtual void ZoomIn(const Display::GRect& rect)= 0;
				virtual void ZoomIn(const GisBoundingBox& bb)= 0;
				virtual void SetScale(double scale)= 0;

				virtual void StartPan(const Display::GPoint& pt)= 0;
				virtual void MovePan(const Display::GPoint& pt)= 0;
				virtual void StopPan(const Display::GPoint& pt)= 0;
				virtual void StopDraw(bool bWait = true)= 0;


				virtual void SetOnInvalidate(OnInvalidate* pFunck, bool bAdd) = 0;
				virtual void SetOnFinishMapDrawing(OnFinishMapDrawing* pFunck, bool bAdd) = 0;

		};
	}
}

#endif