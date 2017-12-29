#pragma once
#include "Display.h"

namespace GisEngine
{
	namespace Display
	{
		
		class ClipPolygon
		{
			public:
				ClipPolygon();
				~ClipPolygon();

				void SetClipBox(const GRect& clipBox);
				void SetPointDst(TVecPoints *pPoints);

				void BeginPolygon();
				void AddVertex(const GPoint& pt, bool bAllPointInBox);
				void EndPolygon(bool bAllPointInBox);
			private:
				void Clear();

				// This template class is the workhorse of the algorithm. It handles the clipping against one boundary.
				// Boundary is either BoundaryHor or BoundaryVert, Stage is the next ClipStage, or the output stage.
				template <class Boundary, class Stage>
				class ClipStage : public Boundary
				{
				public:
					ClipStage(Stage& nextStage, GUnits position = 0)
						: Boundary(position)
						, m_NextStage(nextStage)
						, m_bFirst(true)
					{}
					void HandleVertex(const GPoint& pntCurrent)	// Function to handle one vertex
					{
						bool bCurrentInside = IsInside(pntCurrent);		// See if vertex is inside the boundary.

						if (m_bFirst)	// If this is the first vertex...
						{
							m_pntFirst = pntCurrent;	// ... just remember it,...

							m_bFirst = false;
						}
						else		// Common cases, not the first vertex.
						{
							if (bCurrentInside)	// If this vertex is inside...
							{
								if (!m_bPreviousInside)		// ... and the previous one was outside
									m_NextStage.HandleVertex(Intersect(m_pntPrevious, pntCurrent));
								// ... first output the intersection point.

								m_NextStage.HandleVertex(pntCurrent);	// Output the current vertex.
							}
							else if (m_bPreviousInside) // If this vertex is outside, and the previous one was inside...
								m_NextStage.HandleVertex(Intersect(m_pntPrevious, pntCurrent));
							// ... output the intersection point.

							// If neither current vertex nor the previous one are inside, output nothing.
						}
						m_pntPrevious = pntCurrent;		// Be prepared for next vertex.
						m_bPreviousInside = bCurrentInside;
					}
					void Finalize()
					{
						HandleVertex(m_pntFirst);		// Close the polygon.
						m_NextStage.Finalize();			// Delegate to the next stage.
						m_bFirst = true;
 					}
					void BeginPolygon()
					{
						m_NextStage.BeginPolygon();
						m_bFirst = true;
					}
					 
					Stage& m_NextStage;			// the next stage
					bool m_bFirst;				// true if no vertices have been handled
					GPoint m_pntFirst;			// the first vertex
					GPoint m_pntPrevious;		// the previous vertex
					bool m_bPreviousInside;		// true if the previous vertex was inside the Boundary
				};

				class OutputStage
				{
				public:
					OutputStage() : m_pDest(0), m_dS(0.){}
					void SetDestination(TVecPoints * pDest)
					{ 
						m_pDest = pDest;
					}
					void AddVertex(const GPoint& pnt)
					{
						m_pDest->push_back(pnt);
					}
					void HandleVertex(const GPoint& pnt) 
					{ 

						if (m_nBeginPolygonPos != m_pDest->size())
						{
							auto& ptLast = (*m_pDest)[m_pDest->size() - 1];
							m_dS += (pnt.y + ptLast.y) * (pnt.x - ptLast.x);
						}

						m_pDest->push_back(pnt);
						


					}	// Append the vertex to the output container.
					void Finalize() 
					{
						
					}		// Do nothing.


					bool isClockwise()
					{
						return m_dS >= 0;
					}

					void BeginPolygon()
					{
						m_nBeginPolygonPos = m_pDest->size();
					}

