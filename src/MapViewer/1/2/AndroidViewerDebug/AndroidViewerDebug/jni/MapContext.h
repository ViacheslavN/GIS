#ifndef _ANDROID_MAP_VIEWER_MAP_CONTEXT_
#define _ANDROID_MAP_VIEWER_MAP_CONTEXT_

#include "Cartography/Cartography.h"
#include "Display/Display.h"

class CMapContext
{
public:
	CMapContext();
	~CMapContext();
private:
	GisEngine::Cartography::IMapPtr m_pMap;
	GisEngine::Display::IGraphicsPtr m_pGraphics;
	GisEngine::Display::IDisplayTransformationPtr	m_pDisplayTransformation;
	GisEngine::Display::IDisplayPtr m_pDisplay
};

#endif