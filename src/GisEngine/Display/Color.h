#ifndef GIS_ENGINE_DISPLAY_GRAPHICS_TYPES_H_
#define GIS_ENGINE_DISPLAY_GRAPHICS_TYPES_H_
#include "GraphTypes.h"
#include "CommonLibrary/FixedMemoryStream.h"
#include "CommonLibrary/GeneralTypes.h"
#include "CommonLibrary/str_t.h"
namespace GisEngine
{
	namespace Display
	{
		class Color 
		{ 
		public:
			typedef unsigned char ColorComponent;
			typedef unsigned char AlfaChannel;
			typedef uint32 ColorType;
			enum
			{
				Black = 0x000000,
				White = 0xffffff,
				Transparent = 0x00,
				SemiTransparent = 0x80,
				Opaque = 0xff
			};

			Color(const CommonLib::str_t& rgb, AlfaChannel a = Opaque);
			Color(ColorType rgb = Black, AlfaChannel a = Opaque);
			Color(ColorComponent r, ColorComponent g, ColorComponent b, AlfaChannel a = Opaque);

			ColorType GetRGB() const;
			ColorType GetRGBA() const;
			AlfaChannel GetA() const;
			ColorComponent GetR() const;
			ColorComponent GetG() const;
			ColorComponent GetB() const;

			static Color RndLineColor();
			static Color RndFillColor();

			bool operator==(const Color& a) const;
			bool operator!=(const Color& a) const;

			void Save(CommonLib::IWriteStream *pStream) const;
			void Load(CommonLib::IReadStream *pStream);
			

		private:
			static Color RndColor(Color::ColorComponent *rgb);
			ColorType rgba_;
		};
	}
}

#endif