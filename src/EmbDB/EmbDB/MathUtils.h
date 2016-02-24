#ifndef _EMBEDDED_DATABASE_MATH_CONST_H_
#define _EMBEDDED_DATABASE_MATH_CONST_H_
#include "CommonLibrary/general.h"
#include <math.h>
namespace embDB
{

	namespace mathUtils
	{
	  static  double Ln_2 = 0.69314718055994529;

	  static inline double Log2(double dVal)
	  {
		  return log(dVal)/Ln_2;
	  }

	  static inline uint64 UI64_Sub(uint64 x, uint64 y, bool carryin, bool* carryout)
	  {
			uint64 tmp = y - carryin;
			uint64 result = x - tmp;
			*carryout = (tmp > y) || (result > x);
			return result;
	  }

	  static inline uint64 UI64_Add(uint64 x, uint64 y, bool carryin, bool* carryout)
	  {
		  uint64 tmp = y + carryin;
		  uint64 result = x + tmp;
		  *carryout = (tmp < y) || (result < x);
		  return result;
	  }
	  
	}
}


#endif