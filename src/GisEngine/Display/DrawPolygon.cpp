#include "stdafx.h"
#include "GraphTypes.h"
#include "DisplayMath.h"
#include "Point.h"
#include "DrawPolygon.h"
#include <gl/GL.h>
namespace GisEngine
{
	namespace Display
	{
		CDrawPolygonOpenGL::CDrawPolygonOpenGL(bool bAutoClosePolygon): m_bAutoClosePolygon(bAutoClosePolygon)
		{

		}
		CDrawPolygonOpenGL::~CDrawPolygonOpenGL()
		{

		}

		void CDrawPolygonOpenGL::Init(uint32 nWeight, uint32 nHeight)
		{
			m_vecY.resize(nHeight + 1);
		}

		void CDrawPolygonOpenGL::DrawPolygon(const GPoint* pPoints, int nNumPoints)
		{
			DrawPolyPolygon(pPoints, &nNumPoints, 1);
		}
		void CDrawPolygonOpenGL::DrawPolyPolygon(const GPoint* lpPoints, const int *lpPolyCounts, int nCount)
		{
			
			m_vecCells.clear();
			std::fill(m_vecY.begin(), m_vecY.end(), 0);


			int offcet = 0;
			for (int i = 0; i < nCount; ++i)
			{

				int nParts = lpPolyCounts[i];
				if(nParts < 3)
					continue;

				bool bClose = lpPoints[offcet] == lpPoints[nParts - 1 + offcet];

				for (int j = 1; j < nParts; ++j)
				{
					AddEdge(lpPoints[j - 1 + offcet], lpPoints[j + offcet]);
				}

				if(!bClose)
					AddEdge(lpPoints[nParts - 1 + offcet], lpPoints[0]);

				offcet += nParts;
			}

			SortCell();
			Draw();

		}

		/*
		
		void bhm_line(int x1,int y1,int x2,int y2,int c)
{
 int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
 dx=x2-x1;
 dy=y2-y1;
 dx1=fabs(dx);
 dy1=fabs(dy);
 px=2*dy1-dx1;
 py=2*dx1-dy1;
 if(dy1<=dx1)
 {
  if(dx>=0)
  {
   x=x1;
   y=y1;
   xe=x2;
  }
  else
  {
   x=x2;
   y=y2;
   xe=x1;
  }
  putpixel(x,y,c);
  for(i=0;x<xe;i++)
  {
   x=x+1;
   if(px<0)
   {
	px=px+2*dy1;
   }
   else
   {
	if((dx<0 && dy<0) || (dx>0 && dy>0))
	{
	 y=y+1;
	}
	else
	{
	 y=y-1;
	}
	px=px+2*(dy1-dx1);
   }
   delay(0);
   putpixel(x,y,c);
  }
 }
 else
 {
  if(dy>=0)
  {
   x=x1;
   y=y1;
   ye=y2;
  }
  else
  {
   x=x2;
   y=y2;
   ye=y1;
  }
  putpixel(x,y,c);
  for(i=0;y<ye;i++)
  {
   y=y+1;
   if(py<=0)
   {
	py=py+2*dx1;
   }
   else
   {
	if((dx<0 && dy<0) || (dx>0 && dy>0))
	{
	 x=x+1;
	}
	else
	{
	 x=x-1;
	}
	py=py+2*(dx1-dy1);
   }
   delay(0);
   putpixel(x,y,c);
  }
 }
}
		
		*/
		void CDrawPolygonOpenGL::AddEdge(const GPoint& pt1, const GPoint& pt2)
		{

			int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
			dx = pt2.x - pt1.x;
			dy = pt2.y - pt1.y;
			GUnits dVectorProduct = pt1.x*pt2.y - pt2.x*pt1.y;
			int nDirection = dVectorProduct > 0 ? 1 : -1;
			if (dy == 0)
			{
				AddCell(pt1.x, pt1.y, nDirection);
				AddCell(pt2.x, pt2.y, nDirection);
				return;
			}

			dx1 = fabs(dx);
			dy1 = fabs(dy);
			px = 2 * dy1 - dx1;
			py = 2 * dx1 - dy1;
			


			if (dy1 <= dx1)
			{
				if (dx >= 0)
				{
					x = pt1.x;
					y = pt1.y;
					xe = pt2.x;
				}
				else
				{
					x = pt2.x;
					y = pt2.y;
					xe = pt1.x;
				}
				AddCell(x, y, nDirection);
				for (i = 0; x < xe; i++)
				{
					x = x + 1;
					if (px < 0)
					{
						px = px + 2 * dy1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							y = y + 1;
						}
						else
						{
							y = y - 1;
						}
						px = px + 2 * (dy1 - dx1);
					}
					AddCell(x, y, nDirection);
				}
			}
			else
			{
				if (dy >= 0)
				{
					x = pt1.x;
					y = pt1.y;
					ye = pt2.y;
				}
				else
				{
					x = pt2.x;
					y = pt2.y;
					ye = pt1.y;
				}
				AddCell(x, y, nDirection);
				for (i = 0; y < ye; i++)
				{
					y = y + 1;
					if (py <= 0)
					{
						py = py + 2 * dx1;
					}
					else
					{
						if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						{
							x = x + 1;
						}
						else
						{
							x = x - 1;
						}
						py = py + 2 * (dx1 - dy1);
					}
					AddCell(x, y, nDirection);
				}
			}
		}

