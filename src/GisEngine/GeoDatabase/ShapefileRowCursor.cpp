#include "stdafx.h"
#include "ShapefileRowCursor.h"


namespace GisEngine
{
	namespace GeoDatabase
	{
ShapefileRowCursor::ShapefileRowCursor(IQueryFilter* filter, bool recycling, IFeatureClass* fclass)
	: BaseClass(filter, recycling, gis_interface_cast<ITable>(fclass), true)
	, shp_(&static_cast<ShapefileFeatureClass*>(fclass)->shp_)
	, dbf_(&static_cast<ShapefileFeatureClass*>(fclass)->dbf_)
	, shpIndex_(&static_cast<ShapefileFeatureClass*>(fclass)->shpIndex_)
	, cacheObject_(NULL)
	, cacheGeometry_(NULL)
	, parentFC_(static_cast<ShapefileFeatureClass*>(fclass))
	, invalidCursor_(false)
{
	try
	{
		parentFC_->InitDataset();
	}
	catch(...)
	{
		invalidCursor_ = true;
	}

	if(!invalidCursor_)
	{
		currentRowID_ = oids_.size() ? *rowIDIt_ : 0;
		shapelib::SHPGetInfo(shp_->file, &recordCount_, NULL, NULL, NULL);

		gisGeoDatabase::ISpatialFilter* spatFilter = gis_interface_cast<gisGeoDatabase::ISpatialFilter>(filter);
		gisGeoDatabase::gisSpatialRel spatrel = gisGeoDatabase::gisSpatialRelUndefined;
		if(spatFilter)
			spatrel = spatFilter->GetSpatialRel();

		if(shpIndex_->opened() && spatrel != gisGeoDatabase::gisSpatialRelUndefined)
		{
			//gisGeometry::IShapePtr shape = spatFilter->GetShape();
			extent_ = extentSource_->GetBoundingBox();//shape->GetBoundingBox();
			matches_.list = new CShpList;
			if(!shpIndex_->file->Search(ERectangle(extent_.xMin, extent_.yMin, extent_.xMax, extent_.yMax), matches_.list))
				matches_.clear();      
			else
			{
				matches_.list->sort();
				matches_.list->reset();
				matches_.list->next();
			}
		}
	}
}

ShapefileRowCursor::~ShapefileRowCursor()
{
	if(cacheObject_)
		shapelib::SHPDestroyObject(cacheObject_);

	if(!invalidCursor_)
		parentFC_->FiniDataset();
}

bool ShapefileRowCursor::NextRowEx(IRowPtr* row, IRow* rowCache)
{
	if(invalidCursor_)
		return false;

	bool recordGood = false;

	while(!recordGood)
	{
		for(;;)
		{
			if(EOC())
			{
				currentRow_.reset();
				row->reset();
				return false;
			}

			if(!matches_.list)
				break;

			if((matches_.list->current() - 1) == currentRowID_)
				break;

			if((matches_.list->current() - 1) > currentRowID_)
				SimpleNext();
			else
				matches_.list->next();
		}

		if(rowCache || !currentRow_ || !recycling_)
		{
			if(rowCache)
				currentRow_ = rowCache;
			else
				currentRow_ = CFeature::CreateInstance(filter_->GetFieldSet().get(), sourceFields_.get());
			if(shapeFieldIndex_ >= 0 && IsFieldSelected(shapeFieldIndex_))
			{
				IFeature* feature = gis_interface_cast<IFeature>(currentRow_.get());
				if(feature)
				{
					cacheShape_ = Shape::CreateInstance();//feature->GetShape();
					IShapeEdit* shapeEdit = gis_interface_cast<IShapeEdit>(cacheShape_.get());
					IGeometryPtr geom = Geometry::CreateInstance();
					IGeometryEdit* geomEdit = gis_interface_cast<IGeometryEdit>(geom.get());
					cacheGeometry_ = &geomEdit->Get();
					shapeEdit->AddGeometry(geom.get());
					feature->SetShape(cacheShape_.get());
				}
			}
		}

		recordGood = FillRow(currentRow_.get());

		SimpleNext();
		if(matches_.list)
			matches_.list->next();
	}

	*row = currentRow_.get();

	if(rowCache)
	{
		cacheGeometry_ = 0;
		cacheShape_.reset();
		currentRow_.reset();
	}

	return true;
}

// IRowCursor
bool ShapefileRowCursor::NextRow(IRowPtr* row)
{
	return NextRowEx(row, 0);
}

// IRowCursor2
bool ShapefileRowCursor::QueryRow(IRow* row)
{
	IRowPtr row2;
	return NextRowEx(&row2, row);
}

// ICursorImpl
IQueryFilterPtr ShapefileRowCursor::AlterQueryFilter(IQueryFilter* filter, ITable* table)
{
	// Add shape field if filter is a spatial filter
	if(filter && table)
	{
		ISpatialFilter* filter2 = gis_interface_cast<ISpatialFilter>(filter);
		IFeatureClass* fclass = gis_interface_cast<IFeatureClass>(table);
		if(filter2 && fclass)
		{
			if(filter2->GetSpatialRel() != gisSpatialRelUndefined)
				filter->GetFieldSet()->Add(fclass->GetShapeFieldName());
		}
	}

	return filter;
}

// Private
bool ShapefileRowCursor::FillRow(IRow* row)
{
	for(int i = 0; i < (int)actualFieldsIndexes_.size(); ++i)
	{
		int fieldIndex = actualFieldsIndexes_[i];

		IVariantEditPtr varEdit = row->GetValue(fieldIndex);

		if(fieldIndex == 0) // OID
		{
			//row->SetValue(fieldIndex, VariantHolder::CreateInstance(GisVariant((int)currentRowID_)).get());
			varEdit->Get() = (int)currentRowID_;
			continue;
		}

		if(fieldIndex == 1) // Shape
		{
			//  // start bb changes
			//  //cacheObject_ = shapelib::SHPReadObject(shp_->file, currentRowID_, cacheObject_);
			//  cacheObject_ = shapelib::SHPReadObject(shp_->file, currentRowID_, 0);
			//  ShapefileUtils::SHPObjectToGeometry(cacheObject_, *cacheGeometry_);
			//  if ( cacheObject_ )
			//  {
			//    shapelib::SHPDestroyObject(cacheObject_);
			//    cacheObject_ = 0;
			//  }
			//  // end bb changes
			//  
			//  gisGeometry::IShapeEditPtr(cacheShape_)->SetSpatialReference(extentSource_->GetSpatialReference().get());
			//  if(!AlterShape(cacheShape_.get()))
			//    return false;
			//  varEdit->Get() = IObjectPtr(cacheShape_);
			//  
			continue;
		}

		int shpFieldIndex = fieldIndex - 2;
		GisString strVal;
		double dblVal;
		int intVal;
		GisVariant& value = varEdit->Get();

		switch(actualFieldsTypes_[i])
		{
		case gisFieldTypeString:
			strVal = shapelib::DBFReadStringAttribute(dbf_->file, currentRowID_, shpFieldIndex);
			value = strVal;
			break;
		case gisFieldTypeInteger:
			intVal = shapelib::DBFReadIntegerAttribute(dbf_->file, currentRowID_, shpFieldIndex);
			value = intVal;
			break;
		case gisFieldTypeDouble:
			dblVal = shapelib::DBFReadDoubleAttribute(dbf_->file, currentRowID_, shpFieldIndex);
			value = dblVal;
			break;
		case gisFieldTypeDate:
			{
				decore::str_t tm_str = shapelib::DBFReadDateAttribute(dbf_->file, currentRowID_, shpFieldIndex);
				int year = atoi(tm_str.left(4).cstr());
				int month = atoi(tm_str.mid(4, 2).cstr());
				int day = atoi(tm_str.right(2).cstr());
				value = decore::datetime_t(year, month, day);
				break;
			}
		default:
			GIS_THROW_DEFAULT_EXCEPTION(L"Invalid field type");
		}
	}

	// Test for where
	if(!AlterRow(currentRow_.get()))
		return false;

	if(shapeFieldIndex_ >= 0) // Shape
	{
		IVariantEditPtr varEdit = row->GetValue(shapeFieldIndex_);
		// start bb changes
		//cacheObject_ = shapelib::SHPReadObject(shp_->file, currentRowID_, cacheObject_);
		cacheObject_ = shapelib::SHPReadObject(shp_->file, currentRowID_);//, 0);
		ShapefileUtils::SHPObjectToGeometry(cacheObject_, *cacheGeometry_);
		if ( cacheObject_ )
		{
			shapelib::SHPDestroyObject(cacheObject_);
			cacheObject_ = 0;
		}
		// end bb changes

		gisGeometry::IShapeEditPtr(cacheShape_)->SetSpatialReference(extentSource_->GetSpatialReference().get());
		if(!AlterShape(cacheShape_.get()))
			return false;
		varEdit->Get() = IObjectPtr(cacheShape_);
	}

	return true;
}

bool ShapefileRowCursor::EOC()
{
	if(invalidCursor_)
		return true;

	if(currentRowID_ >= recordCount_ || (oids_.size() > 0 && rowIDIt_ == oids_.end()) || (matches_.list && matches_.list->eol()))
		return true;

	return false;
}

void ShapefileRowCursor::SimpleNext()
{
	if(oids_.size())
	{
		++rowIDIt_;
		if(rowIDIt_ != oids_.end())
			currentRowID_ = *rowIDIt_;
	}
	else
		++currentRowID_;
}

}
}