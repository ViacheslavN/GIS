
union ieee754_double
{
	double d;

	/* This is the IEEE 754 double-precision format.  */
	struct
	{
#if __BYTE_ORDER == __BIG_ENDIAN
		unsigned int negative:1;
		unsigned int exponent:11;
		/* Together these comprise the mantissa.  */
		unsigned int mantissa0:20;
		unsigned int mantissa1:32;
#endif              /* Big endian.  */
#if __BYTE_ORDER == __LITTLE_ENDIAN
# if    __FLOAT_WORD_ORDER == __BIG_ENDIAN
		unsigned int mantissa0:20;
		unsigned int exponent:11;
		unsigned int negative:1;
		unsigned int mantissa1:32;
# else
		/* Together these comprise the mantissa.  */
		unsigned int mantissa1:32;
		unsigned int mantissa0:20;
		unsigned int exponent:11;
		unsigned int negative:1;
# endif
#endif              /* Little endian.  */
	} ieee;

	/* This format makes it easier to see if a NaN is a signalling NaN.  */
	struct
	{
#if __BYTE_ORDER == __BIG_ENDIAN
		unsigned int negative:1;
		unsigned int exponent:11;
		unsigned int quiet_nan:1;
		/* Together these comprise the mantissa.  */
		unsigned int mantissa0:19;
		unsigned int mantissa1:32;
#else
# if    __FLOAT_WORD_ORDER == __BIG_ENDIAN
		unsigned int mantissa0:19;
		unsigned int quiet_nan:1;
		unsigned int exponent:11;
		unsigned int negative:1;
		unsigned int mantissa1:32;
# else
		/* Together these comprise the mantissa.  */
		unsigned int mantissa1:32;
		unsigned int mantissa0:19;
		unsigned int quiet_nan:1;
		unsigned int exponent:11;
		unsigned int negative:1;
# endif
#endif
	} ieee_nan;
};

