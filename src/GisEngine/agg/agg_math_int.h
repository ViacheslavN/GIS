
#ifndef AGG_MATH_INTEGER_INCLUDED
#define AGG_MATH_INTEGER_INCLUDED

#include <math.h>
#include "agg_basics.h"
#include <assert.h>
namespace agg
{

 AGG_INLINE int sqrt_int (int r)
{
    int t, b, c = 0;
    assert (r >= 0);
    for (b = 0x10000000; b != 0; b >>= 2) 
    {
      t = c + b;
      c >>= 1;
      if (t <= r) 
      {
        r -= t;
        c += b;
      }
    }
    return(c);
  }

 AGG_INLINE int sqrt_int_int (int64 r)
  {
    int64 t, b, c = 0;
    if (r <= 0x7fffffff)
      return sqrt_int ((int)r);
    assert (r >= 0);

    for (b = 0x1000000000000000LL; b != 0; b >>= 2) 
    {
      t = c + b;
      c >>= 1;
      if (t <= r) 
      {
        r -= t;
        c += b;
      }
    }
    return int(c);
  }


AGG_INLINE int64 calc_distance_int(int x1, int y1, int x2, int y2)
{
  int64 dx = x2 - x1;
  int64 dy = y2 - y1;
  return sqrt_int_int(dx * dx + dy * dy);
}
AGG_INLINE int64 cross_product_int(int x1, int y1, 
                                int x2, int y2, 
                                int x,  int y)
{
  return (int64)((x - x2)) * (int64)(y2 - y1) - ((int64)(y - y2))* ((int64)(x2 - x1));
}


AGG_INLINE bool calc_intersection_int(int ax, int ay, int bx, int by,
                       int cx, int cy, int dx, int dy,
                       int* x, int* y)
{
  int64 num = (int64)(ay-cy) * (int64)(dx-cx) - (ax-cx) * (int64)(dy-cy);
  int64 den = (int64)(bx-ax) * (int64)(dy-cy) - (by-ay) * (int64)(dx-cx);
  if(!den)
    return false;

  *x = (int)(ax + (num * (bx-ax)) / den);
  *y = (int)(ay + (num * (by-ay)) / den);
  return true;
}

}
#endif