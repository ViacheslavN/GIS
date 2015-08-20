#ifndef GIS_ENGINE_DISPLAY_DISPLAY_TYPES_H
#define GIS_ENGINE_DISPLAY_DISPLAY_TYPES_H

#include "GraphTypes.h"

namespace GisEngine
{

	namespace Display
	{

		enum eDisplayCacheType
		{
			DisplayCacheDefault    = 0,
			DisplayCacheGeography  = 1,
			DisplayCacheAnnotation = 2,
			DisplayCacheSelection  = 3
		};

		enum  eSimpleLineStyle
		{  
			SimpleLineStyleSolid = 0,// The line is solid. 
			SimpleLineStyleDash = 1,// The line is dashed -------. 
			SimpleLineStyleDot = 2,// The line is dotted ....... 
			SimpleLineStyleDashDot = 3,// The line has alternating dashes and dots _._._._. 
			SimpleLineStyleDashDotDot = 4,// The line has alternating dashes and double dots _.._.._. 
			SimpleLineStyleNull = 5 // Null line 
		};

		enum eSimpleFillStyle
		{
			SimpleFillStyleSolid            = 0,
			SimpleFillStyleNull             = 1,  
			SimpleFillStyleHorizontal       = 2,
			SimpleFillStyleVertical         = 3,
			SimpleFillStyleForwardDiagonal  = 4,
			SimpleFillStyleBackwardDiagonal = 5,
			SimpleFillStyleCross            = 6,
			SimpleFillStyleDiagonalCross    = 7
		};

		enum eMarkerFillStyle
		{
			MarkerFillStyleGrid            = 0,
			MarkerFillStyleRandom          = 1
		};


		enum eSimpleMarkerStyle
		{
			SimpleMarkerStyleCircle  = 0,
			SimpleMarkerStyleSquare  = 1,
			SimpleMarkerStyleCross   = 2, 
			SimpleMarkerStyleX       = 3,
			SimpleMarkerStyleDiamond = 4
		};

		enum eArrowMarkerStyle
		{
			ArrowMarkerStylePlain    = 0,
			ArrowMarkerStylePosition = 1
		};

		enum eDisplayTransformationPreserve
		{
			DisplayTransformationPreserveScale = 0,
			DisplayTransformationPreserveCenterExtent = 1
		};

	}

}

#endif