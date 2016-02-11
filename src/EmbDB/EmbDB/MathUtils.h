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
		  return log(dVal)/log(2.);//Ln_2;
	  }
	}
}


#endif