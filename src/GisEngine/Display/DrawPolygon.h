#pragma once
namespace GisEngine
{

	namespace Display
	{

		
		class CDrawPolygonOpenGL
		{
			public:
				CDrawPolygonOpenGL(bool bAutoClosePolygon = true);
				~CDrawPolygonOpenGL();

			void Init(uint32 nWeight, uint32 nHeight);
			void DrawPolygon(const GPoint* pPoints, int nNumPoints);
			void DrawPolyPolygon(const GPoint* lpPoints, const int *lpPolyCounts, int nCount);
		private:
			void AddEdge(const GPoint& pt1, const GPoint& pt2, int idx1, int idx2);
			void AddEdge1(const GPoint& pt1, const GPoint& pt2, int idx1, int idx2);
			void SortCell();
			void Draw(const GPoint* lpPoints);
			void AddCell(int x, int y, int idx1, int idx2);
		private:
			struct SCell
			{
				int m_x;
				int m_y;
				int n_idx1;
				int n_idx2;

				bool isCoordEqual(const SCell& cell) const
				{
					return m_x == cell.m_x && m_y == cell.m_y;
				}

				SCell(int x, int y, int nIdx1, int nIdx2) :m_x(x), m_y(y), n_idx1(nIdx1), n_idx2(nIdx2)
				{}
			};

			typedef std::vector<SCell> TCells;

			struct SLine
			{

				SLine(int x) : m_x(x), m_len(0)
				{}

				int m_x;
				int m_len;

			};
			typedef std::vector<SLine> TLines;

		private:
			TCells m_vecCells;
			bool m_bAutoClosePolygon;
			std::vector<int> m_vecY;

			
		};
	}
}