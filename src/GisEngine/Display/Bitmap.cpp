#include "stdafx.h"
#include "Bitmap.h"


#include "agg/agg_renderer_base.h"
#include "agg/agg_scanline_p.h"
#include "agg/agg_renderer_scanline.h"
#include "agg/agg_pixfmt_rgba.h"

#include "agg/agg_rasterizer_outline.h"
#include "agg/agg_renderer_primitives.h"
#include "agg/agg_rasterizer_outline_aa.h"
#include "agg/agg_renderer_outline_aa.h"
#include "agg/agg_ellipse.h"
#include "agg/agg_font_cache_manager.h"
#include "agg/agg_rasterizer_scanline_aa.h"
#include "agg/agg_conv_dash.h"
#include "agg/agg_conv_stroke.h"

#include "agg/agg_span_allocator.h"
#include "agg/agg_span_interpolator_linear.h"
#include "agg/agg_span_image_filter.h"
#include "agg/agg_image_accessors.h"
#include "agg/agg_span_image_filter_rgba.h"
#include "agg/agg_span_pattern_rgba.h"
#include "agg/agg_trans_affine.h"
#include "Common/Common.h"
#include "ReadPng.h"
#include "ReadJPG.h"

namespace GisEngine
{
	namespace Display
	{

		CBitmap::CBitmap()
		{
			buf_ = 0;
			width_ = 0;
			height_ = 0;
			palette_ = 0;
			release_ = false;
			type_ = BitmapFormatType32bppARGB;
		}

		CBitmap::CBitmap(unsigned char* bits, size_t width, size_t height, BitmapFormatType type, Color* palette, bool release)
		{
			attach(bits, width, height, type, palette, release);
		}

		CBitmap::CBitmap(CommonLib::IReadStream* pStream)
		{		 

			width_ = (size_t)pStream->readInt32();
			height_ = (size_t)pStream->readInt32();
			type_ = (BitmapFormatType)pStream->readByte();
			init(width_, height_, type_);
			pStream->read(buf_, size());
			if(palette_)
				for(int i = 0; i < 1 << bpp(); i++)
					palette_[i].load(pStream);
		}

		CBitmap::CBitmap(size_t width, size_t height, BitmapFormatType type)
		{
			init(width, height, type);
		}

		void CBitmap::init(size_t width, size_t height, BitmapFormatType type)
		{
			width_ = width;
			height_ = height;
			type_ = type;
			palette_ = 0;
			buf_ = new unsigned char[size()];
			switch(type_)
			{
			case BitmapFormatType1bpp:
				palette_ = new Color[2];
				break;
			case BitmapFormatType4bpp:
				palette_ = new Color[16];
				break;
			case BitmapFormatType8bpp:
				palette_ = new Color[256];
				break;
			}
			release_ = true;
		}

		CBitmap::CBitmap(const CBitmap& bmp)
			: palette_(0)
			, buf_(0)
		{
			this->operator=(bmp);
		}

		CBitmap::~CBitmap(void)
		{
			if(release_)
			{
				delete [] buf_;
				if(palette_)
					delete palette_;
			}
		}

		CBitmap& CBitmap::operator=(const CBitmap& bmp)
		{
			if(this == &bmp)
				return *this;

			if(release_)
			{
				delete [] buf_;
				buf_ = 0;
				if(palette_)
					delete palette_;
				palette_ = 0;
			}

			width_ = bmp.width_,
				height_ = bmp.height_,
				type_ = bmp.type_;
			if(bmp.buf_ == 0)
			{
				release_ = false;
				return *this;
			}
			release_ = true;

			buf_ = new unsigned char[size()];
			memcpy(buf_, bmp.buf_, size());

			switch(type_)
			{
			case BitmapFormatType1bpp:
				palette_ = new Color[2];
				memcpy(palette_, bmp.palette_, sizeof(Color) * 2);
				break;
			case BitmapFormatType4bpp:
				palette_ = new Color[16];
				memcpy(palette_, bmp.palette_, sizeof(Color) * 16);
				break;
			case BitmapFormatType8bpp:
				palette_ = new Color[256];
				memcpy(palette_, bmp.palette_, sizeof(Color) * 256);
				break;
			}

			return *this;
		}

		size_t CBitmap::height() const
		{
			return height_;
		}

		size_t CBitmap::width() const
		{
			return width_;
		}

		inline unsigned char* CBitmap::bits()
		{
			return buf_;
		}

		const unsigned char* CBitmap::bits() const
		{
			return buf_;
		}

