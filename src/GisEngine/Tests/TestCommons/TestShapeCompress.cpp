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

	while(pCursor->NextRow(&pRow))
	{
	  GisEngine::GeoDatabase::IFeature *pFeature = (GisEngine::GeoDatabase::IFeature*)pRow.get();
	 CommonLib::IGeoShapePtr pShape= pFeature->GetShape();
	 pShape->getBB();


	}
}