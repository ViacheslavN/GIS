#include "stdafx.h"
#include "RectClipper.h"
#include "ClipRectAlloc.h"
namespace GisEngine
{

	namespace Display
	{
		inline bool is_clockwise(const GPoint* points, int pointCount)
		{
#ifdef _FLOAT_GUNITS_
			double S = 0;
#else
			int64 S = 0;
#endif
			for(int i = 0; i < pointCount - 1; i++)
				S += (points[i + 1].y + points[i].y) * (points[i + 1].x - points[i].x);
			return S >= 0;
		}


		CRectClipper::CRectClipper(CClipRectAlloc *pAlloc) : m_pAlloc(pAlloc)
		{

		}
		CRectClipper::~CRectClipper()
		{

		}

		int CRectClipper::clipLine(const GRect& clipper, GPoint*beg, GPoint* end)
		{
			int begCode = getCode(clipper, *beg);
			int endCode = getCode(clipper, *end);
			int ret = clip_contains;

			if(begCode != 0 && endCode == 0)
				if((end->x == clipper.xMin) ||
					(end->x == clipper.xMax) ||
					(end->y == clipper.yMin) ||
					(end->y == clipper.yMax))
				{
					*beg = *end;
					return ret;
				}

				if(begCode == 0 && endCode != 0)
					if((beg->x == clipper.xMin) ||
						(beg->x == clipper.xMax) ||
						(beg->y == clipper.yMin) ||
						(beg->y == clipper.yMax))
					{
						*beg = *end;
						return ret;
					}

					GPoint tmp1 = *beg;
					GPoint tmp2 = *end;

					while(begCode || endCode)
					{
						if(begCode & endCode)
							return clip_disjoint;

						int code;
						clip_res bit;
						GPoint* p;
						if(begCode)
						{
							code = begCode;
							p = beg;
							bit = clip_begin;
						}
						else
						{
							code = endCode;
							p = end;
							bit = clip_end;
						}
						if (code & clip_left) 
						{
#ifdef _FLOAT_GUNITS_
							p->y += (GUnits)(double(beg->y - end->y) * double(clipper.xMin - p->x) / (beg->x - end->x));
#else
							p->y += GUnits(int64(beg->y - end->y) * int64(clipper.Min.x - p->x) / (beg->x - end->x));
#endif
							p->x = clipper.xMin;
							ret |= bit;
						} 
						else if (code & clip_right) 
						{
#ifdef _FLOAT_GUNITS_
							p->y += (GUnits)(double(beg->y - end->y) * double(clipper.xMax - p->x) / (beg->x - end->x));
#else
							p->y += GUnits(int64(beg->y - end->y) * int64(clipper.Max.x - p->x) / (beg->x - end->x));
#endif
							p->x = clipper.xMax;
							ret |= bit;
						}
						if (code & clip_bottom) 
						{
#ifdef _FLOAT_GUNITS_
							p->x += (GUnits)(double(beg->x - end->x) * double(clipper.yMin - p->y) / (beg->y - end->y));
#else
							p->x += GUnits(int64(beg->x - end->x) * int64(clipper.Min.y - p->y) / (beg->y - end->y));
#endif
							p->y = clipper.yMin;
							ret |= bit;
						} 
						else if (code & clip_top) 
						{
#ifdef _FLOAT_GUNITS_
							p->x += (GUnits)(double(beg->x - end->x) * double(clipper.yMax - p->y) / (beg->y - end->y));
#else
							p->x += GUnits(int64(beg->x - end->x) * int64(clipper.Max.y - p->y) / (beg->y - end->y));
#endif
							p->y = clipper.yMax;
							ret |= bit;
						}

						if (code == begCode)
							begCode = getCode(clipper, *beg);
						else
							endCode = getCode(clipper, *end);
					}
					return ret;
		}

