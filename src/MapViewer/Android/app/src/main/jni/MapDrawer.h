#ifndef _ANDROID_MAP_MAP_DRAWER_
#define _ANDROID_MAP_MAP_DRAWER_
#include "Cartography/Cartography.h"
#include "Display/Display.h"
#include "Display/RectClipper.h"
#include "Display/ClipRectAlloc.h"
#include "Common/Common.h"


class CMapDrawer
{
public:
	CMapDrawer(double dDpi);
	~CMapDrawer();

 void SetMap(GisEngine::Cartography::IMap *pMap);
 void SetSize(int cx , int cy, bool bDraw = true);
private:

	void Init();

	GisEngine::Cartography::IMapPtr m_pMap;
	GisEngine::Display::IGraphicsPtr			   m_pMapGraphics;
	GisEngine::Display::IGraphicsPtr			   m_pLabelGraphics;
	GisEngine::Display::IGraphicsPtr			   m_pOutGraphics;
	GisEngine::Display::IDisplayTransformationPtr m_pDispTran;
	GisEngine::Display::IDisplayTransformationPtr m_pDispCalcTran;
	GisEngine::Display::IDisplayPtr m_pDisplay;
	GisEngine::Display::IClipPtr m_Clipper;
	GisEngine::Display::CClipRectAlloc m_ClipAlloc;

	double m_dDpi;
	uint32 m_nWidht;
	uint32 m_nHeight;
	GisXYPoint m_OrgPoint;
};

#endif