		void CDrawPolygonOpenGL::AddEdge1(const GPoint& pt1, const GPoint& pt2)
		{

			GUnits dVectorProduct = pt1.x*pt2.y - pt2.x*pt1.y;

			int nDirection = dVectorProduct > 0 ? 1 : -1;

			bool steep = false;
			int x0 = pt1.x;
			int y0 = pt1.y;

			int x1 = pt2.x;
			int y1 = pt2.y;


			int dx = x1 - x0;
			int dy = y1 - y0;


			if (dy == 0)
			{
				AddCell(x0, y0, nDirection);
				AddCell(x1, y1, nDirection);
				return;
			}

		 
			if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
				std::swap(x0, y0);
				std::swap(x1, y1);
				steep = true;
			}
			if (x0 > x1) {
				std::swap(x0, x1);
				std::swap(y0, y1);
			}
 
			int derror2 = std::abs(dy) * 2;
			int error2 = 0;
			int y = y0;
			for (int x = x0; x <= x1; x++) 
			{
				if (steep) 
				{
					 
					AddCell( x, y, nDirection);
				}
				else 
				{
					AddCell(y, x, nDirection);
				}
				error2 += derror2;

				if (error2 > dx)
				{
					y += (y1 > y0 ? 1 : -1);
					error2 -= dx * 2;
				}
			}


		}


		void CDrawPolygonOpenGL::AddCell(int x, int y, int nDirection)
		{
			if (x < 0 || y < 0 || y > m_vecY.size() - 1)
				return;

			if (m_vecCells.empty())
			{
				m_vecCells.push_back(SCell(x, y, nDirection));
				m_vecY[y] += 1;
				return;
			}

			auto& cell = m_vecCells[m_vecCells.size() - 1];

			if (cell.m_x == x && cell.m_y == y)
			{
				cell.m_nDirection += nDirection;
				return;
			}

			m_vecY[y] += 1;
			m_vecCells.emplace_back(SCell(x, y, nDirection));
		}

		void CDrawPolygonOpenGL::SortCell()
		{
			
			//sort by y
			std::sort(m_vecCells.begin(), m_vecCells.end(), [](const SCell& left, const SCell& right) {
				if(left.m_y != right.m_y)
					return left.m_y < right.m_y; 
				return left.m_x < right.m_x;
			});
		}

		void CDrawPolygonOpenGL::Draw()
		{
			if (m_vecCells.empty())
				return;

			int nCell = 0;
			int nNumCell = m_vecCells.size();
			while (nCell < nNumCell)
			{
				auto& cell = m_vecCells[nCell];
				int nY0 = cell.m_y;
				int nXnums = m_vecY[nY0];
				if (nXnums == 1)
				{
					::glBegin(GL_LINES);
					::glVertex2f(cell.m_x + 0.5f, cell.m_y + 0.5f);
					::glVertex2f(cell.m_x + 0.5f, cell.m_y + 0.5f);
					::glEnd();
				}
				else
				{

					TCells drawCells;

					drawCells.push_back(cell);

					for (int i = 1; i < nXnums; ++i)
					{
						auto& cell1 = m_vecCells[nCell + i];

						auto& cell2 = drawCells[drawCells.size() - 1];
						if (cell2.isCoordEqual(cell1))
							cell2.m_nDirection += cell1.m_nDirection;
						else
							drawCells.push_back(cell1);
					}

					int nSum =0;
					for (int i = drawCells.size(); i > 0; --i)
					{

						int nPrev = i - 1 - 1;
						if(nPrev < 0)
							break;

						nSum += m_vecCells[nCell + i - 1].m_nDirection;
						if (nSum != 0)
						{
							auto& cell1 = drawCells[i - 1];
							auto& cell0 = drawCells[nPrev];
						
							::glBegin(GL_LINES);
							::glVertex2f(cell0.m_x + 0.5f, cell0.m_y + 0.5f);
							::glVertex2f(cell1.m_x + 0.5f, cell1.m_y + 0.5f);
							::glEnd();
						}

					}
				}
				nCell += nXnums;
			}

		}



	}

}