		void CRectClipper::clipLine(const GRect& clipper, GPoint** points, int** pointCounts, int* count)
		{
			if(all_points_in_rect(clipper, *points, *pointCounts, *count))
				return;

			GPoint* inPoints = *points;
			int*    inPointCounts = *pointCounts;
			int     inCount = *count;

			int outPointOffset = 0;
			int outPartOffset = 0;
			int offset = 0;

			int pointBufferSize = total_points(*points, *pointCounts, *count);
			int partBufferSize = *count;
			GPoint* outPoints = m_pAlloc->getPointBuf(pointBufferSize, out_points_slot); 
			int* outParts = m_pAlloc->getPartsBuf(partBufferSize, out_parts_slot);

			for(int part = 0; part < inCount; part++)
			{
				int partSize = inPointCounts[part];
				if(partSize < 2)
					continue;

				bool partStarted = false;
				for(int point = offset; point < offset + partSize - 1; point++)
				{
					GPoint begin = inPoints[point];
					GPoint end = inPoints[point + 1];
					int res = clipLine(clipper, &begin, &end);

					if(res & clip_disjoint)
						continue;

					if(!partStarted)
					{
						if(outPartOffset >= partBufferSize)
							outParts = m_pAlloc->getPartsBuf(++partBufferSize, out_parts_slot);

						outParts[outPartOffset++] = outPointOffset;
						partStarted = true;
					}

					if(res & clip_end)
						partStarted = false;

					if(pointBufferSize < outPointOffset + 2)
					{
						outPoints = m_pAlloc->getPointBuf(outPointOffset + 2, out_points_slot);
						pointBufferSize = outPointOffset + 2;
					}

					outPoints[outPointOffset++] = begin;
					if(!partStarted || point == offset + partSize - 2)
						outPoints[outPointOffset++] = end;
				}
				offset += partSize;
			}

			int totalPoints = 0;
			for(int i = 0; i < outPartOffset - 1; i++)
			{
				outParts[i] = outParts[i + 1] - outParts[i];
				totalPoints += outParts[i];
			}
			if(outPartOffset > 1)
				outParts[outPartOffset - 1] = outPointOffset - totalPoints;
			else
				outParts[0] = outPointOffset;

			*points = outPoints;
			*pointCounts = outParts;
			*count = outPartOffset;
		}

		class op
		{
		public:
			op(GUnits border)
				: border_(border)
			{ }
		public:
			enum clip_res
			{
				beg_contains = 1,
				end_contains = 2,
			};
			const GPoint& cross_x(const GPoint& p1, const GPoint& p2) const
			{
				cross_.x = border_;
#ifdef _FLOAT_GUNITS_
				cross_.y = (GUnits)((double(border_ - p1.x) * double(p2.y - p1.y)) / (p2.x - p1.x) + p1.y);
#else
				cross_.y = GUnits((int64(border_ - p1.x) * int64(p2.y - p1.y)) / (p2.x - p1.x) + p1.y);
#endif
				return cross_;
			}
			const GPoint& cross_y(const GPoint& p1, const GPoint& p2) const
			{
				cross_.y = border_;
#ifdef _FLOAT_GUNITS_
				cross_.x = (GUnits)((double(border_ - p1.y) * double(p2.x - p1.x)) / (p2.y - p1.y) + p1.x);
#else
				cross_.x = GUnits((int64(border_ - p1.y) * int64(p2.x - p1.x)) / (p2.y - p1.y) + p1.x);
#endif
				return cross_;
			}

		protected:
			GUnits border_;
			mutable GPoint cross_;
		};

		struct left_op : op
		{
			left_op(GUnits border)
				: op(border)
			{ }
			int compare(const GPoint& p1, const GPoint& p2) const
			{
				int res = 0;
				if(p1.x > border_)
					res |= beg_contains;
				if(p2.x > border_)
					res |= end_contains;
				return res;
			}
			const GPoint& cross(const GPoint& p1, const GPoint& p2) const
			{
				return cross_x(p1, p2);
			}
			bool all_in(const GPoint* points, int count) const
			{
				for(int i = 0; i < count; i++)
					if(points[i].x < border_)
						return false;
				return true;
			}
		};

