#ifndef GIS_ENGINE_DISPLAY_GRAPHICS_TYPES_H_
#define GIS_ENGINE_DISPLAY_GRAPHICS_TYPES_H_

#include "DisplayMath.h"
#define _FLOAT_GUNITS_
namespace GisEngine
{
	namespace Display
	{
		enum DeviceType
		{
			DeviceTypeDisplay  = 0,
			DeviceTypePrinter  = 1,
			DeviceTypePlotter  = 2,
			DeviceTypeMetafile = 3
		};

		enum GradientType
		{
			GradientTypeBdiagonal  = 0,
			GradientTypeFdiagonal  = 1,
			GradientTypeHorizontal = 2,
			GradientTypeVertical   = 3
		};

		enum PenType
		{
			PenTypeSolid       = 0,
			PenTypeDash        = 1,
			PenTypeDot         = 2,
			PenTypeDashDot     = 3,
			PenTypeDashDotDot  = 4,
			PenTypeNull        = 5
		};

		enum CapType
		{
			CapTypeButt      = 0,
			CapTypeRound     = 1,
			CapTypeSquare    = 2
		};

		enum JoinType
		{
			JoinTypeMiter     = 0,
			JoinTypeRound     = 1,
			JoinTypeBevel     = 2
		};

		enum BrushType
		{
			BrushTypeSolid     = 0,
			BrushTypeNull      = 1,
			BrushTypeHatched   = 2,
			BrushTypeTextured  = 3,
			BrushTypeGradientHorizontal = 4,
			BrushTypeGradientVertical = 5
		};

		
		enum FontStyle
		{ 
			FontStyleRegular    = 0,
			FontStyleBold       = 1,
			FontStyleItalic     = 2,
			FontStyleUnderline  = 4
		};


		enum TextVAlignment
		{
			TextVAlignmentTop      = 0,
			TextVAlignmentCenter   = 1,
			TextVAlignmentBaseline = 2,
			TextVAlignmentBottom   = 3
		};

		enum TextHAlignment
		{
			TextHAlignmentLeft     = 0,
			TextHAlignmentCenter   = 1,
			TextHAlignmentRight    = 2
		};

		enum TextDraw
		{
			TextDrawHaloOnly = 1,
			TextDrawTextOnly = 2,
			TextDrawAll = 3
		};

		enum BitmapFormatType
		{
			BitmapFormatType1bpp           = 0,  //Specifies that the format is 1 bit per pixel, indexed.
			BitmapFormatType4bpp           = 1,  //Specifies that the format is 4 bit per pixel, indexed.
			BitmapFormatType8bpp           = 2,  //Specifies that the format is 8 bit per pixel, indexed.
			BitmapFormatType16bppARGB1555  = 3,  //Specifies that the format is 16 bits per pixel; 1 bit is used for the alpha component, and 5 bits each are used for the red, green, and blue components.
			BitmapFormatType16bppGrayScale = 4, //Specifies that the format is 16 bits per pixel, grayscale.
			BitmapFormatType16bppRGB555    = 5, //Specifies that the format is 16 bits per pixel; 5 bits each are used for the red, green, and blue components. The remaining bit is not used.
			BitmapFormatType16bppRGB565    = 6, //Specifies that the format is 16 bits per pixel; 5 bits are used for the red component, 6 bits are used for the green component, and 5 bits are used for the blue component.
			BitmapFormatType24bppRGB       = 7, //Specifies that the format is 24 bits per pixel; 8 bits each are used for the red, green, and blue components.
			BitmapFormatType32bppARGB      = 8, //Specifies that the format is 32 bits per pixel; 8 bits each are used for the alpha, red, green, and blue components.
		};


#ifdef _FLOAT_GUNITS_
		typedef float GUnits;
#else
		typedef int GUnits;
#endif


	}
}

#endif