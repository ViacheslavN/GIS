#pragma once
#include "Display.h"
namespace GisEngine
{
	namespace Display
	{
		typedef std::vector<GPoint> TVecPoints;
		typedef std::vector<int> TVecParts;
		class ClipLine
		{
		public:
			ClipLine();
			~ClipLine();

			void SetClipBox(const GRect& clipBox);
			void SetPointDst(TVecPoints *pPoints, TVecParts *pParts);
			void BeginLine(bool bAllPointInBox);
			void AddVertex(const GPoint& pt, bool bAllPointInBox);
			void EndLine(bool bAllPointInBox);
		private:

			typedef int OutCode;

			const int INSIDE = 0; // 0000
			const int LEFT = 1;   // 0001
			const int RIGHT = 2;  // 0010
			const int BOTTOM = 4; // 0100
			const int TOP = 8;    // 1000

			OutCode GetOutCode(const GPoint& pt);
			void ClosePart();
		private:
			bool m_bFirst;
			OutCode m_nPrevCode;
			TVecPoints* m_pVecPoints;
			TVecParts* m_pVecParts;
			GRect  m_clipBox;
			GPoint m_prevPoint;
			uint32 m_nBeginPos;

		 

		};

	}
}