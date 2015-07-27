#ifndef _LIB_COMMON_BOUNDARY_BOX_H_
#define _LIB_COMMON_BOUNDARY_BOX_H_

namespace CommonLib
{
	enum bbox_type
	{
		bbox_type_invalid  = 0xFFFFFFFF,
		bbox_type_null     = 0,
		bbox_type_infinite = 1,
		bbox_type_normal   = 2
	};

	enum bbox_modifiers
	{
		bbox_has_zs          = 0x80000000,
		bbox_has_ms          = 0x40000000,
		bbox_basic_type_mask = 0x000000FF,
		bbox_modifier_mask   = 0xFF000000
	};

	struct bbox
	{
		bbox_type type;
		double xMin;
		double xMax;
		double yMin;
		double yMax;
		double zMin;
		double zMax;
		double mMin;
		double mMax;

		bbox();
		bbox(const bbox& bbox);
		bbox& operator=(const bbox& bbox);
		bool    operator==(const bbox &bbox) const; 
		bool    operator!=(const bbox &bbox) const;
		bool    operator<(const bbox &bbox) const;
		bool    doesInclude(const bbox &bbox, bool useZ = false, bool useM = false) const;
		bool    contains(const bbox &bbox, bool useZ = false, bool useM = false) const;
		bool    intersects(const bbox &bbox, bool useZ = false, bool useM = false) const;
		bbox& expand(const bbox &bbox);
	};

}

#endif