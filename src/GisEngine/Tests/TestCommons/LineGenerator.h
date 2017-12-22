#pragma once


namespace GisEngine
{
	namespace Display
	{

		class  CLineGenerator
		{
		public:

			enum eFlags
			{
				eReady = 0,
				eCapIn = 1,
				eCapOut = 2,
				eInLine = 4,
				eOutLine = 5,
				eStop = 6

			};


			CLineGenerator();
			~CLineGenerator();


			void CreateLine(std::vector<GPoint>& vecLine);
			
			void SetWidth(double dWidth);

		private:
			double cross_product(double x1, double y1,
				double x2, double y2,
				double x, double y)
			{
				return (x - x2) * (y2 - y1) - (y - y2) * (x2 - x1);
			}

			bool calc_intersection(double ax, double ay, double bx, double by,
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
			double calc_distance(const GPoint& pt0, const GPoint& pt1)
			{
				return calc_distance(pt0.x, pt0.y, pt1.x, pt1.y);
			}

			double calc_distance(double x1, double y1, double x2, double y2)
			{
				double dx = x2 - x1;
				double dy = y2 - y1;
				return sqrt(dx * dx + dy * dy);
			}
			void CalcJoin(const GPoint& v0, const GPoint& v1, const GPoint& v2);
				void CalcCap(const GPoint& v0, const GPoint& v1);

			void calc_miter(const GPoint& v0,	const GPoint& v1,	const GPoint& v2,
					double dx1, double dy1,
					double dx2, double dy2,
					eJoinType lj,
					double mlimit,
					double dbevel);

			void calc_arc(double x, double y,	double dx1, double dy1,	double dx2, double dy2);
			void add_vertex(double x, double y);
			const double pi = 3.14159265358979323846;
			// See calc_intersection
			const double intersection_epsilon = 1.0e-30;
		private:
			typedef std::vector<GPoint> TPoints;
			TPoints m_out;

			eCapType m_capType;
			eJoinType m_JoinType;
			eInnerJoinType m_InnerJoinType;
			double       m_width;
			double       m_width_abs;
			double       m_width_eps;
			int          m_width_sign;
			double       m_miter_limit;
			double       m_inner_miter_limit;
			double       m_approx_scale;
		};
 
	}
}