#include "stdafx.h"
#include <math.h>
#include "Units.h"


namespace GisEngine
{
	namespace GisCommon
	{
#ifndef M_PI
#define M_PI            3.14159265358979323846
#endif

#define RAD2DEG(r) ((double)(r) * (180.0 / M_PI))
#define DEG2RAD(d) ((double)(d) * (M_PI / 180.0))

		///////////////////////////////////////////////////////////////////////////////////////
		////  length conversions

		const double c_inchesPerMeter = 39.37;
		const double c_metersPerInch = (1.0 / c_inchesPerMeter);
		const double c_cmPerInch = (100.0 / c_inchesPerMeter);
		const double c_mmPerInch = (1000.0 / c_inchesPerMeter);
		const double c_inchesPerCm = (1.0 / c_cmPerInch);
		const double c_inchesPerFoot = 12.0;
		const double c_feetPerYard = 3.0;
		const double c_inchesPerYard = (c_inchesPerFoot * c_feetPerYard);
		const double c_feetPerMile = 5280.0;
		const double c_feetPerNauticalMile = 6080.27;
		const double c_inchesPerMile = (c_inchesPerFoot * c_feetPerMile);
		const double c_inchesPerNauticalMile = (c_inchesPerFoot * c_feetPerNauticalMile);
		const double c_defaultSemiMajorAxis = 6378137.0;    // Use WGS84 spheroid
		const double c_defaultFlattening = 0.003352813;     // 1 / 298.257

		double DegreesToMeters(double degrees)
		{
			// This is a bastardization of the algorithm used in S_DegreePtsToMeters
			// for use with a distance instead of two points.  It is only accurate
			// at the equator, and gets less accurate the farther you get from the
			// equator.

			long numHalfCircles = (long)(degrees / 180);
			double remainder = degrees - (numHalfCircles*180);
			double L = DEG2RAD(remainder)/2;
			double sL = sin(L);
			double cL = cos(L);
			return (20015077.0 * (double) numHalfCircles) + (12741994.0 * atan(sqrt((sL*sL)/(cL*cL))));
		}

		double ToMeters(double value, Units units)
		{
			switch (units)
			{
			case UnitsInches:
				return (value * c_metersPerInch);
			case UnitsPoints:
				return (value * c_metersPerInch) / 72.0;
			case UnitsFeet:
				return ((value * c_inchesPerFoot) * c_metersPerInch);
			case UnitsYards:
				return ((value * c_inchesPerYard)* c_metersPerInch);
			case UnitsMiles:
				return ((value * c_inchesPerMile) * c_metersPerInch);
			case UnitsNauticalMiles:
				return ((value * c_inchesPerNauticalMile) * c_metersPerInch);
			case UnitsMillimeters:
				return (value / 1000.0);
			case UnitsCentimeters:
				return (value / 100.0);
			case UnitsDecimeters:
				return (value / 10.0);
			case UnitsMeters:
				return value;
			case UnitsKilometers:
				return (value * 1000.0);
			case UnitsDecimalDegrees:
				return DegreesToMeters(value);
			}
			return value; // whatever...
		}

		double FromMeters(double meters, Units units)
		{
			switch (units)
			{
			case UnitsInches:
				return (meters * c_inchesPerMeter);
			case UnitsPoints:
				return (meters * c_inchesPerMeter) * 72.0;
			case UnitsFeet:
				return ((meters * c_inchesPerMeter) / c_inchesPerFoot);
			case UnitsYards:
				return ((meters * c_inchesPerMeter) / c_inchesPerYard);
			case UnitsMiles:
				return ((meters * c_inchesPerMeter) / c_inchesPerMile);
			case UnitsNauticalMiles:
				return ((meters * c_inchesPerMeter) / c_inchesPerNauticalMile);
			case UnitsMillimeters:
				return (meters * 1000.0);
			case UnitsCentimeters:
				return (meters * 100.0);
			case UnitsDecimeters:
				return (meters * 10.0);
			case UnitsMeters:
				return meters;
			case UnitsKilometers:
				return (meters / 1000.0);
			case UnitsDecimalDegrees:
				return (meters * 0.000008993220293);
			}
			return meters;
		}

		// Area conversion constants:
		const double c_sqMetersPerSqMillimeter  = 1.0E-6;
		const double c_sqMetersPerSqCentimeter  = 1.0E-4;
		const double c_sqMetersPerSqDecimeter   = 1.0E-2;
		const double c_sqMetersPerAre           = 1.0E+2;
		const double c_sqMetersPerHectare       = 1.0E+4;
		const double c_sqMetersPerSqKilometer   = 1.0E+6;
		const double c_sqMetersPerSqInch        = 6.4516E-4;
		const double c_sqMetersPerSqFoot        = 9.290304E-2;                              // (144 sq inches)
		const double c_sqMetersPerSqYard        = 8.3612736E-1;                             // (9 sq feet)
		const double c_sqMetersPerAcre          = 4.0468564224E+3;                          // (4840 sq yards)
		const double c_sqMetersPerSqMile        = 2.58998811E+6;                            // (640 acres)