		struct right_op : op
		{
			right_op(GUnits border)
				: op(border)
			{ }
			int compare(const GPoint& p1, const GPoint& p2) const
			{
				int res = 0;
				if(p1.x < border_)
					res |= beg_contains;
				if(p2.x < border_)
					res |= end_contains;
				return res;
			}
			const GPoint& cross(const GPoint& p1, const GPoint& p2) const
			{
				return cross_x(p1, p2);
			}
			bool all_in(const GPoint* points, int count) const
			{
				for(int i = 0; i < count; i++)
					if(points[i].x > border_)
						return false;
				return true;
			}
		};

		struct top_op : op
		{
			top_op(GUnits border)
				: op(border)
			{ }
			int compare(const GPoint& p1, const GPoint& p2) const
			{
				int res = 0;
				if(p1.y < border_)
					res |= beg_contains;
				if(p2.y < border_)
					res |= end_contains;
				return res;
			}
			const GPoint& cross(const GPoint& p1, const GPoint& p2) const
			{
				return cross_y(p1, p2);
			}
			bool all_in(const GPoint* points, int count) const
			{
				for(int i = 0; i < count; i++)
					if(points[i].y > border_)
						return false;
				return true;
			}
		};

		struct bottom_op : op
		{
			bottom_op(GUnits border)
				: op(border)
			{ }
			int compare(const GPoint& p1, const GPoint& p2) const
			{
				int res = 0;
				if(p1.y > border_)
					res |= beg_contains;
				if(p2.y > border_)
					res |= end_contains;
				return res;
			}
			const GPoint& cross(const GPoint& p1, const GPoint& p2) const
			{
				return cross_y(p1, p2);
			}
			bool all_in(const GPoint* points, int count) const
			{
				for(int i = 0; i < count; i++)
					if(points[i].y < border_)
						return false;
				return true;
			}
		};

		template <typename T>
		bool clip_ring_op(GPoint** points, int* pointCount, const T& op_, size_t slot, CClipRectAlloc *pAlloc)
		{
			if(op_.all_in(*points, *pointCount))
				return true;

			GPoint* outPoints = pAlloc->getPointBuf(*pointCount, slot);
			int bufferSize = *pointCount;
			GPoint* inPoints = *points;
			int pointOffset = 0;

			for(int point = 0; point < *pointCount - 1; point++)
			{
				if(bufferSize < pointOffset + 2)
				{
					bufferSize = pointOffset + 2;
					outPoints = pAlloc->getPointBuf(bufferSize, slot);
				}

				int res = op_.compare(inPoints[point], inPoints[point + 1]);
				if(res & op::beg_contains)
				{
					outPoints[pointOffset++] = inPoints[point];
					if(!(res & op::end_contains))
						outPoints[pointOffset++] = op_.cross(inPoints[point], inPoints[point + 1]);
				}
				else if(res & op::end_contains)
					outPoints[pointOffset++] = op_.cross(inPoints[point], inPoints[point + 1]);
			}

			
			if(pointOffset >= 1 && outPoints[0] != outPoints[pointOffset - 1])
			{
				pointOffset++;
				if(bufferSize < pointOffset)
					outPoints = (GPoint*)pAlloc->getPointBuf(pointOffset, slot);
				outPoints[pointOffset - 1] = outPoints[0];
			}

			*points = outPoints;
			*pointCount = pointOffset;
			return false;
		}

		void CRectClipper::clipRing(const GRect& clipper, GPoint** points, int* pointCount)
		{
			if(all_points_in_rect(clipper, *points, pointCount, 1))
				return;

			if(all_points_out_rect(clipper, *points, pointCount, 1))
			{
				if(!has_poly_intersections(clipper, *points, *pointCount))
				{
					*pointCount = 0;
					return;
				}
			}

			bool clockwise = is_clockwise(*points, *pointCount);

			int slot = base_slot;

			if(!clip_ring_op(points, pointCount, left_op(clipper.xMin), slot, m_pAlloc))
				slot++;

			if(!clip_ring_op(points, pointCount, top_op(clipper.yMax), slot, m_pAlloc))
				slot++;

			if(!clip_ring_op(points, pointCount, right_op(clipper.xMax), slot, m_pAlloc))
				slot++;

			if(!clip_ring_op(points, pointCount, bottom_op(clipper.yMin), slot, m_pAlloc))
				slot++;

			if(clockwise != is_clockwise(*points, *pointCount)) 
			{
				GPoint* pnts = *points;
				for(int i = 0, sz = *pointCount / 2; i < sz; i++)
				{
					GPoint tmp = pnts[i];
					pnts[i] = pnts[*pointCount - i - 1];
					pnts[*pointCount - i - 1] = tmp;
				}
			}
		}

