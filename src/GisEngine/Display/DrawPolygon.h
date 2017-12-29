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
			void AddEdge(const GPoint& pt1, const GPoint& pt2);
			void AddEdge1(const GPoint& pt1, const GPoint& pt2);
			void SortCell();
			void Draw();
			void AddCell(int x, int y, int bDirection);
		private:
			struct SCell
			{
				int m_x;
				int m_y;
				int m_nDirection;

				bool isCoordEqual(const SCell& cell) const
				{
					return m_x == cell.m_x && m_y == cell.m_y;
				}

				SCell(int x, int y, int nDirection) :m_x(x), m_y(y),  m_nDirection(nDirection)
				{}
			};

			typedef std::vector<SCell> TCells;
		private:
			TCells m_vecCells;
			bool m_bAutoClosePolygon;
			std::vector<int> m_vecY;
		};
	}
}