		size_t CBitmap::lineSize() const
		{
			return 4 * ((width_ * bpp() + 31) / 32);
		}

		size_t CBitmap::size() const
		{
			return lineSize() * height_;
		}

		size_t CBitmap::bpp() const
		{
			switch(type_)
			{
			case BitmapFormatType1bpp:
				return 1;
			case BitmapFormatType4bpp:
				return 4;
			case BitmapFormatType8bpp:
				return 8;
			case BitmapFormatType16bppARGB1555:
			case BitmapFormatType16bppGrayScale:
			case BitmapFormatType16bppRGB555:
			case BitmapFormatType16bppRGB565:
				return 16;
			case BitmapFormatType24bppRGB:
				return 24;
			case BitmapFormatType32bppARGB:
				return 32;
			}
			return 0;
		}

		Color* CBitmap::palette()
		{
			return palette_;
		}

		BitmapFormatType CBitmap::type() const
		{
			return type_;
		}

		Color CBitmap::pixel(size_t row, size_t col)
		{
			size_t row_ptr = lineSize() * row;
			size_t ptr = row_ptr + (col * bpp()) / 8;

			if(bpp() <= 8) // get from pallete
			{
				size_t idx = 0;
				if(!palette_)
					return Color();
				switch(type_)
				{
				case BitmapFormatType1bpp:
					idx = ((buf_[ptr] >> (7 - (col % 8))) & 1) ? 1 : 0;
					break;
				case BitmapFormatType4bpp:
					idx = ((col % 2) == 0) ? (buf_[ptr] >> 4) & 0xF : buf_[ptr] & 0xF;
					return 4;
				case BitmapFormatType8bpp:
					idx = buf_[ptr];
					return 8;
				}
				return palette_[idx];
			}
			else
			{
				Color::ColorComponent r = 0, g = 0, b = 0, a = Color::Opaque;
				switch(type_)
				{
				case BitmapFormatType16bppARGB1555:
					a = buf_[ptr] & 1;
					b = (buf_[ptr] >> 1) & 31;
					g = ((*((int16*)&buf_[ptr])) >> 6) & 31;
					r = ((*((int16*)&buf_[ptr])) >> 11) & 31;
					break;
				case BitmapFormatType16bppGrayScale: // type Color can store 256 grayscale gradations only
					return Color();
				case BitmapFormatType16bppRGB555:
					b = (buf_[ptr] >> 1) & 31;
					g = (*((int16*)&buf_[ptr]) >> 6) & 31;
					r = (*((int16*)&buf_[ptr]) >> 11) & 31;
					break;
				case BitmapFormatType16bppRGB565:
					r = ((*((int16*)&buf_[ptr])) >> 8) & 0xF8;
					g = ((*((int16*)&buf_[ptr])) >> 3) & 0xFC; 
					b = ((*((int16*)&buf_[ptr])) << 3) & 0xF8;
					break;
				case BitmapFormatType24bppRGB:
					b = buf_[ptr];
					g = buf_[ptr + 1];
					r = buf_[ptr + 2];
					break;
				case BitmapFormatType32bppARGB:
					b = buf_[ptr];
					g = buf_[ptr + 1];
					r = buf_[ptr + 2];
					a = buf_[ptr + 3];
					break;
					//return *((Color*)&buf_[ptr]);
				}
				return Color(r, g, b, a);
			}
		}

		bool CBitmap::copy_to(CBitmap* bitmap)
		{
			if(bitmap->height() != height() || bitmap->width() != width())
				return false;

			// TODO: support all bitmap formats
			if(bitmap->type() != BitmapFormatType32bppARGB)
				return false;

			Color::ColorComponent* bits = bitmap->bits();
			for(size_t row = 0; row < height_; row++)
			{
				for(size_t col = 0; col < width_; col++, bits += 4)
				{
					Color color = pixel(row, col);
					bits[0] = color.GetB();
					bits[1] = color.GetG();
					bits[2] = color.GetR();
					bits[3] = color.GetA();
				}
			}

			return true;
		}

		void CBitmap::save(CommonLib::IWriteStream *pStream) const
		{

			pStream->write((uint32)width_);
			pStream->write((uint32)height_);
			pStream->write((byte)type_);
			pStream->write(buf_, size());
			if(palette_)
				for(int i = 0; i < 1 << bpp(); i++)
					palette_[i].save(pStream);
		}

		void CBitmap::attach(unsigned char* bits, size_t width, size_t height, BitmapFormatType type, Color* palette, bool release)
		{
			buf_ = bits;
			width_ = width;
			height_ = height;
			type_ = type;
			palette_ = palette_;
			release_ = release;
		}

