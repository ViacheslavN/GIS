#include "StdAfx.h"
#include "conv_offset.h"
#include <math.h>

namespace GisEngine
{
	namespace Display
	{

		conv_offset::conv_offset(void)
		{
		}

		conv_offset::~conv_offset(void)
		{
		}

		void conv_offset::gen(GUnits offset, const GPoint** points, int* pointCounts, int count)
		{
			const GPoint* inPoints = *points;
			GPoint* outPoints = 0;

			int pointBufferSize = 0;
			int outPointOffset = 0;

			for(int part = 0, off = 0; part < count; part++)
			{
				if(pointCounts[part] < 2)
					continue;

				if(pointBufferSize + pointCounts[part] > pointBufferSize)
				{
					pointBufferSize = pointBufferSize + pointCounts[part];
					outPoints = (GPoint*) malloc (sizeof(GPoint) * pointBufferSize);
				}

				GPoint beg, end;

				if(pointCounts[part] == 2)
				{
					if(calc_segment(inPoints[off], inPoints[off + 1], offset, beg, end))
					{
						outPoints[outPointOffset++] = beg;
						outPoints[outPointOffset++] = end;
					}
					off += 2;
					continue;
				}

				bool use_both;
				int count = 0;
				for(int point = off, sz = off + pointCounts[part] - 2; point < sz; point++)
				{
					int second_point = point + 1, third_point = point + 2;
					bool cont = false;
					while(inPoints[point] == inPoints[second_point])
					{
						if(second_point == sz + 1)
						{
							cont = true;
							break;
						}
						second_point ++;
						third_point ++;
					}
					if(cont)
						continue;

					while(inPoints[second_point] == inPoints[third_point])
					{
						if(third_point == sz + 2)
						{
							cont = true;
							break;
						}
						third_point ++;
					}
					if(cont)
						continue;

					if(!calc_segment(inPoints[point], inPoints[second_point], inPoints[third_point], offset, beg, end, true, &use_both))
						continue;

					if(outPointOffset + 2 > pointBufferSize)
					{
						pointBufferSize = outPointOffset + 2;
						outPoints = (GPoint*)malloc (sizeof(GPoint) * pointBufferSize);
					}

					if(point == off)
					{
						outPoints[outPointOffset++] = beg;
						outPoints[outPointOffset++] = end;
						count += 2;
					}
					else
					{
						if(use_both)
						{
							outPoints[outPointOffset++] = beg;
							count++;
						}
						outPoints[outPointOffset++] = end;
						count++;
					}
				}
				int first_point = off + pointCounts[part] - 3, second_point = off + pointCounts[part] - 2, third_point = off + pointCounts[part] - 1;
				bool ok = true;
				while(inPoints[third_point] == inPoints[second_point])
				{
					if(second_point == off + 1)
					{
						ok = false;
						break;
					}
					second_point--;
					first_point--;
				}
				while(ok && inPoints[second_point] == inPoints[first_point])
				{
					if(first_point == off)
					{
						ok = false;
						break;
					}
					first_point--;
				}

				if(ok && calc_segment(inPoints[first_point], inPoints[second_point], inPoints[third_point], offset, beg, end, false, &use_both))
				{
					if(outPointOffset + 2 > pointBufferSize)
					{
						pointBufferSize = outPointOffset + 2;
						outPoints = (GPoint*)malloc (sizeof(GPoint) * pointBufferSize);
					}
					if(use_both)
					{
						outPoints[outPointOffset++] = beg;
						count++;
					}
					outPoints[outPointOffset++] = end;
					count++;
				}
				off += pointCounts[part];
				pointCounts[part] = count;
			}
			*points = outPoints;
		}

