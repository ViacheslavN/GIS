#include <jni.h>
#include "MapDrawer.h"
#include "Cartography/Map.h"
#include "CommonLibrary/File.h"

#include "GeoDatabase/ShapefileWorkspace.h"
#include "GeoDatabase/SQLiteWorkspace.h"
#include "Cartography/FeatureLayer.h"
#include "Cartography/SimpleSymbolAssigner.h"
#include "Cartography/FeatureRenderer.h"
#include "Display/SimpleLineSymbol.h"
#include "Display/SimpleFillSymbol.h"
#include "Display/Screen.h"
#include "GeoDatabase/ShapefileFeatureClass.h"
#include "GeoDatabase/EmbDBWorkspace.h"
CMapDrawer::CMapDrawer(double dDpi) : m_nWidht(0), m_nHeight(0), m_dDpi(dDpi)
{
	m_Clipper = new GisEngine::Display::CRectClipper(&m_ClipAlloc);
	m_OrgPoint.x = 0;
	m_OrgPoint.y = 0;
	
	m_pMap = new GisEngine::Cartography::CMap();
	m_pMap->SetVerticalFlip(true);
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
	
	CommonLib::CString sExt = CommonLib::FileSystem::FindFileExtension(connectionString);
	
	if (sExt == L"shp")
	{
		return openShape(connectionString, width, height, dpi);
	}
	else if (sExt == L"embdb")
	{
		return openEmbDB(connectionString, width, height, dpi);
	}
	else if (sExt == L"sqlite")
	{
		return openSQLite(connectionString, width, height, dpi);
	}
	return 1;
}
int CMapDrawer::openShape(const CommonLib::CString& connectionString, int width, int height, int dpi)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(connectionString);
	//	CommonLib::CString sFileName = CommonLib::FileSystem::FindOnlyFileName(pszFile);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(connectionString);

	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"", sPath.cwstr());
	
	if (!pWks.get())
		return 0;

	GisEngine::GeoDatabase::IFeatureClassPtr pFC = pWks->GetFeatureClass(sFileName);
	if (!pFC.get())
	{
		pFC = pWks->OpenFeatureClass(sFileName);
	}


	if (!pFC.get())
		return 0;

	GisEngine::GeoDatabase::CShapefileFeatureClass *pShapeFC = (GisEngine::GeoDatabase::CShapefileFeatureClass*)pFC.get();
	/*if (!pShapeFC->GetShapeIndex().get())
	{
		int nRet = ::MessageBox(m_hWnd, L"File not found with a spatial index. Create it?", L"Info", MB_YESNO);
		if (nRet == IDYES)
		{
			pShapeFC->CreateShapeIndex();
		}
	}*/

	AddFeatureClass(pFC.get());
	return 1;
}
int CMapDrawer::openSQLite(const CommonLib::CString& connectionString, int width, int height, int dpi)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(connectionString);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(connectionString);

	
	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CSQLiteWorkspace::Open(sFileName.cwstr(), sPath.cwstr(), true, true);

	if (!pWks.get())
		return 0;

	for (size_t i = 0, sz = pWks->GetDatasetCount(); i < sz; ++i)
	{
		GisEngine::GeoDatabase::IDatasetPtr pDataset = pWks->GetDataset(i);
		if (pDataset->GetDatasetType() ==  GisEngine::GeoDatabase::dtFeatureClass)
		{
			AddFeatureClass((GisEngine::GeoDatabase::IFeatureClass*)pDataset.get());
		}
	}
}
int CMapDrawer::openEmbDB(const CommonLib::CString& connectionString, int width, int height, int dpi)
{
	CommonLib::CString sPath = CommonLib::FileSystem::FindFilePath(connectionString);
	CommonLib::CString sFileName = CommonLib::FileSystem::FindFileName(connectionString);


	GisEngine::GeoDatabase::IWorkspacePtr pWks = GisEngine::GeoDatabase::CEmbDBWorkspace::Open(sFileName.cwstr(), sPath.cwstr(), true, true);

	if (!pWks.get())
		return 0;

	for (size_t i = 0, sz = pWks->GetDatasetCount(); i < sz; ++i)
	{
		GisEngine::GeoDatabase::IDatasetPtr pDataset = pWks->GetDataset(i);
		if (pDataset->GetDatasetType() ==  GisEngine::GeoDatabase::dtFeatureClass)
		{
			AddFeatureClass((GisEngine::GeoDatabase::IFeatureClass*)pDataset.get());
		}
	}
	return 1;
}
void CMapDrawer::AddFeatureClass(GisEngine::GeoDatabase::IFeatureClass *pFC)
{
	if (!pFC)
		return;

	GisEngine::Cartography::IFeatureLayerPtr pFeatureLayer(new GisEngine::Cartography::CFeatureLayer());
	CommonLib::eShapeType type = pFC->GetGeometryType();


	GisEngine::Cartography::CSimpleSymbolAssigner* pSimpleSymbolAssigner = new GisEngine::Cartography::CSimpleSymbolAssigner();
	GisEngine::Display::ISymbolPtr pSymbol;
	GisEngine::GisGeometry::ISpatialReferencePtr pSpRef = pFC->GetSpatialReference();

	switch (type)
	{
	case CommonLib::shape_type_point:
		pSymbol = new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 255, 255), 0.5);
		break;
	case CommonLib::shape_type_polyline:
		pSymbol = new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 255, 255), 0.5);
		break;
	case CommonLib::shape_type_polygon:
	case CommonLib::shape_type_multipatch:
		{
			GisEngine::Display::CSimpleFillSymbol* pFillSymbol = new GisEngine::Display::CSimpleFillSymbol();
			GisEngine::Display::CSimpleLineSymbol* pLineSymbol = new GisEngine::Display::CSimpleLineSymbol(GisEngine::Display::Color(0, 255, 0, 255), 0.5);
			pLineSymbol->SetScaleDependent(true);

			pFillSymbol->SetColor(GisEngine::Display::Color(255, 0, 0, 255));
			pFillSymbol->SetBackgroundColor(GisEngine::Display::Color(255, 0, 0, 255));
			pFillSymbol->SetOutlineSymbol(pLineSymbol);
			pSymbol = pFillSymbol;
		}
		break;
	}

	pSimpleSymbolAssigner->SetSymbol(pSymbol.get());
	GisEngine::Cartography::IFeatureRendererPtr pRenderer((GisEngine::Cartography::IFeatureRenderer*)(new GisEngine::Cartography::CFeatureRenderer()));

	pRenderer->SetSymbolAssigner(pSimpleSymbolAssigner);
	pFeatureLayer->AddRenderer(pRenderer.get());
	pFeatureLayer->SetFeatureClass(pFC);
	pFeatureLayer->SetVisible(true);


	//m_pMapDrawer->StopDraw();
	m_pMap->GetLayers()->AddLayer(pFeatureLayer.get());

	if (m_pMap->GetLayers()->GetLayerCount() == 1)
	{
		m_pMap->SetSpatialReference(pFC->GetSpatialReference().get());
		//m_pDisplayTransformation->SetSpatialReference(pFC->GetSpatialReference().get());
		//m_pDisplayTransformation->SetUnits(m_pMap->GetMapUnits());
		//m_pMapDrawer->SetMap(m_pMap.get());

	}
	//m_pDisplayTransformation->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
//	m_pMapDrawer->GetCalcTransformation()->SetMapVisibleRect(m_pMap->GetFullExtent()->GetBoundingBox());
//	m_pMapDrawer->Redraw();
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