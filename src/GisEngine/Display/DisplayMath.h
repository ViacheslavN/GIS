#pragma once
#include "GraphTypes.h"
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


		const double intersection_epsilon = 1.0e-30;



		inline double cross_product(double x1, double y1,	double x2, double y2,	double x, double y)
		{
			return (x - x2) * (y2 - y1) - (y - y2) * (x2 - x1);
		}

		inline bool calc_intersection(double ax, double ay, double bx, double by,
			double cx, double cy, double dx, double dy,
			double* x, double* y)
		{
			double num = (ay - cy) * (dx - cx) - (ax - cx) * (dy - cy);
			double den = (bx - ax) * (dy - cy) - (by - ay) * (dx - cx);
			if (fabs(den) < intersection_epsilon) return false;
			double r = num / den;
			*x = ax + r * (bx - ax);
			*y = ay + r * (by - ay);
			return true;
		}

		inline double calc_distance(double x1, double y1, double x2, double y2)
		{
			double dx = x2 - x1;
			double dy = y2 - y1;
			return sqrt(dx * dx + dy * dy);
		}

	
		



	}

}
