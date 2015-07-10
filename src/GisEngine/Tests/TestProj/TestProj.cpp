// TestProj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../GisGeometry/SpatialReferenceProj4.h"
#define RAD_TO_DEG	57.29577951308232
#define DEG_TO_RAD	.0174532925199432958

int _tmain(int argc, _TCHAR* argv[])
{
	GisXYPoint pt;
	pt.dX=37.61777;
	pt.dY=55.751667;

 


	GisEngine::Geometry::CSpatialReferenceProj4 spProj4Latlong( CommonLib::str_t("+proj=longlat +ellps=WGS84"));
	GisEngine::Geometry::CSpatialReferenceProj4 spProj4SpherMer( CommonLib::str_t("+proj=merc +lon_0=0 +k=1 +x_0=0 +y_0=0 +a=6378137 +b=6378137 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs"));
	
	spProj4Latlong.Project(&spProj4SpherMer, &pt);
	spProj4SpherMer.Project(&spProj4Latlong, &pt);
	return 0;
}

