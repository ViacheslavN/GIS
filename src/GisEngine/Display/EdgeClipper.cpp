#include "stdafx.h"
#include "RectClipper.h"
#include "EdgeClipper.h"
namespace GisEngine
{
	namespace Display
	{



		//5   4   6
		//1   0   2
		//9   8   10
 
		QuickEdgeClipper::ClipFunc QuickEdgeClipper::clipFuncTable[256];


		/*int64 getYIntersect(int64 *firstPoint, int64 *nextPoint, int64 X)
		{
			assert(nextPoint[0] != firstPoint[0]);
			int64 dx = nextPoint[0] - firstPoint[0];
			return firstPoint[1] + ((X / 10000 - firstPoint[0] / 10000)* (nextPoint[1] / 10000 - firstPoint[1] / 10000) / dx) * 10000;
		}
		int64 getXIntersect(int64 *firstPoint, int64 *nextPoint, int64 Y)
		{
			assert(nextPoint[1] != firstPoint[1]);
			int64 dy = nextPoint[1] - firstPoint[1];
			return firstPoint[0] + ((Y / 10000 - firstPoint[1] / 10000) * (nextPoint[0] / 10000 - firstPoint[0] / 10000) / dy) * 10000;
		}
		*/

		GUnits getYIntersect(GPoint firstPoint, GPoint nextPoint, GUnits X)
		{
			assert(nextPoint != firstPoint);
			GUnits dx = nextPoint.x - firstPoint.x;
			return firstPoint.y + ((X - firstPoint.x )* (nextPoint.y  - firstPoint.y ) / dx) ;
		}
		GUnits getXIntersect(GPoint firstPoint, GPoint nextPoint, GUnits Y)
		{
			assert(nextPoint != firstPoint);
			GUnits dy = nextPoint.y - firstPoint.y;
			return firstPoint.x + ((Y  - firstPoint.y ) * (nextPoint.x  - firstPoint.x) / dy);
		}


		QuickEdgeClipper::QuickEdgeClipper()
		{
			initClipTable();
		}
		void QuickEdgeClipper::initClipTable()
		{
			memset(clipFuncTable, 0, sizeof(clipFuncTable));
			ClipFunc funcMap[] = {
				&QuickEdgeClipper::Center2Center,&QuickEdgeClipper::Center2Middle,&QuickEdgeClipper::Center2Corner,
				&QuickEdgeClipper::Middle2Center,&QuickEdgeClipper::Middle2Middle,&QuickEdgeClipper::Middle2Corner,
				&QuickEdgeClipper::Corner2Center,&QuickEdgeClipper::Corner2Middle,&QuickEdgeClipper::Corner2Corner,
			};
			for (int i = 0; i < 11; i++)
			{//from
				if ((i & 3) == 3)
					continue;
				int fromIdx = 0;
				if (i)
				{
					if ((i & 3) && (i & 0xc))//corner
						fromIdx = 2;
					else//middle
						fromIdx = 1;
				}
				fromIdx *= 3;
				int tabIdx = i << 4;
				for (int j = 0; j < 11; j++, tabIdx++)
				{//to
					if ((j & 3) == 3)
						continue;
					int toIdx = 0;
					if (j)
					{
						if ((j & 3) && (j & 0xc))//corner
							toIdx = 2;
						else//middle
							toIdx = 1;
					}
					clipFuncTable[tabIdx] = funcMap[fromIdx + toIdx];
				}
			}
		}
		QuickEdgeClipper::QuickEdgeClipper(GRect& box)
		{
			mimaxX[0] = box.xMin;
			mimaxX[1] = box.xMax;
			mimaxY[0] = box.yMin;
			mimaxY[1] = box.yMax;

			GUnits Xlen = mimaxX[1] - mimaxX[0];
			GUnits Ylen = mimaxY[1] - mimaxY[0];

			dsX[0] = 2 * Xlen + Ylen;
			dsX[1] = Xlen;

			dsY[0] = Xlen + Ylen;
			dsY[1] = 0;

			sideX[0] = LEFT;
			sideX[1] = RIGHT;
			sideY[0] = BOTTOM;
			sideY[1] = TOP;

		}
		int QuickEdgeClipper::initFirstPoint(GPoint pPoint)
		{
			currentState = getState(pPoint);
			return currentState;
		}
		int QuickEdgeClipper::getCurrentState()
		{
			return currentState;
		}
		int QuickEdgeClipper::clipNextPoint(GPoint prevPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			nextState = getState(nextPoint);
			if (currentState & nextState)
			{
				currentState = nextState;
				return EdgeMiss;
			}
			ClipFunc curClipFunc = clipFuncTable[(currentState << 4) + nextState];
			assert(curClipFunc != 0);
			int retval = (this->*curClipFunc)(prevPoint, nextPoint, result, pt);
			currentState = nextState;
			return retval;
		}

