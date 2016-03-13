#include "stdafx.h"
#include "../../GeoDatabase/EmbDBWorkspace.h"
#include "../../GeoDatabase/ShapefileWorkspace.h"
#include "../../GeoDatabase/QueryFilter.h"
#include "../../GeoDatabase/EmbDBInsertCursor.h"
#include "../../GeoDatabase/EmbDBFeatureClass.h"
#include "CommonLibrary/MemoryStream.h"

void CompressShape()
{
	
	GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"d:\\work\\MyProject\\GIS\\src\\GisEngine\\Tests\\TestData");
	//GisEngine::GeoDatabase::IWorkspacePtr pShapeWks  = GisEngine::GeoDatabase::CShapefileWorkspace::Open(L"ShapeTest", L"D:\\test\\GIS\\GIS\\src\\GisEngine\\Tests\\TestData");
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

	if(ii == 14884)
	 {
		 int dd =0;
		 dd++;
	 }
	 pShape->compress(&compressStreamTmp, &params);
	 compressStream.write(compressStreamTmp.buffer(), compressStreamTmp.pos());


	 readCompressStream.attachBuffer(compressStreamTmp.buffer(), compressStreamTmp.pos());

	 shape.decompress(&readCompressStream, &params);
	 ii++;
	 if(*pShape.get() != shape)
	 {
		 int dd = 0;
		 dd++;
	 }

	}

	int dd = 0;
	dd++;
}