#include "stdafx.h"
#include "ReadPng.h"
#include "Bitmap.h"
#include "GraphTypes.h"


#include "zlib.h"
#include "zlib_lib.h"

#include "png.h"
//#include "pnginfo.h"
#include "png_lib.h"

namespace GisEngine
{
	namespace Display
	{
		ReadPng::ReadPng(const BYTE* ptr, int length) : data_(ptr), length_(length)
		{
			assert(data_ || length_);

			start = data_;
			end = data_+ length_;
			pos = start;
		}

		static void PNGAPI png_read_data(png_structp png_ptr,png_bytep data, png_size_t length) 
		{
			ReadPng *pio = (ReadPng *)png_get_io_ptr(png_ptr);
			if(pio->pos + length > pio->end)
				png_error(png_ptr,"PNG read error");
			memcpy(data,pio->pos,length);
			pio->pos += length;
		}

		bool  ReadPng::readPNG()
		{
			if(!data_ || !length_)
			{
				return false;
			}


			if(end - start <= 8)
				return false;
			if(!png_check_sig(const_cast<byte*>(start), 8))
				return false;

			png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
			if (!png_ptr) // out of memory?
				return false;
			png_infop	info_ptr = png_create_info_struct(png_ptr);
			if (!info_ptr) { // out of memory?
				png_destroy_read_struct(&png_ptr,NULL,NULL);
				return false;
			}
			// setup i/o routine
			png_set_read_fn(png_ptr,this,png_read_data);



			// init jmpbuf
			if (setjmp(png_jmpbuf(png_ptr))) { // error occurred
				png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
				return false;
			}
			png_read_info(png_ptr,info_ptr);
			png_set_bgr(png_ptr);
			//png_read_png (png_ptr,info_ptr, PNG_TRANSFORM_BGR, NULL);
			png_uint_32	width,height;
			int		bit_depth, color_type, interlace_type;
			png_get_IHDR(png_ptr,info_ptr,&width,&height,&bit_depth,&color_type,
				&interlace_type,NULL,NULL);


			//if (interlace_type!=PNG_INTERLACE_NONE) 
			//{
			//  png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
			//  return 0;
			//}
			// configure transformations, we always want RGB data in the end
			if (color_type == PNG_COLOR_TYPE_GRAY ||
				color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(png_ptr);
			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_palette_to_rgb(png_ptr);
			if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
				/*png_set_gray_1_2_4_to_8*/png_set_expand_gray_1_2_4_to_8(png_ptr);
			if (png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(png_ptr);
			if (bit_depth == 16)
				png_set_strip_16(png_ptr);
			if (bit_depth < 8)
				png_set_packing(png_ptr);
			double screen_gamma=2.2; // typical value
			double gamma;
			if (png_get_gAMA(png_ptr, info_ptr, &gamma))
				png_set_gamma(png_ptr, screen_gamma, gamma);
			else
				png_set_gamma(png_ptr, screen_gamma, 0.45455);
			//#endif

			// update info after applying transformations
			png_read_update_info(png_ptr,info_ptr);

			//aPNG ++
		//	png_uint_32 num_frames;
		//	png_uint_32 num_plays;
		//	png_uint_32 is_animated = png_get_acTL(png_ptr, info_ptr, &num_frames, &num_plays);

		/*	png_uint_32 is_animated = 0;
			if (png_ptr != NULL && info_ptr != NULL &&
				(info_ptr->valid & PNG_INFO_acTL) &&
				num_frames != NULL && num_plays != NULL)
			{
				is_animated = 1;
			}*/

		
			//aPNG --

			int		rowBytes = png_get_rowbytes(png_ptr, info_ptr);
			SIZE dim = {width, height};
			int   bpp = rowBytes / width;

			/*if(is_animated)
			{
				assert(false);
				return false;
			}*/

			switch (bpp)
			{
			case 4:
				{
					std::vector<png_byte *> rowPtrs;
					rowPtrs.resize(dim.cy);
					dib_.resizeDib(dim, 32);
					for (int i = 0; i < dim.cy; i++)
					{
						rowPtrs[i] = dib_.row_ptr(i);
					}
					png_read_image(png_ptr, &rowPtrs[0]);
				}
				break;
			case 3:
				{
					dib_.resizeDib(dim, 24);
					std::vector<png_byte *> rowPtrs;
					rowPtrs.resize(dim.cy);
					for (int i = 0; i < dim.cy; i++)
						rowPtrs[i] = dib_.row_ptr(i);
					png_read_image(png_ptr, &rowPtrs[0]);
				}
				break;
			default:
				assert(0);
			}
			png_read_end(png_ptr,info_ptr);
			png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

			return true;
		}


		CBitmap* ReadPng::read()
		{  
			if(!readPNG())
				return 0;
			CBitmap* pBitmap = new  CBitmap(); 
			pBitmap->attach(dib_.bitmap_bits(),  dib_.bitmap_width(), dib_.bitmap_height(),dib_.bpp_ == 32 ? BitmapFormatType32bppARGB : BitmapFormatType24bppRGB, 0, true );
			return pBitmap;
		}
		bool ReadPng::read(CBitmap* pBitmap)
		{
			if(!readPNG())
				return false;
			pBitmap->attach(dib_.bitmap_bits(),  dib_.bitmap_width(), dib_.bitmap_height(),dib_.bpp_ == 32 ? BitmapFormatType32bppARGB : BitmapFormatType24bppRGB, 0, true );
			return true;

		}
	}
}