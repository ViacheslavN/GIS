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

#ifndef AGG_CONV_DASH_INTEGER_INCLUDED
#define AGG_CONV_DASH_INTEGER_INCLUDED

#include "agg_basics.h"
#include "agg_vcgen_dash_int.h"
#include "agg_conv_adaptor_vcgen_int.h"

namespace agg
{

  //---------------------------------------------------------------conv_dash
  template<class VertexSource, class Markers=null_markers> 
  struct conv_dash_int : public conv_adaptor_vcgen_int<VertexSource, vcgen_dash_int, Markers>
  {
    typedef Markers marker_type;
    typedef conv_adaptor_vcgen_int<VertexSource, vcgen_dash_int, Markers> base_type;

    conv_dash_int(VertexSource& vs) : 
    conv_adaptor_vcgen_int<VertexSource, vcgen_dash_int, Markers>(vs)
    {
    }

    void remove_all_dashes() 
    { 
      base_type::generator().remove_all_dashes(); 
    }

    void add_dash(double dash_len, double gap_len) 
    { 
      base_type::generator().add_dash(dash_len * poly_subpixel_scale, gap_len * poly_subpixel_scale); 
    }

    void dash_start(double ds) 
    { 
      base_type::generator().dash_start(ds); 
    }

    void shorten(double s) { base_type::generator().shorten(s); }
    double shorten() const { return base_type::generator().shorten(); }

  private:
    conv_dash_int(const conv_dash_int<VertexSource, Markers>&);
    const conv_dash_int<VertexSource, Markers>& 
      operator = (const conv_dash_int<VertexSource, Markers>&);
  };


}

#endif
