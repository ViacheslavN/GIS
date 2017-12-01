#pragma once
#include "Display.h"

namespace GisEngine
{
	namespace Display
	{

		class QuickEdgeClipper
		{

		private:
			QuickEdgeClipper();
			void initClipTable();

		public:
			enum PointType
			{
				OUTPOINT = 1,
				INPUT = 2
			};
			enum SideType
			{
				LEFT,
				RIGHT,
				BOTTOM,
				TOP
			};

			struct SPointInfo
			{
				int type;
				int side;
			};


			enum TypeRet
			{
				EdgeMiss = 0,
				EdgeInput = 1,
				EdgeInputVertex = 2,
				EdgeInner = 3,
				EdgeOutPut = 4,
				EdgeOutPutVertex = 5,
				EdgeThrough = 6
			};
			typedef int (QuickEdgeClipper::*ClipFunc)(GPoint, GPoint, GPoint&, SPointInfo *);

			QuickEdgeClipper(GRect& box);
			int initFirstPoint(GPoint Point);
			int getCurrentState();
			int clipNextPoint(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
		protected:
			static ClipFunc clipFuncTable[256];
			int currentState;
			int nextState;
			GUnits mimaxX[2];
			GUnits mimaxY[2];
			GUnits dsX[2];
			GUnits dsY[2];

			int sideX[2];
			int sideY[2];

			int getState(GPoint Point);
			int Center2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Center2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Center2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Middle2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Middle2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Middle2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Corner2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Corner2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
			int Corner2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt);
		};


	}

}


 