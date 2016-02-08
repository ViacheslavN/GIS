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
int CMapDrawer::openMap(const CommonLib::CString& connectionString, int width, int height, int dpi)
{
	return 1;
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

extern "C"
jint 
Java_app_com_mapviewer_nativeApi_MapDrawer_openMapN
	(
	JNIEnv *env,
	jobject thiz,
	jint ptr,
	jstring path,
	jint width,
	jint height,
	jint dpi)
{
	CMapDrawer* pdrawer = (CMapDrawer*)ptr;
	if (!pdrawer)
	{
		return 0;
	}

	const char *string = env->GetStringUTFChars(path, 0);
	CommonLib::CString connectionString(string, strlen(string));


	int ret =  pdrawer->openMap(connectionString, width, height, dpi);
	env->ReleaseStringUTFChars(path, string);
	return ret;
}