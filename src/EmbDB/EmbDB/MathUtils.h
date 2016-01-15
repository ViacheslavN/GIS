#ifndef _EMBEDDED_DATABASE_MATH_CONST_H_
#define _EMBEDDED_DATABASE_MATH_CONST_H_
#include "CommonLibrary/general.h"
#include <math.h>
namespace embDB
{

	namespace mathUtils
	{
	  static  double LOG10_2 = 0.30102999566;

	  static inline double Log2(double dVal)
	  {
		  return log(dVal)/LOG10_2;
	  }
	}
}


#endif