#include "stdafx.h"
#include "BoundaryBox.h"

namespace CommonLib
{
	bbox_t::bbox_t(const bbox_t& bbox)
		: type(bbox.type)
		, xMin(bbox.xMin)
		, xMax(bbox.xMax)
		, yMin(bbox.yMin)
		, yMax(bbox.yMax)
		, zMin(bbox.zMin)
		, zMax(bbox.zMax)
		, mMin(bbox.mMin)
		, mMax(bbox.mMax)
	{}

	bbox_t& bbox_t::operator=(const bbox_t& bbox)
	{
		if(this == &bbox)
			return *this;

		type = bbox.type;
		xMin = bbox.xMin;
		xMax = bbox.xMax;
		yMin = bbox.yMin;
		yMax = bbox.yMax;
		zMin = bbox.zMin;
		zMax = bbox.zMax;
		mMin = bbox.mMin;
		mMax = bbox.mMax;

		return *this;
	}

	bool bbox_t::operator==(const bbox_t &bbox) const
	{
		if(type != bbox.type)
			return false;

		if(type == bbox_type_null || type == bbox_type_infinite)
			return true;

		if(type == bbox_type_invalid)
			return false;

		if(xMin != bbox.xMin || xMax != bbox.xMax || yMin != bbox.yMin || yMax != bbox.yMax)
			return false;

		if((type & bbox_has_zs) != 0 && (zMin != bbox.zMin || zMax != bbox.zMax))
			return false;

		if((type & bbox_has_ms) != 0 && (mMin != bbox.mMin || mMax != bbox.mMax))
			return false;

		return true;
	}

	bool bbox_t::operator!=(const bbox_t &bbox) const
	{
		return !(this->operator ==(bbox));
	}

	bool bbox_t::operator<(const bbox_t &bbox) const
	{
		if(type != bbox.type)
			return type < bbox.type;

		if(type == bbox_type_null || type == bbox_type_infinite || type == bbox_type_invalid)
			return true;

		if(xMin != bbox.xMin || xMax != bbox.xMax || yMin != bbox.yMin || yMax != bbox.yMax)
		{
			return xMin != bbox.xMin ? xMin < bbox.xMin : (yMin != bbox.yMin ? yMin < bbox.yMin : 
				(xMax != bbox.xMax ? xMax < bbox.xMax : (yMax != bbox.yMax ? yMax < bbox.yMax : false)));
		}

		if((type & bbox_has_zs) != 0 && (zMin != bbox.zMin || zMax != bbox.zMax))
		{
			return zMin != bbox.zMin ? zMin < bbox.zMin : zMax < bbox.zMax;
		}

		if((type & bbox_has_ms) != 0 && (mMin != bbox.mMin || mMax != bbox.mMax))
		{
			return mMin != bbox.mMin ? mMin < bbox.mMin : mMax < bbox.mMax;
		}

		return false;
	}

	bool bbox_t::doesInclude(const bbox_t &bbox, bool useZ /*= false*/, bool useM /*= false*/) const
	{
		if(type == bbox_type_invalid || bbox.type == bbox_type_invalid)
			return false;

		if(bbox.type == bbox_type_null)
			return true;

		if(type == bbox_type_null)
			return false;

		if(type == bbox_type_infinite)
			return true;

		if(bbox.type == bbox_type_infinite)
			return false;

		if(xMin > bbox.xMin || xMax < bbox.xMax || yMin > bbox.yMin || yMax < bbox.yMax)
			return false;

		if(useZ && ((type & bbox_has_zs) != 0) && (zMin > bbox.zMin || zMax < bbox.zMax))
			return false;

		if(useM && ((type & bbox_has_ms) != 0) && (mMin > bbox.mMin || mMax < bbox.mMax))
			return false;

		return true;
	}

	bool bbox_t::contains(const bbox_t &bbox, bool useZ, bool useM) const
	{
		return doesInclude(bbox, useZ, useM);
	}

	bool bbox_t::intersects(const bbox_t &bbox, bool useZ, bool useM) const
	{
		if(type == bbox_type_invalid || bbox.type == bbox_type_invalid)
			return false;

		if(type == bbox_type_null || bbox.type == bbox_type_null)
			return false;

		if(type == bbox_type_infinite || bbox.type == bbox_type_infinite)
			return true;

		if(xMin > bbox.xMax || yMin > bbox.yMax || xMax < bbox.xMin || yMax < bbox.yMin)
			return false;

		if(useZ && ((type & bbox_has_zs) != 0) && (zMin > bbox.zMin || zMax < bbox.zMin))
			return false;

		if(useM && ((type & bbox_has_ms) != 0) && (mMin > bbox.mMin || mMax < bbox.mMin))
			return false;

		return true;
	}

	bbox_t& bbox_t::expand(const bbox_t &bbox)
	{
		if(this == &bbox)
			return *this;

		if(bbox.type == bbox_type_null || bbox.type == bbox_type_invalid)
			return *this;

		if(type == bbox_type_infinite)
			return *this;

		if(bbox.type == bbox_type_infinite)
		{
			type = bbox_type_infinite;
			return *this;
		}

		if(type == bbox_type_null || type == bbox_type_invalid)
		{
			type = bbox.type;
			xMin = bbox.xMin;
			xMax = bbox.xMax;
			yMin = bbox.yMin;
			yMax = bbox.yMax;
			zMin = bbox.zMin;
			zMax = bbox.zMax;
			mMin = bbox.mMin;
			mMax = bbox.mMax;
			return *this;
		}

		if(xMin > bbox.xMin)
			xMin = bbox.xMin;

		if(xMax < bbox.xMax)
			xMax = bbox.xMax;

		if(yMin > bbox.yMin)
			yMin = bbox.yMin;

		if(yMax < bbox.yMax)
			yMax = bbox.yMax;

		if(zMin > bbox.zMin)
			zMin = bbox.zMin;

		if(zMax < bbox.zMax)
			zMax = bbox.zMax;

		if(mMin > bbox.mMin)
			mMin = bbox.mMin;

		if(mMax < bbox.mMax)
			mMax = bbox.mMax;

		return *this;
	}
}