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

			TVecPoints& GetPoints() { return m_out;}


			void GenerateLine(const TVecPoints& inVecLine);
			void SetWidth(double dWidth);
			void SetCapType(eCapType capType) { m_capType = capType; }
			void SetJoinType(eJoinType joinType) { m_JoinType = joinType; }
			void SetInnerJoinType(eInnerJoinType innerJoinType) { m_InnerJoinType = innerJoinType; }
			void SetMitterLimit(double dMitterLimit) {	m_dMiter_limit = dMitterLimit;	}
			void SetInnerMitterLimit(double dInnerMitterLimit) { m_dInner_miter_limit = dInnerMitterLimit; }
		private:
			
			void CalcJoin(const GPoint& v0, const GPoint& v1, const GPoint& v2);
			void CalcCap(const GPoint& v0, const GPoint& v1);

			void calc_miter(const GPoint& v0, const GPoint& v1, const GPoint& v2,
				double dx1, double dy1,
				double dx2, double dy2,
				eJoinType lj,
				double mlimit,
				double dbevel);

			void calc_arc(double x, double y, double dx1, double dy1, double dx2, double dy2);
			void add_vertex(double x, double y);
		private:
			TVecPoints m_out;
			eCapType m_capType;
			eJoinType m_JoinType;
			eInnerJoinType m_InnerJoinType;
			double       m_dWidth;
			double       m_dWidth_abs;
			double       m_dWidthEps;
			int          m_nWidth_sign;
			double       m_dMiter_limit;
			double       m_dInner_miter_limit;
			double       m_dApprox_scale;
		};
	}
}