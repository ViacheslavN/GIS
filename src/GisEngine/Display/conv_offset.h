#ifndef GIS_ENGINE_DISPLAY_CONVOFFSET_H
#define GIS_ENGINE_DISPLAY_CONVOFFSET_H
#include "GraphTypes.h"
#include "Point.h"
namespace GisEngine
{
	namespace Display
	{

		class conv_offset
		{
		public:
			conv_offset(void);
			~conv_offset(void);
		public:
			// возращает точки со смещением
			static void gen(GUnits offset, const GPoint** points, int* pointCounts, int count);
		private:
			static bool calc_segment(const GPoint& p1, 
				const GPoint& p2, 
				const GPoint& p3,                       
				GUnits offset,
				GPoint& beg, 
				GPoint& end,
				bool first_seg,
				bool *use_both);

			static bool calc_segment(const GPoint& p1, 
				const GPoint& p2, 
				GUnits offset,
				GPoint& beg, 
				GPoint& end);

			static bool gen_line_koef(const GPoint& p1, const GPoint& p2, double* a, double* b, double* c);
			static bool is_parallel(double A1, double B1, double A2, double B2);
			static bool is_same_orientation(double A1, double B1, double A2, double B2);
			static bool cross(double A1, double B1, double C1, double A2, double B2, double C2, GPoint& p);
		public:
			static double len(const GPoint& p1, const GPoint& p2);
		};

	}

}

#endif