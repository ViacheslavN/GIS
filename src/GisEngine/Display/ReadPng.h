#ifndef GIS_ENGINE_DISPLAY_PNG_READER_H_
#define GIS_ENGINE_DISPLAY_PNG_READER_H_
#include "GraphTypes.h"
#include "Common/Common.h"
#include "CommonLibrary/GeneralTypes.h"


namespace GisEngine
{
	namespace Display
	{


		struct argb 
		{
			byte    blue; 
			byte    green; 
			byte    red; 
			byte    alfa; 
		};

		struct dib
		{
			int row_byte_length() const
			{
				int nBytesPerLine = bmp_size.cx *( bytes_per_pixel());
				nBytesPerLine = ((nBytesPerLine + 31) & (~31)) / 8;
				return nBytesPerLine;
			}
			byte* row_ptr(int r)
			{
				//ATLASSERT(r <= bmp_size.cy);
				return (bitmap_bits() + (bmp_size.cy - r - 1) * row_byte_length());
			}

			SIZE size() const {return bmp_size;}
			int         bitmap_width() const  { return bmp_size.cx; }
			int         bitmap_height() const { return bmp_size.cy; }
			BYTE *      bitmap_bits() {return bits;}
			dib(): bpp_(0), bits(0), buf_size(0)
			{bmp_size.cx = 0; bmp_size.cy = 0;}
			void resizeDib(SIZE sz, int bpp)
			{
				bmp_size = sz;
				bpp_ = bpp;
				buf_size  = 4 * ((bitmap_width() * bpp_ + 31) / 32) * bitmap_height();/*bitmap_width() * bitmap_height()*/; 
				assert(!bits);
				bits = new unsigned char [buf_size];
			}

			int bytes_per_pixel() const {return bpp_;}
			void alpha_inv()
			{
				argb* pc = (argb*)bitmap_bits();
				argb* pcend = pc + (size().cx * size().cy);
				double K = 0;
				while(pc < pcend)
				{
					K = pc->alfa / 255.0;
					pc->blue = (byte)(pc->blue * K);
					pc->red = (byte)(pc->red * K);
					pc->green = (byte)(pc->green * K);
					//pc->alfa = 255 - pc->alfa;
					++pc;
				}
			}


			unsigned char* bits;
			SIZE bmp_size;
			int bpp_;
			int buf_size;

		};
		class CBitmap;
		class ReadPng
		{
		public:
			ReadPng(const BYTE* ptr = 0, int length = 0);

			CBitmap*  read();
			bool read(CBitmap* pBitmap);

			const BYTE * start;
			const BYTE * end;
			const BYTE * pos;
		private:

			bool  readPNG();
			dib dib_;
			int length_;
			const BYTE * data_;

		};
	}
}

#endif