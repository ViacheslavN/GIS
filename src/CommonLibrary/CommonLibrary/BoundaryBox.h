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

	struct bbox_t
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

		bbox_t();
		bbox_t(const bbox_t& bbox);
		bbox_t& operator=(const bbox_t& bbox);
		bool    operator==(const bbox_t &bbox) const; 
		bool    operator!=(const bbox_t &bbox) const;
		bool    operator<(const bbox_t &bbox) const;
		bool    doesInclude(const bbox_t &bbox, bool useZ = false, bool useM = false) const;
		bool    contains(const bbox_t &bbox, bool useZ = false, bool useM = false) const;
		bool    intersects(const bbox_t &bbox, bool useZ = false, bool useM = false) const;
		bbox_t& expand(const bbox_t &bbox);
	};

}

#endif