		CBitmap* CBitmap::transform(double xScale, double yScale, double angle)
		{
			if(this->type() != BitmapFormatType32bppARGB)
				return 0;
			size_t newWidth = size_t(this->width() * xScale);
			size_t newHeight = size_t(this->height() * yScale);

			CBitmap* ret = new CBitmap(newWidth, newHeight, this->type());
			memset(ret->bits(), 0, ret->size());

			int w = (int)this->width();
			int h = (int)this->height(); 
			std::vector<unsigned char> tmpBuf;

			typedef agg::pixfmt_bgra32 pixfmt_t;
			typedef agg::span_allocator<agg::rgba8> span_allocator_t;
			typedef agg::wrap_mode_repeat pattern_wrap_t;
			typedef agg::image_accessor_wrap<agg::pixfmt_bgra32, pattern_wrap_t, pattern_wrap_t> pattern_accessor_t;
			typedef agg::span_pattern_rgba<pattern_accessor_t> span_pattern_generator_t;
			typedef agg::renderer_base<pixfmt_t> renderer_base_t;
			typedef agg::renderer_scanline_aa_solid<renderer_base_t> renderer_t;
			typedef agg::rasterizer_scanline_aa<> rasterizer_t;

			agg::rendering_buffer rbuf_(ret->bits(), (int)newWidth, (int)newHeight, -4 * (int)newWidth);
			pixfmt_t rendering_buffer_(rbuf_);
			renderer_base_t renderer_base_(rendering_buffer_);
			renderer_t renderer_(renderer_base_);
			rasterizer_t rasterizer_;

			typedef agg::scanline_p8 scanline_t;


			span_allocator_t         span_allocator_;
			agg::pixfmt_bgra32       pattern_rgba32_;
			agg::rendering_buffer    pattern_rbuf_;
			span_pattern_generator_t span_pattern_generator_;

			pattern_rbuf_.attach((agg::int8u*)this->bits(), w , h, (-w) * 4); 
			pattern_rgba32_.attach(pattern_rbuf_);

			typedef agg::image_accessor_clone<agg::pixfmt_bgra32> img_accessor_type;
			img_accessor_type ia(pattern_rgba32_);

			double pol[8] = {0, 0, newWidth, 0, newWidth, newHeight, 0, newHeight};

			rasterizer_.reset();
			agg::trans_affine mtx2(pol, 0, 0, w, h);
			if(!equals(angle, 0.0))
			{
				mtx2 *= agg::trans_affine_translation(-(newWidth /  2.0), -(newHeight /  2.0));
				mtx2 *= agg::trans_affine_rotation(DEG2RAD(angle));
				mtx2 *= agg::trans_affine_translation(newWidth /  2.0, newWidth /  2.0);
			}
			//mtx2 *= agg::trans_affine_scaling(1 / xScale, 1 / yScale);

			typedef agg::span_interpolator_linear<> interpolator_type;
			interpolator_type interpolator(mtx2);

			typedef agg::span_image_filter_rgba_bilinear<img_accessor_type, interpolator_type> span_gen_type;
			span_gen_type sg(ia, interpolator, 255);

			rasterizer_.move_to_d(0, 0);
			rasterizer_.line_to_d(newWidth, 0);
			rasterizer_.line_to_d(newWidth, newHeight);
			rasterizer_.line_to_d(0, newHeight);
			rasterizer_.line_to_d(0, 0);

			agg::scanline_p8 scanline_;

			agg::render_scanlines_aa(rasterizer_, scanline_, renderer_base_, span_allocator_, sg);

			return ret;
		}


