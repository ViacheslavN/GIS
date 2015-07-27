#include "stdafx.h"
#include "IGeoShape.h"


namespace CommonLib
{
	bool IGeoShape::isTypeSimple(eShapeType shapeType)
	{
		return shapeType<= shape_type_multipatch  && shapeType >= shape_type_null;
	}

	eShapeType IGeoShape::generalType(eShapeType _general_type)
	{
		eShapeType shapeType = _general_type;

		if(isTypeSimple(shapeType))
		{
			switch(shapeType)
			{
			case shape_type_null:
				return shape_type_null;

			case shape_type_point:
			case shape_type_point_z:
			case shape_type_point_m:
			case shape_type_point_zm:
				return shape_type_general_point;

			case shape_type_multipoint:
			case shape_type_multipoint_z:
			case shape_type_multipoint_m:
			case shape_type_multipoint_zm:
				return shape_type_general_multipoint;

			case shape_type_polyline:
			case shape_type_polyline_z:
			case shape_type_polyline_m:
			case shape_type_polyline_zm:
				return shape_type_general_polyline;

			case shape_type_polygon:
			case shape_type_polygon_z:
			case shape_type_polygon_m:
			case shape_type_polygon_zm:
				return shape_type_general_polygon;

			case shape_type_multipatch:
			case shape_type_multipatch_m:
				return shape_type_general_multipatch;
			}
		}

		return (eShapeType)(shapeType & shape_basic_type_mask);
	}
	void IGeoShape::getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id)
	{
		if(IGeoShape::isTypeSimple(shapeType))
		{
			if(has_z != NULL)
				*has_z = false;
			if(has_m != NULL)
				*has_m = false;
			if(has_curve != NULL)
				*has_curve = false;
			if(has_id != NULL)
				*has_id = false;

			eShapeType gType;
			switch(shapeType)
			{
			case shape_type_point:
			case shape_type_point_m:
			case shape_type_point_zm:
			case shape_type_point_z:
				gType = shape_type_general_point;
				break;
			case shape_type_multipoint:
			case shape_type_multipoint_m:
			case shape_type_multipoint_zm:
			case shape_type_multipoint_z:
				gType = shape_type_general_multipoint;
				break;
			case shape_type_polyline:
			case shape_type_polyline_m:
			case shape_type_polyline_zm:
			case shape_type_polyline_z:
				gType = shape_type_general_polyline;
				break;
			case shape_type_polygon:
			case shape_type_polygon_m:
			case shape_type_polygon_zm:
			case shape_type_polygon_z:
				gType = shape_type_general_polygon;
				break;
			case shape_type_multipatch_m:
			case shape_type_multipatch:
				gType = shape_type_general_multipatch;
				break;
			default:
				gType = shape_type_null;
			}

			if(gType != shape_type_null)
			{
				if(has_m != NULL)
				{
					if(shapeType == shape_type_point_m ||
						shapeType == shape_type_multipoint_m ||
						shapeType == shape_type_polyline_m ||
						shapeType == shape_type_polygon_m ||
						shapeType == shape_type_multipatch_m ||
						shapeType == shape_type_point_zm ||
						shapeType == shape_type_multipoint_zm ||
						shapeType == shape_type_polyline_zm ||
						shapeType == shape_type_polygon_zm)
						*has_m = true;
				}

				if(has_z != NULL)
				{
					if(shapeType == shape_type_point_z ||
						shapeType == shape_type_multipoint_z ||
						shapeType == shape_type_polyline_z ||
						shapeType == shape_type_polygon_z ||
						shapeType == shape_type_multipatch ||
						shapeType == shape_type_multipatch_m ||
						shapeType == shape_type_point_zm ||
						shapeType == shape_type_multipoint_zm ||
						shapeType == shape_type_polyline_zm ||
						shapeType == shape_type_polygon_zm)
						*has_z = true;
				}
			}

			if(pGenType != NULL)
				*pGenType = gType;
		}
		else 
		{
			if(pGenType != NULL)
				*pGenType = (eShapeType)(shapeType & shape_basic_type_mask);
			if(has_z != NULL)
			{
				if((eShapeType)(shapeType & shape_basic_type_mask) == shape_type_general_multipatch)
					*has_z = true;
				else
					*has_z = (shapeType & shape_has_zs) != 0;
			}
			if(has_m != NULL)
				*has_m = (shapeType & shape_has_ms) != 0;
			if(has_curve != NULL)
			{
				if((shapeType & shape_non_basic_modifier_mask) == 0 && 
					(shapeType == shape_type_general_polyline || shapeType == shape_type_general_polygon))
					*has_curve = true;
				else
					*has_curve = (shapeType & shape_has_curves) != 0;
			}
			if(has_id != NULL)
				*has_id = (shapeType & shape_has_ids) != 0;
		}

	}
}