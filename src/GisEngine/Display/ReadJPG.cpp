#include "stdafx.h"
#include "stdafx.h"
#include "ReadJPG.h"
#include "Bitmap.h"
#include "GraphTypes.h"
#include "jpeglib_lib.h"
#include <setjmp.h>
namespace GisEngine
{
	namespace Display
	{

		struct my_error_mgr 
		{
			struct jpeg_error_mgr pub;	/* "public" fields */
			jmp_buf setjmp_buffer;	/* for return to caller */
		};

		typedef struct my_error_mgr * my_error_ptr;

		METHODDEF(void)  __my_error_exit (j_common_ptr cinfo)
		{
			/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
			my_error_ptr myerr = (my_error_ptr) cinfo->err;

			/* Return control to the setjmp point */
			longjmp(myerr->setjmp_buffer, 1);
		}

		ReadJPG::ReadJPG(const BYTE* ptr, int length) : data_(ptr), length_(length)
		{
			assert(data_ || length_);
		}
		CBitmap*  ReadJPG::read()
		{
			CBitmap* pBitmap = new  CBitmap(); 
			if(readJPG(pBitmap))
				return pBitmap;
			delete pBitmap;
			return NULL;

		}
		bool ReadJPG::read(CBitmap* pBitmap)
		{
			return readJPG(pBitmap);
		}
		bool  ReadJPG::readJPG(CBitmap* pBitmap)
		{
			  struct jpeg_decompress_struct cinfo;
			  struct my_error_mgr __jerr;

			  //int    i, j, k, offset;

			  JSAMPROW row_pointer[1];
			  bool isgray;


			  /* Step 1: allocate and initialize JPEG decompression object */
			  cinfo.err = jpeg_std_error(&__jerr.pub);
			  __jerr.pub.error_exit = __my_error_exit;

			  /* Establish the setjmp return context for my_error_exit to use. */

			  if (setjmp(__jerr.setjmp_buffer)) {
				/* If we get here, the JPEG code has signaled an error.
				* We need to clean up the JPEG object, close the input file, and return.
				*/
				jpeg_destroy_decompress(&cinfo);
				return 0;
			  }

			  /* Now we can initialize the JPEG decompression object. */
			  jpeg_create_decompress(&cinfo);

			  /* Step 2: specify data source (eg, a file) */

			  jpeg_mem_src(&cinfo, (unsigned char*)data_, (int)length_);

			  /* Step 3: read file parameters with jpeg_read_header() */

			  (void) jpeg_read_header(&cinfo, (unsigned char)1);

			  if(cinfo.num_components == 3 && cinfo.out_color_space == JCS_RGB) 
			  {
				isgray = false;
			  }
			  else if(cinfo.num_components == 1 && cinfo.out_color_space == JCS_GRAYSCALE) 
			  {
				isgray = true;
			  }
			  else {
				jpeg_destroy_decompress(&cinfo);
				return false;
			  }

			  pBitmap->init(cinfo.image_width, cinfo.image_height, BitmapFormatType24bppRGB);

			  /* Step 4: set parameters for decompression */

			  /* Step 5: Start decompressor */

			  (void) jpeg_start_decompress(&cinfo);

			  /* Step 6: while (scan lines remain to be read) */
			  /*           jpeg_read_scanlines(...); */
			  if(isgray) {
				std::vector<unsigned char> row(cinfo.image_width); 
				row_pointer[0] = &row[0];
				while (cinfo.output_scanline < cinfo.output_height) 
				{
				  (void) jpeg_read_scanlines(&cinfo, row_pointer , 1);
				  if(cinfo.output_scanline >= cinfo.output_height) break;
				  unsigned char* pp = pBitmap->bits() + pBitmap->lineSize() * cinfo.output_scanline;
				  for(int i = 0; i < int(cinfo.image_width); i++, pp += 3)
					pp[0] = pp[1] = pp[2] = row[i];
				}
			  }
			  else {
				while (cinfo.output_scanline < cinfo.output_height) 
				{
				  unsigned char* pp = pBitmap->bits() + pBitmap->lineSize() * (cinfo.output_height - cinfo.output_scanline - 1);
				  row_pointer[0] = pp;
				  (void) jpeg_read_scanlines(&cinfo, row_pointer, 1);
				  for(size_t i = 0, sz = pBitmap->lineSize(); i < sz; i += 3)
					std::swap(pp[i], pp[i + 2]);
				}
			  }   

			  /* Step 7: Finish decompression */
			  (void) jpeg_finish_decompress(&cinfo);

			  /* Step 8: Release JPEG decompression object */
			  jpeg_destroy_decompress(&cinfo);

			  return true;
		}
	}
}