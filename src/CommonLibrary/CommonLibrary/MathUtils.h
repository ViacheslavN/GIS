#ifndef _LIB_COMMON_MATH_UTILS_H_
#define _LIB_COMMON_MATH_UTILS_H_
#include "general.h"
#include <math.h>
namespace CommonLib
{

	namespace mathUtils
	{
	  static  double Ln_2 = 0.69314718055994529;

	  static inline double Log2(double dVal)
	  {
		  return log(dVal)/Ln_2;
	  }

	  static inline uint64 UI64_Add(uint64 x, uint64 y, bool carryin, bool* carryout)
	  {
		  uint64 tmp = y + carryin;
		  uint64 result = x + tmp;
		  *carryout = (tmp < y) || (result < x);
		  return result;
	  }

	  static inline uint32 GetScale(double dVal, uint32 nMaxScale = 15)
	  {
		  uint64 nPow = 1;
		  dVal = fabs(dVal);
		  for (uint32 i = 0; i < nMaxScale; ++i)
		  {
			  uint64 nVal = uint64(dVal * nPow);
			  double dBack = (double)nVal/nPow;
			  if(fabs(dVal - dBack) == 0.)
				  return i;
			  nPow *= 10;
		  }
		  return nMaxScale;
	  } 

	}
}


#endif