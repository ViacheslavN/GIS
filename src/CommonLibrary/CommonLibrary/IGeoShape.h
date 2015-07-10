#ifndef _LIB_COMMON_I_GEO_SHAPE_H_
#define _LIB_COMMON_I_GEO_SHAPE_H_
#include "general.h"
#include "alloc_t.h"
#include "BoundaryBox.h"
namespace CommonLib
{
	enum eShapeType
	{
		shape_type_null               = 0,
		shape_type_point              = 1,
		shape_type_point_m            = 21,
		shape_type_point_zm           = 11,
		shape_type_point_z            = 9,
		shape_type_multipoint         = 8,
		shape_type_multipoint_m       = 28,
		shape_type_multipoint_zm      = 18,
		shape_type_multipoint_z       = 20,
		shape_type_polyline           = 3,
		shape_type_polyline_m         = 23,
		shape_type_polyline_zm        = 13,
		shape_type_polyline_z         = 10,
		shape_type_polygon            = 5,
		shape_type_polygon_m          = 25,
		shape_type_polygon_zm         = 15,
		shape_type_polygon_z          = 19,
		shape_type_multipatch_m       = 31,
		shape_type_multipatch         = 32,
		shape_type_general_polyline   = 50,
		shape_type_general_polygon    = 51,
		shape_type_general_point      = 52,
		shape_type_general_multipoint = 53,
		shape_type_general_multipatch = 54,
		shape_type_last               = shape_type_general_multipatch + 1
	};


	enum eShapeMasks
	{
		shape_has_zs                    = 0x80000000,
		shape_has_ms                    = 0x40000000,
		shape_has_curves                = 0x20000000,
		shape_has_ids                   = 0x10000000,
		shape_has_normals               = 0x08000000,
		shape_has_textures              = 0x04000000,
		shape_has_partid                = 0x02000000,
		shape_has_materials             = 0x01000000,
		shape_is_compressed             = 0x00800000,
		shape_modifier_mask             = 0xFF000000,
		shape_multi_patch_modifier_mask = 0x00F00000,
		shape_basic_type_mask           = 0x000000FF,
		shape_basic_modifier_mask       = shape_has_zs | shape_has_ms,
		shape_non_basic_modifier_mask   = shape_modifier_mask & ~shape_basic_modifier_mask,
		shape_extended_modifier_mask    = shape_basic_modifier_mask | shape_has_ids | shape_has_normals | shape_has_textures | shape_has_materials
	};
	enum patch_type
	{
		patch_type_triangle_strip = 0,
		patch_type_triangle_fan   = 1,
		patch_type_outer_ring     = 2,
		patch_type_inner_ring     = 3,
		patch_type_first_ring     = 4,
		patch_type_ring           = 5,
		patch_type_triangles      = 6
	};


	enum segment_type
	{
		segment_type_circular_arc  = 1,
		segment_type_line          = 2,
		segment_type_spiral        = 3,
		segment_type_bezier3_curve = 4,
		segment_type_elliptic_arc  = 5
	};

	enum circular_arc_flags
	{
		circular_arc_flag_is_empty = 1,
		circular_arc_flag_is_ccw   = 8,
		circular_arc_flag_is_minor = 16,
		circular_arc_flag_is_line  = 32,
		circular_arc_flag_is_point = 64,
		circular_arc_flag_is_defined = 128
	};

	enum elliptic_arc_flags
	{
		elliptic_arc_flag_is_empty    = 1,
		elliptic_arc_flag_is_line     = 64,
		elliptic_arc_flag_is_point    = 128,
		elliptic_arc_flag_is_circular = 256,
		elliptic_arc_flag_center_to   = 512,
		elliptic_arc_flag_center_from = 1024,
		elliptic_arc_flag_is_ccw      = 2048,
		elliptic_arc_flag_is_minor    = 4096,
		elliptic_arc_flag_is_complete = 8192
	};
	struct GeoPt
	{
		double x;
		double y;
	};


	class IGeoShape
	{
		public:
			IGeoShape(){};
			virtual ~IGeoShape(){}
			virtual eShapeType type() const = 0;
			virtual uint32  size() const = 0;
			virtual uint32  getPartCount() const = 0;
			virtual uint32  getPartSize(size_t idx) const = 0;


	};
}
#endif