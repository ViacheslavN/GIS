#ifndef _LIB_GIS_ENGINE_COMMON_UNITS_
#define _LIB_GIS_ENGINE_COMMON_UNITS_


namespace GisEngine
{
	namespace Common
	{
		enum Units
		{ 
			UnitsUnknown         = 0,
			UnitsInches          = 1,
			UnitsPoints          = 2,
			UnitsFeet            = 3,
			UnitsYards           = 4,
			UnitsMiles           = 5,
			UnitsNauticalMiles   = 6,
			UnitsMillimeters     = 7,
			UnitsCentimeters     = 8,
			UnitsMeters          = 9,
			UnitsKilometers      = 10,
			UnitsDecimalDegrees  = 11,
			UnitsDecimeters      = 12
		};

		enum AreaUnits
		{ 
			AreaUnitsUnknown           = 0,
			AreaUnitsSquareInches      = 1,
			AreaUnitsSquareFeet        = 2,
			AreaUnitsSquareYards       = 3,
			AreaUnitsAcres             = 4,
			AreaUnitsSquareMiles       = 5,
			AreaUnitsSquareMillimeters = 6,
			AreaUnitsSquareCentimeters = 7,
			AreaUnitsSquareDecimeters  = 8,
			AreaUnitsSquareMeters      = 9,
			AreaUnitsAres              = 10,
			AreaUnitsHectares          = 11,
			AreaUnitsSquareKilometers  = 12
		};

		enum DirectionType
		{
			DTNorthAzimuth = 0,
			DTSouthAzimuth = 1,
			DTPolar = 2,
			DTQuadrantBearing = 3
		};

		enum DirectionUnits
		{
			DURadians = 0,
			DUDecimalDegrees = 1,
			DUDegreesMinutesSeconds = 2,
			DUGradians = 3,
			DUGons = 4
		};


		double ConvertUnits(double len, Units fromUnits, Units toUnits);
		double ConvertAreaUnits(double area, AreaUnits fromUnits, AreaUnits toUnits);
		AreaUnits GetAreaUnitByLinearUnit(Units unit);
		double CalcRadiansDistance(double lat0, double long0, double lat1, double long1);
		double CalcMetersDistance(double lat0, double long0, double lat1, double long1);
	}
}
#endif