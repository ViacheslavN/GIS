#ifndef _SIN_COS_TABLE_H_
#define _SIN_COS_TABLE_H_

#include "agg_basics.h"

namespace agg
{

  long sqrt_long (long r);
  long sqrt_long_long (int64 r);
  long sin_long (long grad);        //the rusult is multiplied by 10000
  long cos_long (long grad);        //the rusult is multiplied by 10000
  double sin_double (long grad);
  double cos_double (long grad);
  long acos_long (long val);        //result is in degrees, input MUST be multiplied by 10000
  long acos_double (double dval);   //result is in degrees, input must NOT be multiplied by 10000
  long asin_long (long val);        //result is in degrees, input MUST be multiplied by 10000
  long asin_double (double dval);   //result is in degrees, input must NOT be multiplied by 10000
}
#endif