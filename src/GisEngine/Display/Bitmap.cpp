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

		CBitmap::CBitmap(CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
		{
			m_pBuf = 0;
			m_nWidth = 0;
			m_nHeight = 0;
			m_pPalette = 0;
			m_bRelease = false;
			m_type = BitmapFormatType32bppARGB;

			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
		}

		CBitmap::CBitmap(unsigned char* bits, size_t width, size_t height, eBitmapFormatType type, 
			Color* palette, bool release, CommonLib::alloc_t *pAlloc): m_pAlloc(pAlloc)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;
			attach(bits, width, height, type, palette, release);
		}

		CBitmap::CBitmap(CommonLib::IReadStream* pStream, CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
		{		 
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;

			m_nWidth = (size_t)pStream->readInt32();
			m_nHeight = (size_t)pStream->readInt32();
			m_type = (eBitmapFormatType)pStream->readByte();
			init(m_nWidth, m_nHeight, m_type);
			pStream->read(m_pBuf, size());
			if(m_pPalette)
				for(int i = 0; i < 1 << bpp(); i++)
					m_pPalette[i].load(pStream);
		}

		CBitmap::CBitmap(size_t width, size_t height, eBitmapFormatType type, CommonLib::alloc_t *pAlloc) : m_pAlloc(pAlloc)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_alloc;

			init(width, height, type);
		}

		void CBitmap::init(size_t width, size_t height, eBitmapFormatType type)
		{
			m_nWidth = width;
			m_nHeight = height;
			m_type = type;
			m_pPalette = 0;
			m_pBuf = new unsigned char[size()];
			switch(m_type)
			{
			case BitmapFormatType1bpp:
				m_pPalette = new Color[2];
				break;
			case BitmapFormatType4bpp:
				m_pPalette = new Color[16];
				break;
			case BitmapFormatType8bpp:
				m_pPalette = new Color[256];
				break;
			}
			m_bRelease = true;
		}

		CBitmap::CBitmap(const CBitmap& bmp)
			: m_pPalette(0)
			, m_pBuf(0)
		{
			if(bmp.m_pAlloc != &bmp.m_alloc)
				m_pAlloc = bmp.m_pAlloc ;
			else
				m_pAlloc = &m_alloc;


			this->operator=(bmp);
		}

		CBitmap::~CBitmap(void)
		{
			if(m_bRelease)
			{
				m_pAlloc->free(m_pBuf);
				if(m_pPalette)
					m_pAlloc->free(m_pPalette);
			}
		}

		CBitmap& CBitmap::operator=(const CBitmap& bmp)
		{
			 if(this == &bmp)
				return *this;

			 
			if(m_bRelease && m_pBuf)
			{
				m_pAlloc->free(m_pBuf);
				m_pBuf = 0;
				if(m_pPalette)
					m_pAlloc->free(m_pPalette);
				m_pPalette = 0;
			}

			if(m_pAlloc != bmp.m_pAlloc && bmp.m_pAlloc != &bmp.m_alloc)
				m_pAlloc = bmp.m_pAlloc;

			m_nWidth = bmp.m_nWidth,
				m_nHeight = bmp.m_nHeight,
				m_type = bmp.m_type;
			if(bmp.m_pBuf == 0)
			{
				m_bRelease = false;
				return *this;
			}
			m_bRelease = true;

			m_pBuf = (byte*)m_pAlloc->alloc(size());
			memcpy(m_pBuf, bmp.m_pBuf, size());

			switch(m_type)
			{
			case BitmapFormatType1bpp:
				m_pPalette = (Color*)m_pAlloc->alloc(sizeof(Color) * 2);
				memcpy(m_pPalette, bmp.m_pPalette, sizeof(Color) * 2);
				break;
			case BitmapFormatType4bpp:
				m_pPalette = (Color*)m_pAlloc->alloc(sizeof(Color) * 16);
				memcpy(m_pPalette, bmp.m_pPalette, sizeof(Color) * 16);
				break;
			case BitmapFormatType8bpp:
				m_pPalette = (Color*)m_pAlloc->alloc(sizeof(Color) * 256);
				memcpy(m_pPalette, bmp.m_pPalette, sizeof(Color) * 256);
				break;
			}

			return *this;
		}

		size_t CBitmap::height() const
		{
			return m_nHeight;
		}

		size_t CBitmap::width() const
		{
			return m_nWidth;
		}

		inline unsigned char* CBitmap::bits()
		{
			return m_pBuf;
		}

		const unsigned char* CBitmap::bits() const
		{
			return m_pBuf;
		}

		size_t CBitmap::lineSize() const
		{
			return 4 * ((m_nWidth * bpp() + 31) / 32);
		}

		size_t CBitmap::size() const
		{
			return lineSize() * m_nHeight;
		}

		size_t CBitmap::bpp() const
		{
			switch(m_type)
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
			return m_pPalette;
		}

		eBitmapFormatType CBitmap::type() const
		{
			return m_type;
		}

		Color CBitmap::pixel(size_t row, size_t col)
		{
			size_t row_ptr = lineSize() * row;
			size_t ptr = row_ptr + (col * bpp()) / 8;

			if(bpp() <= 8) // get from pallete
			{
				size_t idx = 0;
				if(!m_pPalette)
					return Color();
				switch(m_type)
				{
				case BitmapFormatType1bpp:
					idx = ((m_pBuf[ptr] >> (7 - (col % 8))) & 1) ? 1 : 0;
					break;
				case BitmapFormatType4bpp:
					idx = ((col % 2) == 0) ? (m_pBuf[ptr] >> 4) & 0xF : m_pBuf[ptr] & 0xF;
					return 4;
				case BitmapFormatType8bpp:
					idx = m_pBuf[ptr];
					return 8;
				}
				return m_pPalette[idx];
			}
			else
			{
				Color::ColorComponent r = 0, g = 0, b = 0, a = Color::Opaque;
				switch(m_type)
				{
				case BitmapFormatType16bppARGB1555:
					a = m_pBuf[ptr] & 1;
					b = (m_pBuf[ptr] >> 1) & 31;
					g = ((*((int16*)&m_pBuf[ptr])) >> 6) & 31;
					r = ((*((int16*)&m_pBuf[ptr])) >> 11) & 31;
					break;
				case BitmapFormatType16bppGrayScale: // type Color can store 256 grayscale gradations only
					return Color();
				case BitmapFormatType16bppRGB555:
					b = (m_pBuf[ptr] >> 1) & 31;
					g = (*((int16*)&m_pBuf[ptr]) >> 6) & 31;
					r = (*((int16*)&m_pBuf[ptr]) >> 11) & 31;
					break;
				case BitmapFormatType16bppRGB565:
					r = ((*((int16*)&m_pBuf[ptr])) >> 8) & 0xF8;
					g = ((*((int16*)&m_pBuf[ptr])) >> 3) & 0xFC; 
					b = ((*((int16*)&m_pBuf[ptr])) << 3) & 0xF8;
					break;
				case BitmapFormatType24bppRGB:
					b = m_pBuf[ptr];
					g = m_pBuf[ptr + 1];
					r = m_pBuf[ptr + 2];
					break;
				case BitmapFormatType32bppARGB:
					b = m_pBuf[ptr];
					g = m_pBuf[ptr + 1];
					r = m_pBuf[ptr + 2];
					a = m_pBuf[ptr + 3];
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
			for(size_t row = 0; row < m_nHeight; row++)
			{
				for(size_t col = 0; col < m_nWidth; col++, bits += 4)
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

		bool CBitmap::save(CommonLib::IWriteStream *pStream) const
		{

			pStream->write((uint32)m_nWidth);
			pStream->write((uint32)m_nHeight);
			pStream->write((byte)m_type);
			pStream->write(m_pBuf, size());
			if(m_pPalette)
				for(int i = 0; i < 1 << bpp(); i++)
					m_pPalette[i].save(pStream);
			return true;
		}
		bool  CBitmap::load(CommonLib::IReadStream *pStream)
		{

			SAFE_READ_RES(pStream, m_nWidth)
			SAFE_READ_RES(pStream, m_nHeight);
			byte type = 0;
			SAFE_READ_RES(pStream, type);
			m_type = (eBitmapFormatType)type;
			init(m_nWidth, m_nHeight, m_type);
			size_t nSize = size();
			if(nSize)
				pStream->read(m_pBuf, nSize);
			if(m_pPalette)
			{
				for(int i = 0; i < 1 << bpp(); i++)
				{
					if(!m_pPalette[i].load(pStream))
						return false;
				}
			}
			return true;

		}

		bool CBitmap::saveXML(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszNodeName) const
		{
			GisCommon::IXMLNodePtr pBlobNode = pXmlNode->CreateChildNode();
			pBlobNode->SetName(pszNodeName);
			CommonLib::MemoryStream stream(m_pAlloc);
			save(&stream);
			CommonLib::CBlob blob(stream.buffer(), stream.size(), true, NULL);
			pBlobNode->SetBlobCDATA(blob);
			return true;

		}
		bool CBitmap::load(GisCommon::IXMLNode* pXmlNode, const wchar_t *pszNodeName)
		{
			 uint32 nCnt = pXmlNode->GetChildCnt();
			 for(uint32 i = 0; i < nCnt; ++i)
			 {
				 GisCommon::IXMLNodePtr pBlobNode = pXmlNode->GetChild(i);
				 if(!pBlobNode.get())
					continue;
				 if(pBlobNode->GetName() != pszNodeName)
					 continue;
				 CommonLib::CBlob& blob = pBlobNode->GetBlobCDATA();
				 CommonLib::FxMemoryReadStream stream;
				 stream.attach(blob.buffer(), blob.size());
				 return load(&stream);
			 }
			 return false;
		}



		void CBitmap::attach(unsigned char* bits, size_t width, size_t height, eBitmapFormatType type, Color* palette, bool release)
		{
			m_pBuf = bits;
			m_nWidth = width;
			m_nHeight = height;
			m_type = type;
			m_pPalette = m_pPalette;
			m_bRelease = release;
		}

		CBitmap* CBitmap::transform(double xScale, double yScale, double angle)
		{
			if(this->type() != BitmapFormatType32bppARGB)
				return 0;
			size_t newWidth = size_t(this->width() * xScale);
			size_t newHeight = size_t(this->height() * yScale);

			CBitmap* ret = new CBitmap(newWidth, newHeight, this->type(), m_pAlloc);
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