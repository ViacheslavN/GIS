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

				void Init(GRect clipBox, GPoint *pOut, uint32 nMaxCnt, uint32 *pPart, uint32 nMaxPart);
				void BeginPart();
				void AddEdge(const GPoint& pt1, const GPoint& pt2);
				void Clear();
			private:
				enum eCurrState
				{
					eEmpty = 0,
					eInEdge = 1,
					eInnerRect = 2,
					eOutEdge = 4,
					eOutRect = 8

				};



				 GRect m_clipBox;
				 bool m_blockWise;
				 bool m_bNoPointIntersect;
				 uint32 m_nParts;
				 uint32 m_nPoints;

				 double m_dSquare;

				 eCurrState m_currState;


				 GPoint *m_pOut;
				 uint32 m_nMaxCnt;
				 uint32 *m_pPart;
				 uint32 nMaxPart;


		};

	}
}