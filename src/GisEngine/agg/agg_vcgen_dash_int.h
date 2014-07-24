//----------------------------------------------------------------------------
// Anti-Grain Geometry (AGG) - Version 2.5
// A high quality rendering engine for C++
// Copyright (C) 2002-2006 Maxim Shemanarev
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://antigrain.com
// 
// AGG is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// AGG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with AGG; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA 02110-1301, USA.
//----------------------------------------------------------------------------

#ifndef AGG_VCGEN_DASH_INTEGER_INCLUDED
#define AGG_VCGEN_DASH_INTEGER_INCLUDED

#include "agg_basics.h"
#include "agg_vertex_sequence_int.h"

namespace agg
{

  //---------------------------------------------------------------vcgen_dash
  //
  // See Implementation agg_vcgen_dash.cpp
  //
  class vcgen_dash_int
  {
    enum max_dashes_e
    {
      max_dashes = 32
    };

    enum status_e
    {
      initial,
      ready,
      polyline,
      stop
    };

  public:
    typedef vertex_sequence_int<vertex_dist_int, 6> vertex_storage;

    vcgen_dash_int();

    void remove_all_dashes();
    void add_dash(int dash_len, int gap_len);
    void dash_start(int ds);

    void shorten(int s) { m_shorten = s; }
    double shorten() const { return m_shorten; }

    // Vertex Generator Interface
    void remove_all();
    void add_vertex(int x, int y, unsigned cmd);

    // Vertex Source Interface
    void     rewind(unsigned path_id);
    unsigned vertex(int* x, int* y);

  private:
    vcgen_dash_int(const vcgen_dash_int&);
    const vcgen_dash_int& operator = (const vcgen_dash_int&);

    void calc_dash_start(int ds);

    int             m_dashes[max_dashes];
    int             m_total_dash_len;
    unsigned           m_num_dashes;
    int             m_dash_start;
    int             m_shorten;
    int             m_curr_dash_start;
    unsigned           m_curr_dash;
    int64             m_curr_rest;
    const vertex_dist_int* m_v1;
    const vertex_dist_int* m_v2;

    vertex_storage m_src_vertices;
    unsigned       m_closed;
    status_e       m_status;
    unsigned       m_src_vertex;
  };


}

#endif