		bool conv_offset::calc_segment(const GPoint& p1, 
			const GPoint& p2, 
			const GPoint& p3,
			GUnits offset,
			GPoint& beg, 
			GPoint& end,
			bool first_seg,
			bool *use_both)
		{
			*use_both = false;

			offset = -offset;

			double A1, B1, C1, A2, B2, C2;
			if(!gen_line_koef(p1, p2, &A1, &B1, &C1))
				return false;
			if(!gen_line_koef(p2, p3, &A2, &B2, &C2))
				return false;

			if(is_parallel(A1, B1, A2, B2) && !is_same_orientation(A1, B1, A2, B2))
				*use_both = true;

			double l1 = sqrt(A1 * A1 + B1 * B1);
			double vec_y_1 = B1 / l1, vec_x_1 = A1 / l1;

			double l2 = sqrt(A2 * A2 + B2 * B2);
			double vec_y_2 = B2 / l2, vec_x_2 = A2 / l2;

			if(first_seg)
			{
				beg.x = GUnits(p1.x + vec_x_1 * double(offset));
				beg.y = GUnits(p1.y + vec_y_1 * double(offset));
			}
			else
			{
				end.x = GUnits(p3.x + vec_x_2 * double(offset));
				end.y = GUnits(p3.y + vec_y_2 * double(offset));
			}

			if(*use_both || is_parallel(A1, B1, A2, B2))
			{
				if(first_seg)
				{
					end.x = GUnits(p2.x + vec_x_1 * double(offset));
					end.y = GUnits(p2.y + vec_y_1 * double(offset));
				}
				else
				{
					end.x = GUnits(p3.x + vec_x_2 * double(offset));
					end.y = GUnits(p3.y + vec_y_2 * double(offset));
				}
			}
			else
			{
				if(first_seg)
					cross(A1, B1, C1 - offset * l1, A2, B2, C2 - offset * l2, end);
				else
					cross(A1, B1, C1 - offset * l1, A2, B2, C2 - offset * l2, beg);
			}

			// отсекаем через чур длинные линии
			bool cut = false;
			double maxlen = 0;
			double l = len(beg, end);
			if(first_seg)
			{
				maxlen = len(p1, p2) + abs((int)offset);
				if(l > maxlen)
					cut = true;
			}
			else
			{
				maxlen = len(p2, p3) + abs((int)offset);
				if(l > maxlen)
					cut = true;
			}

			if(cut)
			{
				*use_both = true;
				double koef = maxlen / l;
				if(first_seg)
				{
					end.x = GUnits(beg.x + koef * (end.x - beg.x));
					end.y = GUnits(beg.y + koef * (end.y - beg.y));
				}
				else
				{
					beg.x = GUnits(end.x - koef * (end.x - beg.x));
					beg.y = GUnits(end.y - koef * (end.y - beg.y));
				}
			}

			return true;
		}

		bool conv_offset::calc_segment(const GPoint& p1, 
			const GPoint& p2, 
			GUnits offset,
			GPoint& beg, 
			GPoint& end)
		{
			offset = -offset;

			double A1, B1, C1;
			if(!gen_line_koef(p1, p2, &A1, &B1, &C1))
				return false;
			double l1 = sqrt(A1 * A1 + B1 * B1);
			double vec_y_1 = B1 / l1, vec_x_1 = A1 / l1;

			beg.x = GUnits(p1.x + vec_x_1 * double(offset));
			beg.y = GUnits(p1.y + vec_y_1 * double(offset));
			end.x = GUnits(p2.x + vec_x_1 * double(offset));
			end.y = GUnits(p2.y + vec_y_1 * double(offset));
			return true;
		}

		bool conv_offset::gen_line_koef(const GPoint& p1, const GPoint& p2, double* a, double* b, double* c)
		{
			if(p1 == p2)
				return false;

			*a = p1.y - p2.y;
			*b = p2.x - p1.x;
			*c = (p1.x * (-(*a)) - p1.y * (*b));
			return true;
		}

		bool conv_offset::is_parallel(double A1, double B1, double A2, double B2)
		{
			const double e = std::numeric_limits<double>::epsilon() * 10;
			return fabs(A1*B2 - A2*B1) < e;
		}

		bool conv_offset::is_same_orientation(double A1, double B1, double A2, double B2)
		{
			const double e = std::numeric_limits<double>::epsilon() * 10;
			return (fabs(A1 - A2) < e) && (fabs(B1 - B2) < e);
		}

		bool conv_offset::cross(double A1, double B1, double C1, double A2, double B2, double C2, GPoint& p)
		{
			if(!is_parallel(A1, B1, A2, B2))
			{
				p.x = GUnits((B1*C2 - B2*C1) / (A1*B2 - A2*B1));
				p.y = GUnits((C1*A2 - C2*A1) / (A1*B2 - A2*B1));
				return true;
			}
			return false;
		}

		double conv_offset::len(const GPoint& p1, const GPoint& p2)
		{
			return sqrt(double((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)));
		}

	}

}