					void CheckOut(bool bisInClockwise)
					{
					
						uint32 nEndPos = m_pDest->size();
						if (m_nBeginPolygonPos != nEndPos)
						{
							auto &ptFirst = (*m_pDest)[m_nBeginPolygonPos];
							auto &ptLast = (*m_pDest)[nEndPos - 1];

							if (ptLast != ptFirst)
							{
								m_dS += (ptFirst.y + ptLast.y) * (ptFirst.x - ptLast.x);
							}

							bool bisOutClockwise = isClockwise();
							if (bisInClockwise != bisOutClockwise)
							{
								uint32 nSize = nEndPos - m_nBeginPolygonPos;
								for (int i = 0; i < nSize / 2; i++)
								{
									GPoint tmp = (*m_pDest)[m_nBeginPolygonPos + i];
									(*m_pDest)[i] = (*m_pDest)[nEndPos - 1 - i];
									(*m_pDest)[i - 1] = tmp;
								}
							}
						}						

					
						m_dS = 0.;
					}

				private:
					TVecPoints * m_pDest;
					double m_dS;
					uint32 m_nBeginPolygonPos;
				};


				// Implementation of a horizontal boundary (top or bottom).
				// Comp is a std::binary_function object, comparing its two parameters, f.i. std::less.
				template <class Comp>
				class BoundaryHor
				{
				public:
					BoundaryHor(GUnits y = 0) : m_Y(y) {}
					bool IsInside(const GPoint& pnt) const 
					{
						return Comp()(pnt.y, m_Y);
					}	// return true if pnt.Y is at the inside of the boundary
					GPoint Intersect(const GPoint& p0, const GPoint& p1) const			// return intersection point of line p0...p1 with boundary
					{																	// assumes p0...p1 is not strictly horizontal

						GPoint d = p1 - p0;
						GUnits xslope = d.x / d.y;

						GPoint r;
						r.y = m_Y;
						r.x = p0.x + xslope * (m_Y - p0.y);
						return r;
					}
					void SetPoint(GUnits y) { m_Y = y; };
				private:
					GUnits m_Y;
				};

				// Implementation of a vertical boundary (left or right).
				template <class Comp>
				class BoundaryVert
				{
				public:
					BoundaryVert(GUnits x = 0) : m_X(x) {}
					bool IsInside(const GPoint& pnt) const
					{
						return Comp()(pnt.x, m_X); 
					}
					GPoint Intersect(const GPoint& p0, const GPoint& p1) const		// assumes p0...p1 is not strictly vertical
					{

						GPoint d = p1 - p0;
						GUnits yslope = d.y / d.x;

						GPoint r;
						r.x = m_X;
						r.y = p0.y + yslope * (m_X - p0.x);
						return r;
					}
					void SetPoint(GUnits x) { m_X = x; }

				private:
					GUnits m_X;
				};

			private:
 

				// These typedefs define the four boundaries. In keeping up with the GDI/GDI+ interpretation of
				// rectangles, we include the left and top boundaries, but not the right and bottom boundaries.
				// In other words: a vertex on the left boundary is considered to be inside, but a vertex
				// on the right boundary is considered to be outside.
				typedef BoundaryVert<std::less<GUnits> >				BoundaryRight;
				typedef BoundaryHor<std::greater_equal<GUnits> >		BoundaryTop;
				typedef BoundaryVert<std::greater_equal<GUnits> >		BoundaryLeft;
				typedef BoundaryHor<std::less<GUnits> >				BoundaryBottom;

				// Next typedefs define the four stages. First template parameter is the boundary,
				// second template parameter is the next stage.
				typedef ClipStage<BoundaryBottom, OutputStage>	ClipBottom;
				typedef ClipStage<BoundaryLeft, ClipBottom>		ClipLeft;
				typedef ClipStage<BoundaryTop, ClipLeft>		ClipTop;
				typedef ClipStage<BoundaryRight, ClipTop>		ClipRight;

				// Our data members.
				OutputStage m_stageOut;
				ClipBottom m_stageBottom;
				ClipLeft m_stageLeft;
				ClipTop m_stageTop;
				ClipRight m_stageRight;

				double m_dS;
				GPoint m_nPrev;
				bool m_bFirst;

		};

	}
}