#include "stdafx.h"
#include "ShapefileRowCursor.h"
#include "ShapefileFeatureClass.h"
#include "FieldSet.h"
#include "Feature.h"
#include "GisGeometry/Envelope.h"
namespace GisEngine
{
	namespace GeoDatabase
	{
		CShapefileRowCursor::CShapefileRowCursor(IQueryFilter* filter, bool recycling, CShapefileFeatureClass* pFClass) :
			shp_(pFClass->GetSHP())
			, dbf_(pFClass->GetDBF())
			, cacheObject_(NULL)
			, parentFC_(pFClass)
			, invalidCursor_(false)
			, recycling_(recycling)
		{
		
			filter_ = filter;

			if(!filter_->GetWhereClause().isEmpty())
			{
				 //TO DO set fields
			}
			sourceFields_ = parentFC_->GetFields();

			invalidCursor_ = parentFC_->reload(false);
			int fieldCount = sourceFields_->GetFieldCount();
			fieldsExists_.resize(fieldCount, 0);
			actualFieldsIndexes_.clear();


			CommonLib::str_t field;
			filter_->GetFieldSet()->Reset();

			oidFieldIndex_ = -1;
			shapeFieldIndex_ = -1;
			annoFieldIndex_ = -1;

			while(filter_->GetFieldSet()->Next(&field))
			{
				if(field == L"*")
				{
					filter_->GetFieldSet()->Clear();
					for(int i = 0; i < fieldCount; ++i)
					{
						IFieldPtr field = sourceFields_->GetField(i);
						filter_->GetFieldSet()->Add(field->GetName());
					}

					filter_->GetFieldSet()->Reset();
					actualFieldsIndexes_.clear();
					actualFieldsTypes_.clear();
					continue;
				}
				int fieldIndex = sourceFields_->FindField(field);
				fieldsExists_[fieldIndex] = 1;
				actualFieldsIndexes_.push_back(fieldIndex);
				actualFieldsTypes_.push_back(sourceFields_->GetField(fieldIndex)->GetType());

				if(actualFieldsTypes_.back() == dtOid && oidFieldIndex_ < 0)
					oidFieldIndex_ = fieldIndex;
				if(actualFieldsTypes_.back() == dtGeometry && (shapeFieldIndex_ < 0 || shapeFieldIndex_ > fieldIndex))
					shapeFieldIndex_ = fieldIndex;
				if(actualFieldsTypes_.back() == dtAnnotation && (annoFieldIndex_ < 0 || annoFieldIndex_ > fieldIndex))
					annoFieldIndex_ = fieldIndex;
			}

			// Change fieldset to right names (from DB)
			int actualfieldCount = (int)actualFieldsIndexes_.size();
			IFieldSetPtr fieldSet(new CFieldSet());
			for(int i = 0; i < actualfieldCount; ++i)
				fieldSet->Add(sourceFields_->GetField(actualFieldsIndexes_[i])->GetName());
			fieldSet->Reset();
			filter_->SetFieldSet(fieldSet.get());

			IOIDSetPtr oidSet = filter_->GetOIDSet();
			oidSet->Reset();

			int oid;
			while(oidSet->Next(&oid))
				oids_.push_back(oid);

			std::sort(oids_.begin(), oids_.end());

			rowIDIt_ = oids_.begin();

			// Spatial queries
		
				Geometry::ISpatialReferencePtr spatRefOutput(filter_->GetOutputSpatialReference());
				Geometry::ISpatialReferencePtr spatRefSource(pFClass->GetSpatialReference());

				ISpatialFilter* spatFilter = (ISpatialFilter*)filter_.get();
				spatialRel_ = gisGeoDatabase::gisSpatialRelUndefined;
				if(spatFilter)
					spatialRel_ = spatFilter->GetSpatialRel();

				if(spatialRel_ != gisGeoDatabase::gisSpatialRelUndefined)
				{
					gisGeometry::IShapePtr shape = spatFilter->GetShape();
					extentOutput_ = gisSystem::IClonePtr(shape->GetEnvelope())->Clone();
					extentSource_ = gisSystem::IClonePtr(extentOutput_)->Clone();
					extentOutput_->Project(spatRefOutput.get());
					extentSource_->Project(spatRefSource.get());
				}
				else
				{
					extentOutput_ = gisGeometry::Envelope::CreateInstance(gisCommon::GisBoundingBox(), spatRefOutput.get());
					extentSource_ = gisGeometry::Envelope::CreateInstance(gisCommon::GisBoundingBox(), spatRefSource.get());
				}

				needTransform_ = spatRefOutput != NULL 
					&& spatRefSource != NULL 
					&& !gisCommon::gis_interface_cast<gisSystem::IClone>(spatRefOutput.get())->IsEqual(gisCommon::gis_interface_cast<gisSystem::IClone>(spatRefSource.get()));
				

			if(!invalidCursor_)
			{
				currentRowID_ = oids_.size() ? *rowIDIt_ : 0;
				ShapeLib::SHPGetInfo(shp_->file, &recordCount_, NULL, NULL, NULL);
			
			}
		}

		CShapefileRowCursor::~CShapefileRowCursor()
		{
			if(cacheObject_)
				ShapeLib::SHPDestroyObject(cacheObject_);

			if(!invalidCursor_)
				parentFC_->close();
		}

		bool CShapefileRowCursor::NextRowEx(IRowPtr* row, IRow* rowCache)
		{
			if(invalidCursor_)
				return false;

			bool recordGood = false;

			while(!recordGood)
			{
				if(EOC())
				{
					currentRow_.reset();
					row->reset();
					return false;
				}

				if(rowCache || !currentRow_ || !recycling_)
				{
					if(rowCache)
						currentRow_ = rowCache;
					else
						currentRow_ = new  CFeature(filter_->GetFieldSet().get(), sourceFields_.get());
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
		bool CShapefileRowCursor::NextRow(IRowPtr* row)
		{
			return NextRowEx(row, 0);
		}

		// IRowCursor2
		bool CShapefileRowCursor::QueryRow(IRow* row)
		{
			IRowPtr row2;
			return NextRowEx(&row2, row);
		}

		// ICursorImpl
		IQueryFilterPtr CShapefileRowCursor::AlterQueryFilter(IQueryFilter* filter, ITable* table)
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
		bool CShapefileRowCursor::FillRow(IRow* row)
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

		bool CShapefileRowCursor::EOC()
		{
			if(invalidCursor_)
				return true;

			if(currentRowID_ >= recordCount_ || (oids_.size() > 0 && rowIDIt_ == oids_.end()) || (matches_.list && matches_.list->eol()))
				return true;

			return false;
		}

		void CShapefileRowCursor::SimpleNext()
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