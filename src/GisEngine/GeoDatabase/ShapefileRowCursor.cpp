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
				spatialRel_ =  srlUndefined;
				if(spatFilter)
					spatialRel_ = spatFilter->GetSpatialRel();

				if(spatialRel_ != srlUndefined)
				{
					CommonLib::IGeoShapePtr pShape(spatFilter->GetShape());
					extentOutput_ = new Geometry::CEnvelope(pShape->getBB(), spatRefOutput.get());
					extentSource_ = new Geometry::CEnvelope(pShape->getBB(), spatRefOutput.get());
					extentOutput_->Project(spatRefOutput.get());
					extentSource_->Project(spatRefSource.get());
				}
				else
				{
					extentOutput_ = new Geometry::CEnvelope(GisBoundingBox(), spatRefOutput.get());
					extentSource_ = new Geometry::CEnvelope(GisBoundingBox(), spatRefSource.get());
				}

				needTransform_ = spatRefOutput != NULL 
					&& spatRefSource != NULL 
					&& !spatRefOutput->IsEqual(spatRefSource.get());
				

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
						IFeature* feature = (IFeature*)(currentRow_.get());
						if(feature)
						{
							cacheShape_ = new CommonLib::CGeoShape();
							feature->SetShape(cacheShape_.get());
						}
					}
				}

				recordGood = FillRow(currentRow_.get());

				SimpleNext();
				
			}

			*row = currentRow_.get();

			if(rowCache)
			{
				cacheShape_.reset();
				currentRow_.reset();
			}

			return true;
		}


		bool CShapefileRowCursor::NextRow(IRowPtr* row)
		{
			return NextRowEx(row, 0);
		}


		bool CShapefileRowCursor::FillRow(IRow* row)
		{
			for(int i = 0; i < (int)actualFieldsIndexes_.size(); ++i)
			{
				int fieldIndex = actualFieldsIndexes_[i];

				CommonLib::CVariant* pValue = row->GetValue(fieldIndex);

				if(fieldIndex == 0) // OID
				{				
					*pValue = (int)currentRowID_;
					continue;
				}

				if(fieldIndex == 1) // Shape
				{
					continue;
				}

				int shpFieldIndex = fieldIndex - 2;
				CommonLib::str_t strVal;
				double dblVal;
				int intVal;
		
				switch(actualFieldsTypes_[i])
				{
					case dtString:
						strVal = ShapeLib::DBFReadStringAttribute(dbf_->file, currentRowID_, shpFieldIndex);
						*pValue  = strVal;
						break;
					case dtInteger8:
					case dtInteger16:
					case dtInteger32:
					case dtUInteger8:
					case dtUInteger16:
					case dtUInteger32:
						intVal = ShapeLib::DBFReadIntegerAttribute(dbf_->file, currentRowID_, shpFieldIndex);
						*pValue  = intVal;
						break;
					case dtDouble:
						dblVal = ShapeLib::DBFReadDoubleAttribute(dbf_->file, currentRowID_, shpFieldIndex);
						*pValue  = dblVal;
						break;
					default:
						return false;
				}
			}

			if(shapeFieldIndex_ >= 0) // Shape
			{
				CommonLib::CVariant* pShapeVar = row->GetValue(shapeFieldIndex_);
				cacheObject_ = ShapeLib::SHPReadObject(shp_->file, currentRowID_);
				ShapefileUtils::SHPObjectToGeometry(cacheObject_, *cacheShape_);
				if ( cacheObject_ )
				{
					ShapeLib::SHPDestroyObject(cacheObject_);
					cacheObject_ = 0;
				}
				// end bb changes
							
				if(!AlterShape(cacheShape_.get()))
					return false;
				*pShapeVar  = CommonLib::IRefObjectPtr(cacheShape_.get());
			}

			return true;
		}

		 
		bool CShapefileRowCursor::AlterShape(CommonLib::CGeoShape* pShape) const
		{
			if(!pShape)
				return !(extentOutput_->GetBoundingBox().type & CommonLib::bbox_type_normal);

			if (needTransform_&& pShape)
				extentSource_->GetSpatialReference()->Project(extentOutput_->GetSpatialReference().get(), pShape);
		

			GisBoundingBox boxShape = pShape->getBB();
			GisBoundingBox boxOutput = extentOutput_->GetBoundingBox();
			if((boxShape.type & CommonLib::bbox_type_normal) && (boxOutput.type & CommonLib::bbox_type_normal))
			{
				if (boxShape.xMin > boxOutput.xMax || boxShape.xMax < boxOutput.xMin || 
					boxShape.yMin > boxOutput.yMax || boxShape.yMax < boxOutput.yMin)
				{
					return false;
				}
			}

			return true;
		}


		bool CShapefileRowCursor::EOC()
		{
			if(invalidCursor_)
				return true;

			if(currentRowID_ >= recordCount_ || (oids_.size() > 0 && rowIDIt_ == oids_.end()))
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