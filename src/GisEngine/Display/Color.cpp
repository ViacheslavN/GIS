#include "stdafx.h"
#include "Color.h"

namespace GisEngine
{
	namespace Display
	{
		Color::Color(const CommonLib::str_t& rgb, AlfaChannel a)
		{
			int r, g, b;
			swscanf(rgb.cwstr(), L"%2X%2X%2X", &r, &g, &b);
			rgba_ = (static_cast<ColorType>(ColorComponent(r))
				| (static_cast<ColorType>(ColorComponent(g)) << 8)
				| (static_cast<ColorType>(ColorComponent(b)) << 16)
				| (static_cast<ColorType>(a) << 24));
		}
		Color::Color(ColorType rgb, AlfaChannel a): 
		rgba_((rgb & 0xffffff) | (static_cast<ColorType>(a) << 24))
		{
		}
		Color::Color(ColorComponent r, ColorComponent g, ColorComponent b, AlfaChannel a):
		rgba_(static_cast<ColorType>(r)
			| (static_cast<ColorType>(g) << 8)
			| (static_cast<ColorType>(b) << 16)
			| (static_cast<ColorType>(a) << 24))
		{
		}

		Color::ColorType Color::GetRGB() const
		{
			return rgba_ & 0x00ffffff;
		}
		Color::ColorType Color::GetRGBA() const
		{
			return rgba_;
		}
		Color::ColorComponent Color::GetR() const 
		{
			return static_cast<Color::ColorComponent>(rgba_);
		}
		Color::ColorComponent Color::GetG() const
		{
			return static_cast<Color::ColorComponent>(rgba_ >> 8);
		}
		Color::ColorComponent Color::GetB() const 
		{
			return static_cast<Color::ColorComponent>(rgba_ >> 16);
		}
		Color::ColorComponent Color::GetA() const
		{
			return static_cast<Color::ColorComponent>(rgba_ >> 24);
		}

		Color Color::RndLineColor()
		{
			Color::ColorComponent rgb[3];
			rgb[0] = 128;
			rgb[1] = rgb[0] - Color::ColorComponent(rand() % 128);
			rgb[2]= 128 - rgb[1];
			return RndColor(rgb);
		}
		Color Color::RndFillColor()
		{
			Color::ColorComponent rgb[3];
			rgb[0] = 255;
			rgb[1] = rgb[0] - Color::ColorComponent(rand() % 80);
			rgb[2]= 255 - (rgb[1] - 175);
			return RndColor(rgb);
		}

		Color Color::RndColor(Color::ColorComponent *rgb)
		{
			int idx[3];
			idx[0] = rand() % 3;
			while((idx[1] = rand() % 3) == idx[0]);
			idx[2] = 3 - (idx[0] + idx[1]);

			Color::ColorComponent r = rgb[idx[0]];
			Color::ColorComponent g = rgb[idx[1]];
			Color::ColorComponent b = rgb[idx[2]];
			return Color(r, g, b);
		}

		bool Color::operator==(const Color& c) const
		{
			return rgba_ == c.rgba_;
		}
		bool Color::operator!=(const Color& c) const
		{
			return !(this->operator ==(c));
		}

		void Color::Save(CommonLib::IWriteStream *pStream) const
		{
			pStream->write(rgba_);
		}
		void Color::Load(CommonLib::IReadStream *pStream)
		{
			rgba_ = pStream->readIntu32();
		}
	}
}