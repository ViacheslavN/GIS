#ifndef GIS_ENGINE_DISPLAY_DISPLAY_DISPLAYMATH_H
#define GIS_ENGINE_DISPLAY_DISPLAY_DISPLAYMATH_H

namespace GisEngine
{

	namespace Display
	{

		//! Rounding error constant often used when comparing double values.
		const double ROUNDING_ERROR	= 0.00001f;

		const float  ROUNDING_ERROR_FLOAT	= 0.001f;

		//! Constant for PI.
		const double PI				= 3.1415926535897932384626433832795;

		//! Constant for converting bettween degrees and radiants.
		const double GRAD_PI		= 180.0 / 3.1415926535897932384626433832795;

		//! Constant for converting bettween degrees and radiants.
		const double GRAD_PI2		= 3.1415926535897932384626433832795 / 180.0;

		//! returns minimum of two values. Own implementation to get rid of the STL.
		template<class T>
		inline const T min_(const T a, const T b)
		{
			return a < b ? a : b;
		}

		//! returns minimum of two values. Own implementation to get rid of the STL
		template<class T>
		inline T max_(const T a, const T b)
		{
			return a < b ? b : a;
		}

		//! returns if a float equals the other one, taking floating 
		//! point rounding errors into account
		inline bool equals(double a, double b)
		{
			return (a + ROUNDING_ERROR > b) && (a - ROUNDING_ERROR < b);
		}

		inline bool equals(float a, float b)
		{
			return (a + ROUNDING_ERROR_FLOAT > b) && (a - ROUNDING_ERROR_FLOAT < b);
		}

	}

}

#endif