		const double c_sqMillimetersPerSqMeter  = (1.0 / c_sqMetersPerSqMillimeter);
		const double c_sqCentimetersPerSqMeter  = (1.0 / c_sqMetersPerSqCentimeter);
		const double c_sqDecimetersPerSqMeter   = (1.0 / c_sqMetersPerSqDecimeter);
		const double c_aresPerSqMeter           = (1.0 / c_sqMetersPerAre);
		const double c_hectaresPerSqMeter       = (1.0 / c_sqMetersPerHectare);
		const double c_sqKilometersPerSqMeter   = (1.0 / c_sqMetersPerSqKilometer);
		const double c_sqInchesPerSqMeter       = (1.0 / c_sqMetersPerSqInch);
		const double c_sqFeetPerSqMeter         = (1.0 / c_sqMetersPerSqFoot);
		const double c_sqYardsPerSqMeter        = (1.0 / c_sqMetersPerSqYard);
		const double c_acresPerSqMeter          = (1.0 / c_sqMetersPerAcre);
		const double c_sqMilesPerSqMeter        = (1.0 / c_sqMetersPerSqMile);

		// Converts the value from square meters to the specified units
		double FromSquareMeters(double sqMeters, AreaUnits units)
		{
			switch (units)
			{
			case AreaUnitsSquareInches:       return (sqMeters * c_sqInchesPerSqMeter);
			case AreaUnitsSquareFeet:         return (sqMeters * c_sqFeetPerSqMeter);
			case AreaUnitsSquareYards:        return (sqMeters * c_sqYardsPerSqMeter);
			case AreaUnitsAcres:              return (sqMeters * c_acresPerSqMeter);
			case AreaUnitsSquareMiles:        return (sqMeters * c_sqMilesPerSqMeter);
			case AreaUnitsSquareMillimeters:  return (sqMeters * c_sqMillimetersPerSqMeter);
			case AreaUnitsSquareCentimeters:  return (sqMeters * c_sqCentimetersPerSqMeter);
			case AreaUnitsSquareDecimeters:   return (sqMeters * c_sqDecimetersPerSqMeter);
			case AreaUnitsSquareMeters:       return sqMeters;
			case AreaUnitsAres:               return (sqMeters * c_aresPerSqMeter);
			case AreaUnitsHectares:           return (sqMeters * c_hectaresPerSqMeter);
			case AreaUnitsSquareKilometers:   return (sqMeters * c_sqKilometersPerSqMeter);
			default:
				//    ASSERT(0);
				break;
			}
			return sqMeters;
		}

		// Converts the value from the specified units to square meters
		double ToSquareMeters(double value, AreaUnits units)
		{
			switch (units)
			{
			case AreaUnitsSquareInches:      return (value * c_sqMetersPerSqInch);
			case AreaUnitsSquareFeet:        return (value * c_sqMetersPerSqFoot);
			case AreaUnitsSquareYards:       return (value * c_sqMetersPerSqYard);
			case AreaUnitsAcres:             return (value * c_sqMetersPerAcre);
			case AreaUnitsSquareMiles:       return (value * c_sqMetersPerSqMile);
			case AreaUnitsSquareMillimeters: return (value * c_sqMetersPerSqMillimeter);
			case AreaUnitsSquareCentimeters: return (value * c_sqMetersPerSqCentimeter);
			case AreaUnitsSquareDecimeters:  return (value * c_sqMetersPerSqDecimeter);
			case AreaUnitsSquareMeters:      return value;
			case AreaUnitsAres:              return (value * c_sqMetersPerAre);
			case AreaUnitsHectares:          return (value * c_sqMetersPerHectare);
			case AreaUnitsSquareKilometers:  return (value * c_sqMetersPerSqKilometer);
			default:
				//      ASSERT(0);
				break;
			}

			return value;
		}

		double ConvertUnits(double len, Units fromUnits, Units toUnits)
		{
			if (fromUnits == toUnits)
				return len;

			if (fromUnits == UnitsUnknown || toUnits == UnitsUnknown)
				return len;

			return FromMeters(ToMeters(len, fromUnits), toUnits);
		}

		double ConvertAreaUnits(double area, AreaUnits fromUnits, AreaUnits toUnits)
		{
			if (fromUnits == toUnits)
				return area;

			if (fromUnits == AreaUnitsUnknown || toUnits == AreaUnitsUnknown)
				return area;

			return FromSquareMeters(ToSquareMeters(area, fromUnits), toUnits);
		}

		AreaUnits GetAreaUnitByLinearUnit(Units unit)
		{
			switch(unit)
			{
			case UnitsInches: return AreaUnitsSquareInches ;
			case UnitsFeet: return AreaUnitsSquareFeet ;
			case UnitsYards: return AreaUnitsSquareYards ;
			case UnitsMiles: return AreaUnitsSquareMiles ;
			case UnitsMillimeters: return AreaUnitsSquareMillimeters ;
			case UnitsCentimeters: return AreaUnitsSquareCentimeters ;
			case UnitsMeters: return AreaUnitsSquareMeters ;
			case UnitsKilometers: return AreaUnitsSquareKilometers ;
			}
			return AreaUnitsUnknown;
		}

		double CalcRadiansDistance(double lat0, double long0, double lat1, double long1)
		{
			double cosPlus = cos(lat0 + lat1);
			double cosMinus = cos(lat0 - lat1);
			double sproduct = 0.5 * ((cosMinus + cosPlus) * cos(long1 - long0) + (cosMinus - cosPlus));
			return acos(sproduct);
		}

		double CalcMetersDistance(double lat0, double long0, double lat1, double long1)
		{
			return RAD2DEG(CalcRadiansDistance(DEG2RAD(lat0), DEG2RAD(long0), DEG2RAD(lat1), DEG2RAD(long1))) * 111194.8742964;
		}

	}
}