		int QuickEdgeClipper::getState(GPoint pPoint)
		{
			int result = 0;
			if (pPoint.x < mimaxX[0])
				result = 1;
			else if (pPoint.x > mimaxX[1])
				result = 2;
			if (pPoint.y < mimaxY[0])
				result += 4;
			else if (pPoint.y > mimaxY[1])
				result += 8;
			return result;
		}
		int QuickEdgeClipper::Center2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo * pt)
		{

			result.x = nextPoint.x;
			result.y = nextPoint.y;

			return EdgeInner;
		}
		int QuickEdgeClipper::Center2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			GUnits X, Y, dst;
			int ns3 = nextState & 3;
			int index;
			int side;
			if (ns3)
			{
				index = ns3 - 1;
				X = mimaxX[index];

				//dst = dsX[index];
				side = sideX[index];
				if (firstPoint.x == X)
				{
					pt->type = OUTPOINT;
					pt->side = side;
					result.x = firstPoint.x;
					result.y = firstPoint.y;
					return EdgeOutPutVertex;
				}
				Y = getYIntersect(firstPoint, nextPoint, X);
				//dst += _abs64(Y - mimaxY[index]);
			}
			else
			{
				index = (nextState >> 2) - 1;
				Y = mimaxY[index];
			//	dst = dsY[index];
				side = sideY[index];
				if (firstPoint.y == Y)
				{
					pt->type = OUTPOINT;
					//pt->distance = dst + _abs64(firstPoint[0] - mimaxX[!index]);
					pt->side = side;
					result.x = firstPoint.x;
					result.y = firstPoint.y;
					return EdgeOutPutVertex;
				}
				X = getXIntersect(firstPoint, nextPoint, Y);
				//dst += _abs64(X - mimaxX[!index]);
			}
			result.x = X; // Точка выхода
			result.y = Y;
			pt->type = OUTPOINT;
			//pt->distance = dst;
			pt->side = side;

