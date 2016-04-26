#include "stdafx.h"
#include "importFromShape.h"
#include "CommonLibrary/File.h"
#include "CommonLibrary/BoundaryBox.h"
#include "../GisEngine/GisGeometry/SpatialReferenceProj4.h"
#include "../../EmbDB/embDB.h"

embDB::eSpatialCoordinatesUnits GetGeometryUnits(GisEngine::GisCommon::Units units)
{
	switch(units)
	{
	case GisEngine::GisCommon::UnitsUnknown:
		return embDB::scuUnknown;
		break;
	case GisEngine::GisCommon::UnitsInches:
		return embDB::scuInches;
		break;
	case GisEngine::GisCommon::UnitsPoints:
		return embDB::scuPoints;
		break;
	case GisEngine::GisCommon::UnitsFeet:
		return embDB::scuFeet;
		break;
	case GisEngine::GisCommon::UnitsYards:
		return embDB::scuYards;
		break;
	case GisEngine::GisCommon::UnitsMiles:
		return embDB::scuMiles;
		break;
	case GisEngine::GisCommon::UnitsNauticalMiles:
		return embDB::scuNauticalMiles;
		break;
	case GisEngine::GisCommon::UnitsMillimeters:
		return embDB::scuMillimeters;
		break;
	case GisEngine::GisCommon::UnitsCentimeters:
		return embDB::scuCentimeters;
		break;
	case GisEngine::GisCommon::UnitsMeters:
		return embDB::scuMeters;
		break;
	case GisEngine::GisCommon::UnitsKilometers:
		return embDB::scuKilometers;
		break;
	case GisEngine::GisCommon::UnitsDecimalDegrees:
		return embDB::scuDecimalDegrees;
		break;
	case GisEngine::GisCommon::UnitsDecimeters:
		return embDB::scuDecimeters;
		break;
	}

	return embDB::scuUnknown;
}
void SHPObjectToGeometry(ShapeLib::SHPObject* obj, CommonLib::CGeoShape& result)
{
	result.create(SHPTypeToGeometryType(obj->nSHPType, NULL, NULL), obj->nVertices, obj->nParts);

	GisXYPoint* pPt = result.getPoints();
	double* zs = result.getZs();
	double* ms = result.getMs();
	int i;
	for(i = 0; i < obj->nVertices; ++i, ++pPt, zs += zs ? 1 : 0, ms += ms ? 1 : 0)
	{
		pPt->x = obj->padfX[i];
		pPt->y = obj->padfY[i];
		if(obj->padfZ && zs)
			*zs = obj->padfZ[i];
		if(obj->padfM && ms)
			*ms = obj->padfM[i];
	}

	uint32* parts = result.getParts();
	CommonLib::patch_type* partTypes = result.getPartsTypes();
	if ( obj->nParts != 0 )
	{
		for(i = 0; i < obj->nParts; ++i, ++parts, partTypes += partTypes ? 1 : 0)
		{
			*parts = (long)obj->panPartStart[i];
			if(obj->panPartType && partTypes)
			{
				switch(obj->panPartType[i])
				{
				case SHPP_TRISTRIP:
					*partTypes = CommonLib::patch_type_triangle_strip;
					break;
				case SHPP_TRIFAN:
					*partTypes = CommonLib::patch_type_triangle_fan;
					break;
				case SHPP_OUTERRING:
					*partTypes = CommonLib::patch_type_outer_ring;
					break;
				case SHPP_INNERRING:
					*partTypes = CommonLib::patch_type_inner_ring;
					break;
				case SHPP_FIRSTRING:
					*partTypes = CommonLib::patch_type_first_ring;
					break;
				case SHPP_RING:
					*partTypes = CommonLib::patch_type_ring;
					break;
				}
			}
		}
	}

	result.calcBB();
}
