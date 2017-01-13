#include "stdafx.h"
#include "ShapeLib/shapefil.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "importFromShape.h"
#include "../../EmbDB/SpatialRectQuery.h"
#include <vector>
#include <fstream>
#include <stack>



/*typedef embDB::TBPVector<embDB::ZOrderRect2DU16> TVecZOrder;



void FillVector(uint16 xMin, uint16 yMin, uint16 xMax, uint16 yMax, uint16 nWx, uint16 nWy, TVecZOrder& vec)
{

	for (uint16 x = xMin; x < xMax;  x += nWx)
	{
		for (uint16 y = yMin; y < yMax;  y += nWy)
		{

			vec.push_back(embDB::ZOrderRect2DU16(x, y, x + nWx, y + nWy));
		}
	}

	embDB::ZRect16Comp zComp;
	vec.quick_sort(zComp);

}*/