		bool   CBitmap::importFromPNG(const unsigned char* data, size_t len)
		{
#if defined(WIN32) || defined(_WIN32_WCE)  || defined(ANDROID)
			ReadPng png(data, (int)len);
			return png.read(this);
#elif defined(__IPHONE_3_1)
			bool flipY = true; // ????
			if(!data)
				return false;		
			if (!len)
				return false;

			CGDataProviderRef provider = 
				CGDataProviderCreateWithData(NULL, (const void*)data, len, NULL);
			CGImageRef cgImg = CGImageCreateWithPNGDataProvider(provider, NULL, true, kCGRenderingIntentDefault);
			if (cgImg == NULL)
				return false;

			CGDataProviderRelease(provider);

			if (8 != CGImageGetBitsPerComponent(cgImg))
				return false;
			if (32 != CGImageGetBitsPerPixel(cgImg))
				return false;

			const int width = CGImageGetWidth(cgImg);
			const int height = CGImageGetHeight(cgImg);
			const int dataSize = width*height*4;
			const int rowSize = width*4;

			CFDataRef dataRef =
				CGDataProviderCopyData(CGImageGetDataProvider(cgImg));
			CGImageRelease(cgImg);

			unsigned char *bits = new unsigned char[dataSize];	
			this->attach(bits, width, height, Display::BitmapFormatType32bppARGB, 0, true);

			BYTE *rowTempBytes = (BYTE*)malloc(rowSize);

			memcpy(this->bits(), CFDataGetBytePtr(dataRef), dataSize);
			CFRelease(dataRef);

			if (flipY)
			{
				// mirror flip
				for (int row = 0; row < height/2; row++)
				{
					memcpy(rowTempBytes, this->bits() + rowSize*row, rowSize);
					memcpy(this->bits() + rowSize*row, this->bits() + rowSize*(height-row-1), rowSize);
					memcpy(this->bits() + rowSize*(height-row-1), rowTempBytes, rowSize);
				}
			}
			free(rowTempBytes);	

			// ARGB to ABGR or ABGR to ARGB
			// or RGBA to BGRA...???
			for (int row = 0; row < height; row++)
			{
				for (int col = 0; col < width; col++)
				{
					BYTE b0 = *(this->bits() + rowSize*row + col*4+0);
					BYTE b2 = *(this->bits() + rowSize*row + col*4+2);			
					*(this->bits() + rowSize*row + col*4+0) = b2;
					*(this->bits() + rowSize*row + col*4+2) = b0;
				}
			}

			return true;	
#endif

			return false;
		}
		


		bool   CBitmap::importFromJPG(const unsigned char* data, size_t len)
		{
#if defined(WIN32) || defined(_WIN32_WCE)  || defined(ANDROID)
			ReadJPG jpg(data, (int)len);
			return jpg.read(this);
	
#elif defined(__IPHONE_3_1)

				bool flipY = true; // ????
				if(!data)
					return false;		
				if (!len)
					return false;
		
				CGDataProviderRef provider = 
					CGDataProviderCreateWithData(NULL, (const void*)data, len, NULL);
		
				CGImageRef cgImg =
					CGImageCreateWithJPEGDataProvider(provider, NULL, true,
					//kCGRenderingIntentSaturation);
					kCGRenderingIntentDefault);
		
				if (cgImg == NULL)
					return false;
		
				CGDataProviderRelease(provider);
	
				if (8 != CGImageGetBitsPerComponent(cgImg))
					return false;
				if (32 != CGImageGetBitsPerPixel(cgImg))
					return false;
	
				const int width = CGImageGetWidth(cgImg);
				const int height = CGImageGetHeight(cgImg);
				const int dataSize = width*height*4;
				const int rowSize = width*4;
	
				CFDataRef dataRef =
					CGDataProviderCopyData(CGImageGetDataProvider(cgImg));
				CGImageRelease(cgImg);
	
				unsigned char *bits = new unsigned char[dataSize];	
				this->attach(bits, width, height, Display::BitmapFormatType32bppARGB, 0, true);
		
				BYTE *rowTempBytes = (BYTE*)malloc(rowSize);
	
				memcpy(this->bits(), CFDataGetBytePtr(dataRef), dataSize);
				CFRelease(dataRef);
		
				if (flipY)
				{
					// mirror flip
					for (int row = 0; row < height/2; row++)
					{
						memcpy(rowTempBytes, this->bits() + rowSize*row, rowSize);
						memcpy(this->bits() + rowSize*row, this->bits() + rowSize*(height-row-1), rowSize);
						memcpy(this->bits() + rowSize*(height-row-1), rowTempBytes, rowSize);
					}
				}
				free(rowTempBytes);	
	
				// ARGB to ABGR or ABGR to ARGB
				// or RGBA to BGRA...???
				for (int row = 0; row < height; row++)
				{
					for (int col = 0; col < width; col++)
					{
						BYTE b0 = *(this->bits() + rowSize*row + col*4+0);	
						BYTE b2 = *(this->bits() + rowSize*row + col*4+2);			
						
						*(this->bits() + rowSize*row + col*4+0) = b2;
						*(this->bits() + rowSize*row + col*4+2) = b0;
			
						// force set alpha to maximum
						*(this->bits() + rowSize*row + col*4+3) = 0xFF;
					}
				}
	
				return true;
	
#endif
			  return false;
		}
	}
}