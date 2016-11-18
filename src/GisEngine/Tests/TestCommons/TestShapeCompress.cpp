#include "stdafx.h"
#include "../../GeoDatabase/EmbDBWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/EmbDBInsertCursor.h"
#include "../../GeoDatabase/EmbDBFeatureClass.h"
#include "CommonLibrary/MemoryStream.h"
#include "CommonLibrary/BitStream.h"
#include "CommonLibrary/WriteBitStream.h"
#include "CommonLibrary/compressutils.h"
#include "CommonLibrary/FixedBitStream.h"

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
	return fabs(dVal1 - dVal2) < 0.01;
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
		/*double dX1 = Round(pt1.x, 6);
		double dY1 = Round(pt1.y, 6);


		double dX2 = Round(pt2.x, 6);
		double dY2 = Round(pt2.y, 6);*/


		double dX1 = pt1.x;
		double dY1 = pt1.y;


		double dX2 = pt2.x;
		double dY2 = pt1.y;


		dX1 = Round(pt1.x, 6);
		dY1 = Round(pt1.y, 6);


		dX2 = Round(pt2.x, 6);
		dY2 = Round(pt2.y, 6);

		if(!IsEqual(dX1, dX2) || !IsEqual(dY1, dY2) )
			return false;
	}

	return true;
}

uint32 getScale(double dVal, uint32 nMaxScale = 15)
{
	uint64 nPow = 1;
	dVal = fabs(dVal);
	for (uint32 i = 0; i < nMaxScale; ++i)
	{
		uint64 nVal = uint64(dVal * nPow);
		double dBack = (double)nVal/nPow;
		if(fabs(dVal - dBack) == 0.)
			return i;
		nPow *= 10;
	}
	return nMaxScale;
}

void getMaxShapeScale(uint32 &x, uint32& y, CommonLib::CGeoShape* pShp)
{
	x = 0;
	y = 0;

	for (uint32 i = 0; i < pShp->getPointCnt(); ++i)
	{
		CommonLib::GisXYPoint& pt = pShp->getPoints()[i];
		uint32 nCalcX = getScale(pt.x);
		uint32 nCalcY = getScale(pt.y);

		if(nCalcX > x)
			x = nCalcX;
		if(nCalcY > y)
			y = nCalcY;
	}

}
std::string polylineEncode(CommonLib::GisXYPoint* pPoint, int32 nPointCnt, double dOffsetX, double dOffsetY, double dScale);
void CompressShape()
{
	
	CommonLib::WriteBitStream bits;
	CommonLib::FxBitReadStream bitStream;

	bits.resize(10);


	uint64 val = 6330;

	uint32 nLen =  CommonLib::TFindMostSigBit::FMSB(val);


	((CommonLib::IBitWriteStream*)(&bits))->writeBits(val, nLen - 1);


	bitStream.attachBuffer(bits.buffer(), bits.size());

	uint64 val1 = 0;
	bitStream.readBits(val1, nLen - 1);


	val1 |= ((uint64)1 << nLen - 1);

	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\db\\10m_cultural\\");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\db\\");

//	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"building.shp");
	//GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"ne_10m_roads_north_america.shp");
	GisEngine::GeoDatabase::IFeatureClassPtr pShapeFC = pShapeWks->OpenFeatureClass(L"ne_10m_urban_areas_landscan.shp");
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
	params.m_nScaleX = 7;
	params.m_nScaleY = 7;



	double dScaleX = 1/pow(10., params.m_nScaleX);
	double dScaleY = 1/pow(10., params.m_nScaleY);

	/*params.m_PointType = CommonLib::dtType32;
	params.m_dScaleX = 0.001;
	params.m_dScaleY = 0.001;*/

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

	int nError = 0;

	int nSize = 0;
	while(pCursor->NextRow(&pRow))
	{


	 compressStreamTmp.seek(0, CommonLib::soFromBegin);
	  GisEngine::GeoDatabase::IFeature *pFeature = (GisEngine::GeoDatabase::IFeature*)pRow.get();
	 CommonLib::IGeoShapePtr pShape= pFeature->GetShape();
	 pShape->write(&writeStream);

	 uint32 scaleX, scaleY;
	 getMaxShapeScale(scaleX, scaleY, pShape.get());
	if(ii == 40)
	 {

		// std::string sStr = polylineEncode(pShape->getPoints(), pShape->getPointCnt(), params.m_dOffsetX, params.m_dOffsetY, params.m_dScaleX);
		 int dd =0;
		 dd++;
	 }
	 pShape->compress(&compressStreamTmp, &params);
	 compressStream.write(compressStreamTmp.buffer(), compressStreamTmp.pos());

	 if(nSize < compressStreamTmp.pos())
		 nSize = compressStreamTmp.pos();

	 if(nSize == 18955)
	 {
		 int dd = 0;
		 dd++;
	 }
	 readCompressStream.attachBuffer(compressStreamTmp.buffer(), compressStreamTmp.pos());

	 shape.decompress(&readCompressStream, &params);
	
	 if(!CompareShape(pShape.get(), &shape))
	 {
		 nError++;



		 for (uint32 i = 0; i < pShape->getPointCnt(); ++i)
		 {

			 GisXYPoint pt = pShape->getPoints()[i];

			 int32 X = (int32)((pt.x + params.m_dOffsetX)/dScaleX);
			 int32 Y = (int32)((pt.y + params.m_dOffsetY)/dScaleY);


			 double dX = ((double)X *dScaleX) - params.m_dOffsetX;  
			 double dY = ((double)Y *dScaleY) - params.m_dOffsetY;

			 if(!IsEqual(dX, pt.x) || !IsEqual(dY, pt.y) )
			 {
				 int dd = 0;
				 dd++;
			 }


		 }


	 }
	  ii++;
	}

	int dd = 0;
	dd++;
}