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


				for (int j = 1; j < nParts; ++j)
				{

				}		 

				offcet += nParts;
			}

			SortCell();
			Draw(lpPoints);

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
		void CDrawPolygonOpenGL::AddEdge1(const GPoint& pt1, const GPoint& pt2, int idx1, int idx2)
		{

			int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
			dx = pt2.x - pt1.x;
			dy = pt2.y - pt1.y;
			GUnits dVectorProduct = pt1.x*pt2.y - pt2.x*pt1.y;
			int nDirection = dVectorProduct > 0 ? 1 : -1;
			/*if (dy == 0)
			{
				AddCell(pt1.x, pt1.y, idx1, idx2);
				AddCell(pt2.x, pt2.y, idx1, idx2);
				return;
			}*/

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
				AddCell(x, y, idx1, idx2);
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
					AddCell(x, y, idx1, idx2);
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
				AddCell(x, y, idx1, idx2);
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
					AddCell(x, y, idx1, idx2);
				}
			}
		}

		void CDrawPolygonOpenGL::AddEdge(const GPoint& pt1, const GPoint& pt2, int idx1, int idx2)
		{

		//	GUnits dVectorProduct = pt1.x*pt2.y - pt2.x*pt1.y;

		//	int nDirection = dVectorProduct >= 0 ? 1 : -1;

			bool steep = false;
			int x1 = pt1.x;
			int y1 = pt1.y;

			int x2 = pt2.x;
			int y2 = pt2.y;


			int dx = abs(x2 - x1);
			int dy = abs(y2- y1);


		/*	if (dy == 0)
			{
				AddCell(x1, y1, idx1, idx2);
				AddCell(x2, y2, idx1, idx2);
				return;
			}
			*/

			int sx = x2 >= x1 ? 1 : -1;
			int sy = y2 >= y1 ? 1 : -1;
			if (dy <= dx)
			{
				int derr = (dy << 1) - dx;
				int dS = dy << 1;
				int  dD = (dy - dx) << 1;
				AddCell(x1, y1, idx1, idx2);
				for (int x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx)
				{

					if (derr > 0)
					{
						derr += dD;
						y += sy;
					}
					else
					{
						derr += dS;
					}
					AddCell(x, y, idx1, idx2);
				}


			}
			else
			{
				int derr = (dx << 1) - dy;
				int dS = dx << 1;
				int  dD = (dx - dy) << 1;
				AddCell(x1, y1, idx1, idx2);

				for (int x = x1, y = y1 + sy, i = 1; i < dy; ++i, y += sy)
				{
					if (derr > 0)
					{

						derr += dD;
						x += sx;
					}
					else
					{
						derr += dS;
					}

					AddCell(x, y, idx1, idx2);
				}
			}

		 
		/*	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
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
			*/

		}


		void CDrawPolygonOpenGL::AddCell(int x, int y, int idx1, int idx2)
		{
			if (x < 0 || y < 0 || y > m_vecY.size() - 1)
				return;

			if (m_vecCells.empty())
			{
				m_vecCells.push_back(SCell(x, y, idx1, idx2));
				m_vecY[y] += 1;
				return;
			}

			auto& cell = m_vecCells[m_vecCells.size() - 1];

		/*	if (cell.m_x == x && cell.m_y == y)
			{
				cell.m_nDirection += nDirection;
				return;
			}
			*/
			m_vecY[y] += 1;
			m_vecCells.push_back(SCell(x, y, idx1, idx2));
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

		void CDrawPolygonOpenGL::Draw(const GPoint* lpPoints)
		{


			if (m_vecCells.empty())
				return;

			int nCell = 0;
			int nNumCell = m_vecCells.size();
			TLines lines;

			int nScanY = m_vecCells[0].m_y;

		/*	while (nCell < nNumCell)
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
					nCell += 1;
					continue;
				}


				int nSumLeftRight = 0;
				lines.clear();



				for (int i = 0; i < nXnums;)
				{

					auto& cell0 = m_vecCells[nCell + i];
					
					while (i < nXnums)
					{
						i += 1;
						auto& cellX = m_vecCells[nCell + i];
						if(cellX.m_x != cell0.m_x)
							break;
					}
					
					auto& cell1 = m_vecCells[nCell + i];

					auto pt1 = lpPoints[cell0.n_idx1];
					auto pt2 = lpPoints[cell0.n_idx2];


					int nLeftRight = (cell1.m_x - pt1.x) * (pt2.y - pt1.y) - (nScanY - pt1.y) * (pt2.x - pt1.x) > 0 ? 1 : -1;

					nSumLeftRight += nLeftRight;
					if(nSumLeftRight == 0)
						continue;

					if (lines.empty())
					{
						lines.push_back(SLine(cell0.m_x));
					}
					else
					{
						SLine& sLine = lines.back();
						if (cell0.m_x == sLine.m_x + 1)
							sLine.m_len += 1;
						else
						{
							lines.push_back(SLine(cell0.m_x));
						}

					}

				}


				for (size_t l = 0; l < lines.size(); ++l)
				{

					::glBegin(GL_LINES);
					::glVertex2f(lines[l].m_x + 0.5f, nScanY + 0.5f);
					::glVertex2f(lines[l].m_x + lines[l].m_len+ 0.5f, nScanY + 0.5f);
					::glEnd();


				}

				lines.clear();
				nCell += nXnums;
			}


			return;*/

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
					int nSumLeftRight = 0;
					for (int i = 1; i < nXnums; ++i)
					{
						auto& cell1 = m_vecCells[nCell + i];
						auto& cell0 = m_vecCells[nCell + i - 1];

						auto pt1 = lpPoints[cell0.n_idx1];
						auto pt2 = lpPoints[cell0.n_idx2];


						int nLeftRight = (cell1.m_x - pt1.x) * (pt2.y - pt1.y) - (nY0 - pt1.y) * (pt2.x - pt1.x) > 0 ? 1 : -1;

						nSumLeftRight += nLeftRight;
						if (nSumLeftRight != 0)
						{
		

							::glBegin(GL_LINES);
							::glVertex2f(cell0.m_x + 0.5f, cell0.m_y + 0.5f);
							::glVertex2f(cell1.m_x + 0.5f, cell1.m_y + 0.5f);
							::glEnd();
						}


					}

					/*TCells drawCells;

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

					if (drawCells.size() == 1)
					{
						::glBegin(GL_LINES);
						::glVertex2f(drawCells[0].m_x + 0.5f, drawCells[0].m_y + 0.5f);
						::glVertex2f(drawCells[0].m_x + 0.5f, drawCells[0].m_y + 0.5f);
						::glEnd();
					}
					else
					{
						int nSum = 0;
						for (int i = 1; i < drawCells.size(); ++i)
						{
							if (drawCells[i - 1].m_nDirection < -1)
								drawCells[i - 1].m_nDirection = -1;
							else if(drawCells[i - 1].m_nDirection > 1)
								drawCells[i - 1].m_nDirection = 1;


							nSum += drawCells[i - 1].m_nDirection;
							if (nSum != 0)
							{
								auto& cell1 = drawCells[i];
								auto& cell0 = drawCells[i - 1];

								::glBegin(GL_LINES);
								::glVertex2f(cell0.m_x + 0.5f, cell0.m_y + 0.5f);
								::glVertex2f(cell1.m_x + 0.5f, cell1.m_y + 0.5f);
								::glEnd();
							}

						}
					}*/
					
				}
				nCell += nXnums;
			}

		}



	}

}