		void CRectClipper::clipPolygon(const GRect& clipper, GPoint** points, int** pointCounts, int* count)
		{
			if(all_points_in_rect(clipper, *points, *pointCounts, *count))
				return;

			GPoint* inPoints = *points;
			int*    inParts = *pointCounts;
			int     inCount = *count;

			int totalPoints = total_points(inPoints, inParts, inCount);

			int pointBufferSize = 0;
			int partBufferSize = 0;

			int outPointOffset = 0;
			int outPartOffset = 0;

			GPoint* outPoints = 0;
			int*    outParts = 0;

			for(int i = 0, offset = 0; i < inCount; i++)
			{
				GPoint* ringPoints = inPoints + offset;
				int ringPointCount = inParts[i];
				offset += ringPointCount;

				clipRing(clipper, &ringPoints, &ringPointCount);
				if(ringPointCount == 0)
					continue;

				pointBufferSize += ringPointCount;
				partBufferSize++;

				outPoints = m_pAlloc->getPointBuf(pointBufferSize, out_points_slot);
				outParts = m_pAlloc->getPartsBuf(partBufferSize, out_parts_slot);

				::memcpy(&outPoints[outPointOffset], ringPoints, sizeof(GPoint) * ringPointCount);
				outParts[outPartOffset++] = ringPointCount;

				outPointOffset += ringPointCount;
			}
			*points = outPoints;
			*pointCounts = outParts;
			*count = outPartOffset;
		}

		int CRectClipper::getCode(const GRect& clipper, const GPoint& pnt)
		{
			int code = (clipper.xMin > pnt.x) ? clip_left : 0;
			code |= (clipper.xMax < pnt.x) ? clip_right : 0;
			code |= (clipper.yMin > pnt.y) ? clip_bottom: 0;
			code |= (clipper.yMax < pnt.y) ? clip_top : 0;
			return code;
		}

		bool CRectClipper::all_points_in_rect(const GRect& clipper, GPoint* points, int* pointCounts, int count)
		{
			bool allIn = true;
			int offset = 0;
			for(int part = 0; part < count; part++)
			{
				for(int point = offset; point < offset + pointCounts[part]; point++)
				{
					if(!clipper.pointInRect(points[point]))
					{
						allIn = false;
						break;
					}
				}
				offset += pointCounts[part];
			}
			return allIn;
		}

		bool CRectClipper::all_points_out_rect(const GRect& clipper, GPoint* points, int* pointCounts, int count)
		{
			bool allOut = true;
			int  offset = 0;
			for(int part = 0; part < count; part++)
			{
				for(int point = offset; point < offset + pointCounts[part]; point++)
				{
					if(clipper.pointInRect(points[point]))
					{
						allOut = false;
						break;
					}
				}
				offset += pointCounts[part];
			}
			return allOut;
		}

		int CRectClipper::total_points(GPoint* points, int* pointCounts, int count)
		{
			int totalPoints = 0;
			for(int part = 0; part < count; part++)
				totalPoints += pointCounts[part];
			return totalPoints;
		}

		bool CRectClipper::has_poly_intersections(const GRect& clipper, GPoint* points, int count)
		{
			if(count < 2)
				return false;

			int code = getCode(clipper, points[0]);
			for(int i = 1; i < count; i++)
			{
				code &= getCode(clipper, points[i]);
				if(code == 0)
					return true;
			}

			return false;
		}

	}

}