			return EdgeOutPut;
		}
		int QuickEdgeClipper::Center2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			int xIdx = (nextState & 3) - 1;
			int yIdx = (nextState >> 2) - 1;
			GUnits X = mimaxX[xIdx];
			

			if (firstPoint.x == X)
			{
				return EdgeMiss;
			}
			//GUnits dst = dsX[xIdx];
			GUnits Y = getYIntersect(firstPoint, nextPoint, X);
			//dst += _abs64(Y - mimaxY[xIdx]);

			pt->side = sideX[xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				Y = mimaxY[yIdx];
				if (firstPoint.y == Y)
				{
					return EdgeMiss;
				}
				X = getXIntersect(firstPoint, nextPoint, Y);
				//dst = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				pt->side = sideY[yIdx];
			}
			pt->type = OUTPOINT;
			//pt->distance = dst;

			result.x = X;
			result.y = Y;// Точка выхода
			return EdgeOutPut;
		}
		int QuickEdgeClipper::Middle2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			GUnits X, Y;
			int ns3 = currentState & 3;
			GUnits dst = 0;
			int index;
			int side;
			if (ns3)
			{
				index = ns3 - 1;
				X = mimaxX[index];
				side = sideX[index];
				if (nextPoint[0] == X)
				{
					result[0].x = nextPoint[0];
					result[0].y = nextPoint[1];

					pt->type = INPOINT;
					pt->distance = dsX[index] + _abs64(nextPoint[1] - mimaxY[index]);
					pt->side = side;

					return EdgeInputVertex;
				}
				Y = getYIntersect(firstPoint, nextPoint, X);

				dst = dsX[index] + _abs64(Y - mimaxY[index]);
			}
			else
			{
				index = (currentState >> 2) - 1;
				Y = mimaxY[index];
				side = sideY[index];
				if (nextPoint[1] == Y)
				{
					result[0].x = nextPoint[0];
					result[0].y = nextPoint[1];

					pt->type = INPOINT;
					pt->distance = dsY[index] + _abs64(nextPoint[0] - mimaxX[!index]);
					pt->side = side;
					return EdgeInputVertex;
				}
				X = getXIntersect(firstPoint, nextPoint, Y);
				dst = dsY[index] + _abs64(X - mimaxX[!index]);
			}
			result->x = X;// Точка входа
			result->y = Y;
			result[1].x = nextPoint[0];
			result[1].y = nextPoint[1];

			pt->type = INPOINT;
			pt->distance = dst;
			pt->side = side;

			return EdgeInput;
		}
		int QuickEdgeClipper::Middle2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			GUnits X, Y;
			int mix = currentState | nextState;
			int index;
			if ((mix & 3) == 3)
			{
				int ns3 = currentState & 3;
				index = ns3 - 1;
				X = mimaxX[index];
				Y = getYIntersect(firstPoint, nextPoint, X);
				result->x = X;
				result->y = Y; // Точка входа

				pt[0].type = INPOINT;
				pt[0].distance = dsX[index] + _abs64(Y - mimaxY[index]);
				pt[0].side = sideX[index];


				index = 2 - ns3;
				X = mimaxX[index];
				Y = getYIntersect(firstPoint, nextPoint, X);
				result++;
				result->x = X;
				result->y = Y;  //Точка выхода

				pt[1].type = OUTPOINT;
				pt[1].distance = dsX[index] + _abs64(Y - mimaxY[index]);
				pt[1].side = sideX[index];

				return EdgeThrough;
			}
			if ((mix & 0xc) == 0xc)
			{
				int ns3 = currentState >> 2;
				index = ns3 - 1;
				Y = mimaxY[ns3 - 1];
				X = getXIntersect(firstPoint, nextPoint, Y);
				result->x = X;
				result->y = Y;// Точка входа

				pt[0].type = INPOINT;
				pt[0].distance = dsY[index] + _abs64(X - mimaxX[!index]);
				pt[0].side = sideY[index];

				index = 2 - ns3;
				Y = mimaxY[index];
				X = getXIntersect(firstPoint, nextPoint, Y);

				pt[1].type = OUTPOINT;
				pt[1].distance = dsY[index] + _abs64(X - mimaxX[!index]);
				pt[1].side = sideY[index];


				result++;
				result->x = X;
				result->y = Y;// Точка выхода
				return EdgeThrough;
			}
			int xIdx, yIdx;
			if (currentState & 3)
			{
				xIdx = (currentState & 3) - 1;
				yIdx = (nextState >> 2) - 1;
				X = mimaxX[xIdx];
				Y = getYIntersect(firstPoint, nextPoint, X);
				if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
				{
					return EdgeMiss;
				}

				pt[0].type = INPOINT;
				pt[0].distance = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
				pt[0].side = sideX[xIdx];

				result->x = X;
				result->y = Y;// Точка входа

				Y = mimaxY[yIdx];
				X = getXIntersect(firstPoint, nextPoint, Y);

				pt[1].type = OUTPOINT;
				pt[1].distance = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				pt[1].side = sideY[yIdx];

			}
			else
			{
				xIdx = (nextState & 3) - 1;
				yIdx = (currentState >> 2) - 1;
				Y = mimaxY[yIdx];
				X = getXIntersect(firstPoint, nextPoint, Y);
				if ((X < mimaxX[0]) || (X > mimaxX[1]))
				{
					return EdgeMiss;
				}
				result->x = X;
				result->y = Y;// Точка входа

				pt[0].type = INPOINT;
				pt[0].distance = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				pt[0].side = sideY[yIdx];

				X = mimaxX[xIdx];
				Y = getYIntersect(firstPoint, nextPoint, X);

				pt[1].type = OUTPOINT;
				pt[1].distance = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
				pt[1].side = sideX[xIdx];
			}
			result++;
			result->x = X;// Точка выхода
			result->y = Y;
			//Можно здесь все проставить
			return EdgeThrough;
		}
		int QuickEdgeClipper::Middle2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			GUnits X, Y;
			int mix = currentState | nextState;
			int index;
			GUnits dst;
			int side;
			if ((mix & 3) == 3)
			{
				int ns3 = currentState & 3;
				index = ns3 - 1;
				X = mimaxX[index];
				Y = getYIntersect(firstPoint, nextPoint, X);
				if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
				{
					return EdgeMiss;
				}

				dst = dsX[index] + _abs64(Y - mimaxY[index]);
				side = sideX[index];
			}
			else
			{
				int ns3 = currentState >> 2;
				index = ns3 - 1;
				Y = mimaxY[index];
				X = getXIntersect(firstPoint, nextPoint, Y);
				if ((X < mimaxX[0]) || (X > mimaxX[1]))
				{
					return EdgeMiss;
				}
				dst = dsY[index] + _abs64(X - mimaxX[!index]);
				side = sideY[index];

			}
			result->x = X;
			result->y = Y;// Точка входа
			result++;

			pt->type = INPOINT;
			pt->distance = dst;
			pt->side = side;
			pt++;

			int xIdx = (nextState & 3) - 1;
			int yIdx = (nextState >> 2) - 1;

			X = mimaxX[xIdx];
			Y = getYIntersect(firstPoint, nextPoint, X);

			dst = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
			side = sideX[xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				Y = mimaxY[yIdx];
				X = getYIntersect(firstPoint, nextPoint, Y);

				dst = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				side = sideY[yIdx];
			}


			pt->type = OUTPOINT;
			pt->distance = dst;
			pt->side = side;

			result->x = X;
			result->y = Y;// Точка выхода
						  //Можно здесь все проставить
			return EdgeThrough;
		}
		int QuickEdgeClipper::Corner2Center(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			int xIdx = (currentState & 3) - 1;
			GUnits X = mimaxX[xIdx];
			GUnits Y = getYIntersect(firstPoint, nextPoint, X);

			GUnits dst;
			int side;


			dst = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
			side = sideX[xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				int yIdx = (currentState >> 2) - 1;
				Y = mimaxY[yIdx];
				X = getXIntersect(firstPoint, nextPoint, Y);

				dst = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				side = sideY[yIdx];
			}
			if ((X != nextPoint[0]) || (Y != nextPoint[1]))
			{
				result->x = X;
				result->y = Y;// Точка входа

				pt->type = INPOINT;
				pt->distance = dst;
				pt->side = side;

				//result[1] = *nextPoint;

				result[1].x = nextPoint[0];
				result[1].y = nextPoint[1];
				return EdgeInput;
			}
			//  *result=*nextPoint;// Точка выхода
			pt->type = OUTPOINT;



			if (nextPoint[1] == mimaxY[0])
			{
				pt->distance = dsY[0] + mimaxX[1] - nextPoint[0];
				pt->side = sideY[0];
			}
			else if (nextPoint[1] == mimaxY[1])
			{
				pt->distance = dsY[1] + nextPoint[0] - mimaxX[0];
				pt->side = sideY[1];
			}
			else
			{
				assert(0);
			}

			result[0].x = nextPoint[0];
			result[0].y = nextPoint[1];
			return EdgeOutPut;
		}
		int QuickEdgeClipper::Corner2Middle(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			int xIdx = (currentState & 3) - 1;
			int yIdx = (currentState >> 2) - 1;
			GUnits X = mimaxX[xIdx];
			GUnits Y = getYIntersect(firstPoint, nextPoint, X);

			GUnits dst;
			int side;


			dst = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
			side = sideX[xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				Y = mimaxY[yIdx];
				X = getYIntersect(firstPoint, nextPoint, Y);
				if ((X < mimaxX[0]) || (X > mimaxX[1]))
				{
					return EdgeMiss;
				}

				dst = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				side = sideY[yIdx];
			}

			pt->type = INPOINT;
			pt->distance = dst;
			pt->side = side;
			pt++;

			result->x = X;
			result->y = Y;
			result++;

			if (nextState & 3)
			{
				X = mimaxX[1 - xIdx];
				Y = getYIntersect(firstPoint, nextPoint, X);

				dst = dsX[1 - xIdx] + _abs64(Y - mimaxY[1 - xIdx]);
				side = sideX[1 - xIdx];

			}
			else
			{
				int index = 1 - yIdx;
				Y = mimaxY[index];
				X = getXIntersect(firstPoint, nextPoint, Y);

				dst = dsY[index] + _abs64(X - mimaxX[!index]);
				side = sideY[1 - yIdx];

			}

			pt->type = OUTPOINT;
			pt->distance = dst;
			pt->side = side;


			result->x = X;
			result->y = Y;
			return EdgeThrough;
		}
		int QuickEdgeClipper::Corner2Corner(GPoint firstPoint, GPoint nextPoint, GPoint& result, SPointInfo *pt)
		{
			int xIdx = (currentState & 3) - 1;
			int yIdx = (currentState >> 2) - 1;
			GUnits X = mimaxX[xIdx];
			GUnits Y = getYIntersect(firstPoint, nextPoint, X);

			GUnits dst;
			int side;

			dst = dsX[xIdx] + _abs64(Y - mimaxY[xIdx]);
			side = sideX[xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				Y = mimaxY[yIdx];
				X = getXIntersect(firstPoint, nextPoint, Y);

				if ((X < mimaxX[0]) || (X > mimaxX[1]))
				{
					return EdgeMiss;
				}

				dst = dsY[yIdx] + _abs64(X - mimaxX[!yIdx]);
				side = sideY[yIdx];
			}
			result->x = X; // Точка входа
			result->y = Y;
			result++;

			pt->type = INPOINT;
			pt->distance = dst;
			pt->side = side;

			X = mimaxX[1 - xIdx];
			Y = getYIntersect(firstPoint, nextPoint, X);


			dst = dsX[1 - xIdx] + _abs64(Y - mimaxY[1 - xIdx]);
			side = sideX[1 - xIdx];

			if ((Y < mimaxY[0]) || (Y > mimaxY[1]))
			{
				int index = 1 - yIdx;
				Y = mimaxY[index];
				X = getXIntersect(firstPoint, nextPoint, Y);


				dst = dsY[index] + _abs64(X - mimaxX[!index]);
				side = sideY[1 - yIdx];

			}
			pt++;
			pt->type = OUTPOINT;
			pt->distance = dst;
			pt->side = side;

			result->x = X; //Точка выхода
			result->y = Y;
			return EdgeThrough;
		}


		 
	}
}

