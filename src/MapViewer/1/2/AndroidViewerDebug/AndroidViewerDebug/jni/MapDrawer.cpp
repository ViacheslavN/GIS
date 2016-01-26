#include <jni.h>
#include "MapDrawer.h"

CMapDrawer::CMapDrawer(double dDpi) : m_nWidht(0), m_nHeight(0), m_dDpi(dDpi)
{
	m_Clipper = new GisEngine::Display::CRectClipper(&m_ClipAlloc);
	m_OrgPoint.x = 0;
	m_OrgPoint.y = 0;
}
CMapDrawer::~CMapDrawer()
{

}
void CMapDrawer::SetMap(GisEngine::Cartography::IMap *pMap)
{

}
void CMapDrawer::SetSize(int cx , int cy, bool bDraw)
{
	m_nWidht = cx;
	m_nHeight = cy;

	Init();

	if(bDraw && m_pMap.get())
	{
		GisEngine::Display::GPoint pt(-m_nWidht, -m_nHeight);
		m_pDispCalcTran->DeviceToMap(&pt, &m_OrgPoint, 1);
	 
	}
}
void CMapDrawer::Init()
{
}
 //app.com.mapviewer.native_api

extern "C"
jint 
Java_app_com_mapviewer_nativeApi_MapDrawer_createMapDrawerN
	(
	JNIEnv *env,
	jobject thiz,
	jint nDpi
	)
{
	return int(new CMapDrawer(double(nDpi)));
}