#include "stdafx.h"

#if !defined(AGG_GRAY8) && !defined(AGG_GRAY16) && !defined(AGG_GRAY32)
#define AGG_GRAY32
#endif
/*
#if defined(AGG_GRAY8)
#define AGG_RENDERING_BUFFER row_accessor<int8u>
#elif defined(AGG_GRAY16)
#define AGG_RENDERING_BUFFER row_accessor<int16u>
#elif defined(AGG_GRAY32)
#define AGG_RENDERING_BUFFER row_accessor<float>
#endif*/
#define AGG_RENDERING_BUFFER row_accessor<int32u>
#ifndef RECT_X
#define RECT_X 1
#endif
#ifndef RECT_Y
#define RECT_Y 1
#endif

#include "../../agg/agg_basics.h"
#include "../../agg/agg_rendering_buffer.h"
#include "../../agg/agg_rasterizer_scanline_aa.h"
#include "../../agg/agg_scanline_u.h"
#include "../../agg/agg_renderer_scanline.h"
#include "../../agg/agg_pixfmt_rgb.h"
#include "../../agg/agg_pixfmt_rgba.h"
 
#include <iostream>

//#define AGG_GRAY8
//#define AGG_GRAY16
//#define AGG_GRAY32
 

typedef agg::rendering_buffer rendering_buffer;

static const int WIDTH = 10;
static const int HEIGHT = 4;

int TestAgg()
{
	agg::int32u buf_data[WIDTH * HEIGHT];
	agg::rendering_buffer rbuf(buf_data, WIDTH, HEIGHT, WIDTH);

	// Create Pixel Format and Basic renderers
	//--------------------
	typedef agg::pixfmt_bgra32 pixfmt;
	pixfmt pixf(rbuf);
	agg::renderer_base<pixfmt> ren_base(pixf);

	// At last we do some very simple things, like clear
	//--------------------
	ren_base.clear(agg::rgba8(0, 0, 0));

	// Create Scanline Container, Scanline Rasterizer,
	// and Scanline Renderer for solid fill.
	//--------------------
	agg::scanline_u8 sl;
	agg::rasterizer_scanline_aa<> ras;
	agg::renderer_scanline_aa_solid<
		agg::renderer_base<pixfmt> > ren_sl(ren_base);

	float ul_x = RECT_X; // upper-left corner position
	float ul_y = RECT_Y;
	float sz = 2; // size of the drawn rectangle

	ras.move_to_d(ul_x, ul_y);
	ras.line_to_d(ul_x + sz, ul_y);
	ras.line_to_d(ul_x + sz, ul_y + sz);
	ras.line_to_d(ul_x, ul_y + sz);

	// Set the color_type and render the scanlines
	//-----------------------
	ren_sl.color(agg::rgba8(255, 255, 255));
	agg::render_scanlines(ras, sl, ren_sl);

	// Print the buffer
	for (int y = 0; y < HEIGHT; y++) {
		auto row = rbuf.row_ptr(y);
		for (int x = 0; x < WIDTH; x++) {
			std::cout << (float)row[x] << " ";
		}
		std::cout << std::endl;
	}

	return 0;
}