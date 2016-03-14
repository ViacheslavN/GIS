#include "stdafx.h"
#include "../../GeoDatabase/EmbDBWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/EmbDBInsertCursor.h"
#include "../../GeoDatabase/EmbDBFeatureClass.h"
#include "CommonLibrary/MemoryStream.h"


double inline Round(double dValue, unsigned int uiScale = 0)
{
	double dSign = dValue < 0 ? -1 : 1;
	dValue = fabs(dValue);
	double dPow = pow((double)10., (int)uiScale);
	double dFloorArg = dValue * dPow + 0.5;
	double dFloor = floor(dFloorArg);

	if (fabs(fabs(dFloorArg - dFloor) - 1) <= fabs(dFloorArg) * 1e-14)
		dFloor += 1;						

	return (dFloor/dPow) * dSign;
}
bool inline IsEqual(double dVal1, double dVal2)
{
	return fabs(dVal1 - dVal2) < 0.000000001;
}
bool CompareShape(CommonLib::CGeoShape* pShp1, CommonLib::CGeoShape* pShp2, uint32 nScale = 2)
{
	if(pShp1->type() != pShp2->type())
		return false;


	if(pShp1->getPartCount() != pShp2->getPartCount())
		return false;


	for (uint32 i = 0; i < pShp1->getPartCount(); ++i)
	{
		uint32 nPart1 = pShp1->getPart(i);
		uint32 nPart2 = pShp1->getPart(i);
		if(nPart1 != nPart2)
			return false;
	}


	if(pShp1->getPointCnt() != pShp2->getPointCnt())
		return false;


	for (uint32 i = 0; i < pShp1->getPointCnt(); ++i)
	{
		CommonLib::GisXYPoint pt1 = pShp1->getPoints()[i];
		CommonLib::GisXYPoint pt2 = pShp2->getPoints()[i];
		double dX1 = Round(pt1.x, 2);
		double dY1 = Round(pt1.y, 2);


		double dX2 = Round(pt2.x, 2);
		double dY2 = Round(pt2.y, 2);

		if(!IsEqual(dX1, dX2) || !IsEqual(dY1, dY2) )
			return false;
	}

	return true;
}

void CompressShape()
{
	
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");
	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"building.shp");
	if(!pShapeFC.get())
		return;


	GisEngine::GeoDatabase::CQueryFilter filter;
	filter.AddRef();
	filter.SetOutputSpatialReference(pShapeFC->GetSpatialReference().get());
	filter.SetSpatialRel(GisEngine::GeoDatabase::srlIntersects);
	filter.SetBB(pShapeFC->GetExtent()->GetBoundingBox());
	filter.SetShapeField(pShapeFC->GetShapeFieldName());

	GisEngine::GeoDatabase::IFieldSetPtr pFieldSet = filter.GetFieldSet();
	for (size_t i = 0, sz = pShapeFC->GetFields()->GetFieldCount(); i < sz; ++i)
	{
		pFieldSet->Add(pShapeFC->GetFields()->GetField(i)->GetName());
	}

	GisEngine::GeoDatabase::ICursorPtr pCursor = pShapeFC->Search(&filter, true);
	GisEngine::GeoDatabase::IRowPtr pRow;


	CommonLib::CWriteMemoryStream writeStream;
	CommonLib::CWriteMemoryStream compressStream;
	CommonLib::CWriteMemoryStream compressStreamTmp;
	CommonLib::CGeoShape::compress_params params;
	params.m_PointType = CommonLib::dtType32;
	params.m_dScaleX = 0.001;
	params.m_dScaleY = 0.001;

	GisEngine::GisBoundingBox bbox = pShapeFC->GetExtent()->GetBoundingBox();

 	if(bbox.xMin < 0)
		params.m_dOffsetX = fabs(bbox.xMin);
	else
		params.m_dOffsetX = -1 *bbox.xMin;

	if(bbox.yMin < 0)
		params.m_dOffsetY = fabs(bbox.yMin);
	else
		params.m_dOffsetY = -1 *bbox.yMin;


	CommonLib::CGeoShape shape;
	CommonLib::FxMemoryReadStream readCompressStream;

	int ii = 0;
	while(pCursor->NextRow(&pRow))
	{

	 compressStreamTmp.seek(0, CommonLib::soFromBegin);
	  GisEngine::GeoDatabase::IFeature *pFeature = (GisEngine::GeoDatabase::IFeature*)pRow.get();
	 CommonLib::IGeoShapePtr pShape= pFeature->GetShape();
	 pShape->write(&writeStream);

	if(ii == 14399)
	 {
		 int dd =0;
		 dd++;
	 }
	 pShape->compress(&compressStreamTmp, &params);
	 compressStream.write(compressStreamTmp.buffer(), compressStreamTmp.pos());


	 readCompressStream.attachBuffer(compressStreamTmp.buffer(), compressStreamTmp.pos());

	 shape.decompress(&readCompressStream, &params);
	 ii++;
	 if(!CompareShape(pShape.get(), &shape))
	 {
		 int dd = 0;
		 dd++;
	 }

	}

	int dd = 0;
	dd++;
}