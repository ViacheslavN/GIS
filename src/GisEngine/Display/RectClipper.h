#ifndef GIS_ENGINE_DISPLAY_RECT_CLIP_H_
#define GIS_ENGINE_DISPLAY_RECT_CLIP_H_

#include "Clip.h"
#include "CommonLibrary/alloc_t.h"
#include "ClipRectAlloc.h"
namespace GisEngine
{
	namespace Display
	{
		
		class CRectClipper : public IClip
		{
		public:
			 CRectClipper(CommonLib::alloc_t *pAlloc);
			 ~CRectClipper();
			 virtual int  clipLine(const GRect& clipper, GPoint* pBeg, GPoint* pEnd) = 0;
			 virtual void clipLine(const GRect& clipper, GPoint** ppPoints, int** ppnPointCounts, int* pCount) = 0;
			 virtual void clipRing(const GRect& clipper, GPoint** ppPoints, int* pPointCount) = 0;
			 virtual void clipPolygon(const GRect& clipper, GPoint** ppPoints, int** ppPointCounts, int* pCount) = 0;
		private:
			enum clip_bits
			{
				clip_left =   1,
				clip_right =  2,
				clip_top =    4,
				clip_bottom = 8
			};
			enum clip_res
			{
				clip_contains = 0,
				clip_begin = 1,
				clip_end = 2,
				clip_disjoint = 4
			};

			enum clip_memory_slots
			{
				in_points_slot,
				in_parts_slot,
				out_points_slot,
				out_parts_slot,

				base_slot = 100
			};


		private:
			int   getCode(const GRect& clipper, const GPoint& pnt);
			bool  all_points_in_rect(const GRect& clipper, GPoint* points, int* pointCounts, int count);
			bool  all_points_out_rect(const GRect& clipper, GPoint* points, int* pointCounts, int count);
			bool  has_poly_intersections(const GRect& clipper, GPoint* points, int count);
			int   total_points(GPoint* points, int* pointCounts, int count);
		private:
			CommonLib::alloc_t *m_pAlloc;
			CommonLib::simple_alloc_t m_alloc;
		};
	}
}


#endif