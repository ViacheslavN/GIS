#ifndef GIS_ENGINE_DISPLAY_SCREEN_H_
#define GIS_ENGINE_DISPLAY_SCREEN_H_
#include "Display.h"
namespace GisEngine
{
	namespace Display
	{
		class CDisplay : public IDisplay
		{
			public:
				CDisplay( IDisplayTransformation* pTD);
				~CDisplay();
				virtual void SetClipGeometry( const CommonLib::CGeoShape& clipGeom );
				virtual void SetClipRect( const GisBoundingBox& clipGeom );

				virtual void StartDrawing( IGraphics *pGraphics );
				virtual void FinishDrawing();
				virtual IGraphicsPtr GetGraphics();

				virtual IDisplayTransformationPtr GetTransformation();
				virtual void SetTransformation( IDisplayTransformation* pDisplayTransformation );

				virtual void Lock();
				virtual void UnLock();
		private:
			IGraphicsPtr m_pGraphics;
			IDisplayTransformationPtr m_pDisplayTransformation;
		};
	}
}

#endif