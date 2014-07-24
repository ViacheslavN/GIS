// http://www.cs.bell-labs.com/sources/plan9/sys/src/libstdio/

#include "stdafx.h"
/*
#ifdef __IPHONE_3_1
#include "stdafx.h"
#endif
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <limits.h>
#ifdef _WIN32
#include <malloc.h>
#endif
#include <string>
#include <sstream>
#include "sprintf.h"

#ifdef ANDROID
#include <ctype.h>
#include <alloca.h>
#define _gcvt gcvt
#define _ecvt ecvt
#define _alloca alloca 

#endif

#define        _IO_CHMASK        0377                /* mask for 8 bit characters */

#if defined (ANDROID)
char * ecvt (double _Val, int _NumOfDigits, int * _PtDec, int * _PtSign)
{
  return NULL;
}

char * fcvt (double _Val, int _NumOfDec, int * _PtDec, int * _PtSign)
{
  return NULL;
}

char * gcvt (double _Val, int _NumOfDigits, char * _DstBuf)
{
  return NULL;
}
#endif


using namespace std;
namespace CommonLib
{

int __vfprintf(out_str_stream/*wostringstream*/ &f, const wchar_t *s, va_list args);
/*
 * pANS stdio -- sprintf
 */
int __sprintf(wchar_t *buf, int nbuf, const wchar_t *fmt, ...)
{
  int n;
  va_list args;
  //wostringstream f;
  out_str_stream_buf f(buf, (size_t)nbuf);
  va_start(args, fmt);
  n=__vfprintf(f, fmt, args);
  va_end(args);
  //wcsncpy (buf, f.str ().c_str (), nbuf);
  return n;
}

/*
 * pANS stdio -- vsnprintf
 */
int __vsnprintf(wchar_t *buf, int nbuf, const wchar_t *fmt, va_list args)
{
  int n;
  //wostringstream f;
  out_str_stream_buf f(buf, (size_t)nbuf);
  n=__vfprintf(f, fmt, args);
  if (n > nbuf)
    n = nbuf;
  //wcsncpy (buf, f.str ().c_str (), n);
  return n;
}

/*
 * pANS stdio -- vsnprintf
 */
int __vosnprintf(out_str_stream/*wostringstream*/ &f, const wchar_t *fmt, va_list args)
{
  int n = __vfprintf(f, fmt, args);
  return n;
}

#undef putc
#define putc(c,f) (f<<(c))
#undef fputs
#define fputs(c,f) (f<<(c))
/*
 * pANS stdio -- vfprintf
 */
/*
 * Leading flags
 */
#define        SPACE        1                /* ' ' prepend space if no sign printed */
#define        ALT          2                /* '#' use alternate conversion */
#define        SIGN         4                /* '+' prepend sign, even if positive */
#define        LEFT         8                /* '-' left-justify */
#define        ZPAD        16                /* '0' zero-pad */
/*
 * Trailing flags
 */
#define        SHORT        32                /* 'h' convert a short integer */
#define        LONG         64                /* 'l' convert a long integer */
#define        LDBL        128                /* 'L' convert a long double */
#define        PTR         256                /*     convert a void * (%p) */

static int lflag[] = {        /* leading flags */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^@ ^A ^B ^C ^D ^E ^F ^G */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^H ^I ^J ^K ^L ^M ^N ^O */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^P ^Q ^R ^S ^T ^U ^V ^W */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^X ^Y ^Z ^[ ^\ ^] ^^ ^_ */
SPACE,    0,        0,        ALT,      0,        0,        0,        0,        /* sp  !  "  #  $  %  &  ' */
0,        0,        0,        SIGN,     0,        LEFT,     0,        0,        /*  (  )  *  +  ,  -  .  / */
ZPAD,     0,        0,        0,        0,        0,        0,        0,        /*  0  1  2  3  4  5  6  7 */
0,        0,        0,        0,        0,        0,        0,        0,        /*  8  9  :  ;  <  =  >  ? */
0,        0,        0,        0,        0,        0,        0,        0,        /*  @  A  B  C  D  E  F  G */
0,        0,        0,        0,        0,        0,        0,        0,        /*  H  I  J  K  L  M  N  O */
0,        0,        0,        0,        0,        0,        0,        0,        /*  P  Q  R  S  T  U  V  W */
0,        0,        0,        0,        0,        0,        0,        0,        /*  X  Y  Z  [  \  ]  ^  _ */
0,        0,        0,        0,        0,        0,        0,        0,        /*  `  a  b  c  d  e  f  g */
0,        0,        0,        0,        0,        0,        0,        0,        /*  h  i  j  k  l  m  n  o */
0,        0,        0,        0,        0,        0,        0,        0,        /*  p  q  r  s  t  u  v  w */
0,        0,        0,        0,        0,        0,        0,        0,        /*  x  y  z  {  |  }  ~ ^? */

0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
};

static int tflag[] = {        /* trailing flags */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^@ ^A ^B ^C ^D ^E ^F ^G */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^H ^I ^J ^K ^L ^M ^N ^O */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^P ^Q ^R ^S ^T ^U ^V ^W */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^X ^Y ^Z ^[ ^\ ^] ^^ ^_ */
0,        0,        0,        0,        0,        0,        0,        0,        /* sp  !  "  #  $  %  &  ' */
0,        0,        0,        0,        0,        0,        0,        0,        /*  (  )  *  +  ,  -  .  / */
0,        0,        0,        0,        0,        0,        0,        0,        /*  0  1  2  3  4  5  6  7 */
0,        0,        0,        0,        0,        0,        0,        0,        /*  8  9  :  ;  <  =  >  ? */
0,        0,        0,        0,        0,        0,        0,        0,        /*  @  A  B  C  D  E  F  G */
0,        0,        0,        0,        LDBL,     0,        0,        0,        /*  H  I  J  K  L  M  N  O */
0,        0,        0,        0,        0,        0,        0,        0,        /*  P  Q  R  S  T  U  V  W */
0,        0,        0,        0,        0,        0,        0,        0,        /*  X  Y  Z  [  \  ]  ^  _ */
0,        0,        0,        0,        0,        0,        0,        0,        /*  `  a  b  c  d  e  f  g */
SHORT,    0,        0,        0,        LONG,     0,        0,        0,        /*  h  i  j  k  l  m  n  o */
0,        0,        0,        0,        0,        0,        0,        0,        /*  p  q  r  s  t  u  v  w */
0,        0,        0,        0,        0,        0,        0,        0,        /*  x  y  z  {  |  }  ~ ^? */

0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
};

static int ocvt_E(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_G(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_X(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_c(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_d(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_e(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_f(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_g(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_n(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_o(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_p(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_s(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_u(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);
static int ocvt_x(out_str_stream/*wostringstream*/ &, va_list *, int, int, int);

static int(*ocvt[])(out_str_stream/*wostringstream*/ &, va_list *, int, int, int) = {
0,        0,        0,        0,        0,        0,        0,        0,        /* ^@ ^A ^B ^C ^D ^E ^F ^G */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^H ^I ^J ^K ^L ^M ^N ^O */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^P ^Q ^R ^S ^T ^U ^V ^W */
0,        0,        0,        0,        0,        0,        0,        0,        /* ^X ^Y ^Z ^[ ^\ ^] ^^ ^_ */
0,        0,        0,        0,        0,        0,        0,        0,        /* sp  !  "  #  $  %  &  ' */
0,        0,        0,        0,        0,        0,        0,        0,        /*  (  )  *  +  ,  -  .  / */
0,        0,        0,        0,        0,        0,        0,        0,        /*  0  1  2  3  4  5  6  7 */
0,        0,        0,        0,        0,        0,        0,        0,        /*  8  9  :  ;  <  =  >  ? */
0,        0,        0,        0,        0,        ocvt_E,   0,        ocvt_G,   /*  @  A  B  C  D  E  F  G */
0,        0,        0,        0,        0,        0,        0,        0,        /*  H  I  J  K  L  M  N  O */
0,        0,        0,        0,        0,        0,        0,        0,        /*  P  Q  R  S  T  U  V  W */
ocvt_X,   0,        0,        0,        0,        0,        0,        0,        /*  X  Y  Z  [  \  ]  ^  _ */
0,        0,        0,        ocvt_c,   ocvt_d,   ocvt_e,   ocvt_f,   ocvt_g,   /*  `  a  b  c  d  e  f  g */
0,        ocvt_d,   0,        0,        0,        0,        ocvt_n,   ocvt_o,   /*  h  i  j  k  l  m  n  o */
ocvt_p,   0,        0,        ocvt_s,   0,        ocvt_u,   0,        0,        /*  p  q  r  s  t  u  v  w */
ocvt_x,   0,        0,        0,        0,        0,        0,        0,        /*  x  y  z  {  |  }  ~ ^? */

0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
0,        0,        0,        0,        0,        0,        0,        0,
};

static int nprint;


int
__vfprintf(out_str_stream/*wostringstream*/ &f, const wchar_t *s, va_list args)
{
        int flags, width, precision;

        nprint = 0;
        while(*s){
                if(*s != '%'){
                        putc(*s++, f);
                        nprint++;
                        continue;
                }
                s++;
                flags = 0;
                while(lflag[*s&_IO_CHMASK]) flags |= lflag[*s++&_IO_CHMASK];
                if(*s == '*'){
                        width = va_arg(args, int);
                        s++;
                        if(width<0){
                                flags |= LEFT;
                                width = -width;
                        }
                }
                else{
                        width = 0;
                        while('0'<=*s && *s<='9') width = width*10 + *s++ - '0';
                }
                if(*s == '.'){
                        s++;
                        if(*s == '*'){
                                precision = va_arg(args, int);
                                s++;
                        }
                        else{
                                precision = 0;
                                while('0'<=*s && *s<='9') precision = precision*10 + *s++ - '0';
                        }
                }
                else
                        precision = -1;
                while(tflag[*s&_IO_CHMASK]) flags |= tflag[*s++&_IO_CHMASK];
                if(ocvt[*s]) nprint += (*ocvt[*s++])(f, &args, flags, width, precision);
                else if(*s){
                        putc(*s++, f);
                        nprint++;
                }
        }

        return nprint;
}

static int
ocvt_c(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
  int i;

  if(!(flags&LEFT)) for(i=1; i<width; i++) putc(' ', f);
  putc((wchar_t)va_arg(*args, int), f);
  if(flags&LEFT) for(i=1; i<width; i++) putc(' ', f);
  return width<1 ? 1 : width;
}

static int
ocvt_s(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        int i, n = 0;
        wchar_t *s;

        s = va_arg(*args, wchar_t *);
        if(!(flags&LEFT)){
                if(precision >= 0)
                        for(i=0; i!=precision && s[i]; i++);
                else
                        for(i=0; s[i]; i++);
                for(; i<width; i++){
                        putc(' ', f);
                        n++;
                }
        }
        if(precision >= 0){
                for(i=0; i!=precision && *s; i++){
                        putc(*s++, f);
                        n++;
                }
        } else{
                for(i=0;*s;i++){
                        putc(*s++, f);
                        n++;
                }
        }
        if(flags&LEFT){
                for(; i<width; i++){
                        putc(' ', f);
                        n++;
                }
        }
        return n;
}

static int
ocvt_n(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        if(flags&SHORT)
                *va_arg(*args, short *) = nprint;
        else if(flags&LONG)
                *va_arg(*args, long *) = nprint;
        else
                *va_arg(*args, int *) = nprint;
        return 0;
}

/*
 * Generic fixed-point conversion
 *        f is the output wostringstream &;
 *        args is the va_list * from which to get the number;
 *        flags, width and precision are the results of printf-cracking;
 *        radix is the number base to print in;
 *        alphabet is the set of digits to use;
 *        prefix is the prefix to print before non-zero numbers when
 *        using ``alternate form.''
 */
static int
ocvt_fixed(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision,
        int radix, int sgned, wchar_t alphabet[], wchar_t *prefix)
{
        wchar_t digits[128];        /* no reasonable machine will ever overflow this */
        wchar_t *sign;
        wchar_t *dp;
        long snum;
        unsigned long num;
        int nout, npad, nlzero;

        if(sgned){
                if(flags&PTR) snum = (long)va_arg(*args, void *);
                else if(flags&SHORT) snum = va_arg(*args, short);
                else if(flags&LONG) snum = va_arg(*args, long);
                else snum = va_arg(*args, int);
                if(snum < 0){
                        sign = L"-";
                        num = -snum;
                } else{
                        if(flags&SIGN) sign = L"+";
                        else if(flags&SPACE) sign = L" ";
                        else sign = L"";
                        num = snum;
                }
        } else {
                sign = L"";
                if(flags&PTR) num = (long)va_arg(*args, void *);
                else if(flags&SHORT) num = va_arg(*args, unsigned short);
                else if(flags&LONG) num = va_arg(*args, unsigned long);
                else num = va_arg(*args, unsigned int);
        }
        if(num == 0) prefix = L"";
        dp = digits;
        do{
                *dp++ = alphabet[num%radix];
                num /= radix;
        }while(num);
        if(precision==0 && dp-digits==1 && dp[-1]=='0')
                dp--;
        nlzero = precision-(dp-digits);
        if(nlzero < 0) nlzero = 0;
        if(flags&ALT){
                if(radix == 8) if(dp[-1]=='0' || nlzero) prefix = L"";
        }
        else prefix = L"";
        nout = dp-digits+nlzero+wcslen(prefix)+wcslen(sign);
        npad = width-nout;
        if(npad < 0) npad = 0;
        nout += npad;
        if(!(flags&LEFT)){
                if(flags&ZPAD && precision <= 0){
                        fputs(sign, f);
                        fputs(prefix, f);
                        while(npad){
                                putc('0', f);
                                --npad;
                        }
                } else{
                        while(npad){
                                putc(' ', f);
                                --npad;
                        }
                        fputs(sign, f);
                        fputs(prefix, f);
                }
                while(nlzero){
                        putc('0', f);
                        --nlzero;
                }
                while(dp!=digits) putc(*--dp, f);
        }
        else{
                fputs(sign, f);
                fputs(prefix, f);
                while(nlzero){
                        putc('0', f);
                        --nlzero;
                }
                while(dp != digits) putc(*--dp, f);
                while(npad){
                        putc(' ', f);
                        --npad;
                }
        }
        return nout;        
}

static int
ocvt_X(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags, width, precision, 16, 0, L"0123456789ABCDEF", L"0X");
}

static int
ocvt_d(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags, width, precision, 10, 1, L"0123456789", L"");
}

static int
ocvt_o(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags, width, precision, 8, 0, L"01234567", L"0");
}

static int
ocvt_p(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags|PTR|ALT, width, precision, 16, 0,
                L"0123456789ABCDEF", L"0X");
}

static int
ocvt_u(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags, width, precision, 10, 0, L"0123456789", L"");
}

static int
ocvt_x(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_fixed(f, args, flags, width, precision, 16, 0, L"0123456789abcdef", L"0x");
}

static int ocvt_flt(out_str_stream/*wostringstream*/ &, va_list *, int, int, int, wchar_t);

static int
ocvt_E(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_flt(f, args, flags, width, precision, 'E');
}

static int
ocvt_G(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_flt(f, args, flags, width, precision, 'G');
}

static int
ocvt_e(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_flt(f, args, flags, width, precision, 'e');
}

static int
ocvt_f(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_flt(f, args, flags, width, precision, 'f');
}

static int
ocvt_g(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int width, int precision)
{
        return ocvt_flt(f, args, flags, width, precision, 'g');
}


static char *
Fill(char	*obuf, int Width, int LeftJustify, int Zero)
{
	int W = (int)strlen(obuf);
	int	diff = Width - W;
	 /* LeftJustify means there was a '-' flag, and in that case,	*/
	 /* printf man page (UTS4.4) says ignore '0'			*/
	char	FillChar = (Zero && !LeftJustify) ? '0' : ' ';
	int	i;
	int	LeftFill = ! LeftJustify;

	if(Width <= W) 
	  return obuf;

	if(LeftFill) {
		memmove(obuf+diff, obuf, W+1); /* "+1" to get '\0' at end */
		for(i=0 ; i < diff ; ++i) { obuf[i] = FillChar; }
	} else {
		for(i=W ; i < Width ; ++i)
			obuf[i] = FillChar;
		obuf[Width] = '\0';
	}
	return obuf;
}


static char	*
do_efmt(double d, char	*obuf, int Width, int Precision, int Alt, int Plus, int Minus, int Zero, int UpperCase)
{
	char	*Ecvt;
	char	*ob;
	int	decpt, sign, E;
	size_t	len;
	int	AllZeroes = 0;

  Ecvt = _ecvt (d, Precision+1, &decpt, &sign);

	if(!Ecvt) 
	  { *obuf = '\0'; return obuf; }
	/* fprintf(stderr, "decpt=%d, sign=%d\n", decpt, sign); */

	len = strlen(Ecvt);
	if(strspn(Ecvt, "0") == len) 
	  AllZeroes = 1;

	ob = obuf;
	if(sign)	*ob++ = '-';
	else if(Plus)	*ob++ = '+';

	*ob++ = Ecvt[0];

	if(Precision > 0 || Alt) 
	  *ob++ = '.';
	strcpy(ob, &Ecvt[1]);

	ob += strlen(ob);	/* ADVANCE TO END OF WHAT WE JUST ADDED */
	*ob++ = UpperCase ? 'E' : 'e';

	if(AllZeroes)	
	  E = 0;
	else		
	  E = decpt - 1;

	if(E < 0)	
	  { *ob++ = '-'; E = -E; }
	else		
	  { *ob++ = '+'; }

	sprintf(ob, "%.2d", E);	/* Too much horsepower used here */

	int	W = (int)strlen(obuf);
	size_t dot = strcspn (obuf, ",.;");
	if (W != dot)
	  obuf[dot] = '.';

	if(Width > W) 
	  return Fill(obuf, Width, Minus, Zero);
	else			 
	  return obuf;
}

static char	*
do_gfmt(double d, char	*obuf, int Width, int Precision, int Alt, int Plus, int Minus, int Zero, int UpperCase)
{
	char	*Ecvt = _gcvt (d, Precision ? Precision : 1, obuf);
	size_t	len = strlen(obuf);

	 /* gcvt fails (maybe give a warning? For now return empty string): */
	if(!Ecvt) 
	  { *obuf = '\0'; return obuf; }

	/* printf("Ecvt='%s'\n", Ecvt); */
	if(Plus && (Ecvt[0] != '-')) {
		memmove(obuf+1, obuf, len+1); /* "+1" to get '\0' at end */
		obuf[0] = '+';
		++len;
	}
	if(Alt && !strchr(Ecvt, '.')) {
		size_t	LenUpTo_E = strcspn(obuf, "eE");
		size_t	E_etc_len = strlen(&obuf[LenUpTo_E]);
			/* ABOVE: Will be 0 if there's no E/e because */
			/* strcspn will return length of whole string */

		if(E_etc_len)
			memmove(obuf+LenUpTo_E+1, obuf+LenUpTo_E, E_etc_len);
		obuf[LenUpTo_E] = '.';
		obuf[LenUpTo_E + 1 + E_etc_len ] = '\0';
	}
	{ char *E_loc;
	  if(UpperCase && (E_loc = strchr(obuf, 'e'))) { *E_loc = 'E'; }
	}
	size_t	W = strlen(obuf);
	size_t dot = strcspn (obuf, ",.;");
	if (W != dot)
	  obuf[dot] = '.';

	if(Width > int(W)) 
		return Fill(obuf, Width, Minus, Zero);
	else
		return obuf;
}


static int
ocvt_flt(out_str_stream/*wostringstream*/ &f, va_list *args, int flags, int Width, int Precision, wchar_t afmt)
{
	int	
	  Alt = flags & ALT, 
	  Plus = flags & SIGN, 
	  Minus = flags & LEFT, 
	  Zero = flags & ZPAD;
	int	FmtChar = afmt, BaseFmt = tolower (afmt);
	wchar_t	*AfterWidth = 0, *AfterPrecision = 0;
  double d;
  d = va_arg(*args, double);
  if(Precision < 0) 
    Precision = 6;
    
  char *obuf = (char*)_alloca ((Width+10)*sizeof (char*));
  *obuf = 0;

	if(BaseFmt == 'e') {
		do_efmt(d, obuf, Width, Precision, Alt,
			Plus, Minus, Zero, FmtChar == 'E');
	} else {
		do_gfmt(d, obuf, Width, Precision, Alt,
			Plus, Minus, Zero, FmtChar == 'G');
	}
	while (*obuf)
	  {
    	f << wchar_t(*obuf++);
    }
	return (int)strlen (obuf);

#if 0
        int echr;
        wchar_t *digits, *edigits;
        int exponent;
        wchar_t fmt;
        int sign;
        int ndig;
        int nout, i;
        wchar_t ebuf[20];        /* no sensible machine will overflow this */
        wchar_t *eptr;
        double d;

        echr = 'e';
        fmt = afmt;
        d = va_arg(*args, double);
        if(precision < 0) precision = 6;
        switch(fmt){
        case 'f':
                digits = dtoa(d, 3, precision, &exponent, &sign, &edigits);
                break;
        case 'E':
                echr = 'E';
                fmt = 'e';
                /* fall through */
        case 'e':
                digits = dtoa(d, 2, 1+precision, &exponent, &sign, &edigits);
                break;
        case 'G':
                echr = 'E';
                /* fall through */
        case 'g':
                if (precision > 0)
                        digits = dtoa(d, 2, precision, &exponent, &sign, &edigits);
                else {
                        digits = dtoa(d, 0, precision, &exponent, &sign, &edigits);
                        precision = edigits - digits;
                        if (exponent > precision && exponent <= precision + 4)
                                precision = exponent;
                        }
                if(exponent >= -3 && exponent <= precision){
                        fmt = 'f';
                        precision -= exponent;
                }else{
                        fmt = 'e';
                        --precision;
                }
                break;
        }
        if (exponent == 9999) {
                /* Infinity or Nan */
                precision = 0;
                exponent = edigits - digits;
                fmt = 'f';
        }
        ndig = edigits-digits;
        if((afmt=='g' || afmt=='G') && !(flags&ALT)){        /* knock off trailing zeros */
                if(fmt == 'f'){
                        if(precision+exponent > ndig) {
                                precision = ndig - exponent;
                                if(precision < 0)
                                        precision = 0;
                        }
                }
                else{
                        if(precision > ndig-1) precision = ndig-1;
                }
        }
        nout = precision;                                /* digits after decimal point */
        if(precision!=0 || flags&ALT) nout++;                /* decimal point */
        if(fmt=='f' && exponent>0) nout += exponent;        /* digits before decimal point */
        else nout++;                                        /* there's always at least one */
        if(sign || flags&(SPACE|SIGN)) nout++;                /* sign */
        if(fmt != 'f'){                                        /* exponent */
                eptr = ebuf;
                for(i=exponent<=0?1-exponent:exponent-1; i; i/=10)
                        *eptr++ = '0' + i%10;
                while(eptr<ebuf+2) *eptr++ = '0';
                nout += eptr-ebuf+2;                        /* e+99 */
        }
        if(!(flags&ZPAD) && !(flags&LEFT))
                while(nout < width){
                        putc(' ', f);
                        nout++;
                }
        if(sign) putc('-', f);
        else if(flags&SIGN) putc('+', f);
        else if(flags&SPACE) putc(' ', f);
        if(flags&ZPAD)
                while(nout < width){
                        putc('0', f);
                        nout++;
                }
        if(fmt == 'f'){
                for(i=0; i<exponent; i++) putc(i<ndig?digits[i]:'0', f);
                if(i == 0) putc('0', f);
                if(precision>0 || flags&ALT) putc('.', f);
                for(i=0; i!=precision; i++)
                        putc(0<=i+exponent && i+exponent<ndig?digits[i+exponent]:'0', f);
        }
        else{
                putc(digits[0], f);
                if(precision>0 || flags&ALT) putc('.', f);
                for(i=0; i!=precision; i++) putc(i<ndig-1?digits[i+1]:'0', f);
        }
        if(fmt != 'f'){
                putc(echr, f);
                putc(exponent<=0?'-':'+', f);
                while(eptr>ebuf) putc(*--eptr, f);
        }
        while(nout < width){
                putc(' ', f);
                nout++;
        }
        return nout;
#endif        
}


#define isALNUM(c)    (isALPHA(c) || isDIGIT(c) || (c) == '_')
#define isIDFIRST(c)    (isALPHA(c) || (c) == '_')
#define isALPHA(c)    (isUPPER(c) || isLOWER(c))
#define isSPACE(c) \
    ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) =='\r' || (c) == '\f')
#define isPSXSPC(c)    (isSPACE(c) || (c) == '\v')
#define isBLANK(c)    ((c) == ' ' || (c) == '\t')
#define isDIGIT(c)    ((c) >= '0' && (c) <= '9')

static bool
grok_numeric_radix(const char **sp, const char *send)
{
  if (*sp < send && **sp == '.') 
    {
      ++*sp;
      return true;
    }
  return false;
}

static double
S_mulexp10(double value, int exponent)
{
  double result = 1.0;
  double power = 10.0;
  bool negative = 0;
  int bit;

  if (exponent == 0)
    return value;
  if (value == 0)
    return (double)0;

  if (exponent < 0) 
    {
      negative = 1;
      exponent = -exponent;
    }
    for (bit = 1; exponent; bit <<= 1) 
      {
      if (exponent & bit) 
        {
          exponent ^= bit;
          result *= power;
        /* Floating point exceptions are supposed to be turned off,
         *  but if we're obviously done, don't risk another iteration.  
         */
          if (exponent == 0) 
            break;
          }
        power *= power;
    }
  return negative ? value / result : value * result;
}


double
__atof(const char* orig)
{
    double result[3] = {0.0, 0.0, 0.0};
    const char* s = orig;

    unsigned accumulator[2] = {0,0};    /* before/after dp */
    bool negative = 0;
    const char* send = s + strlen(orig) - 1;
    bool seen_digit = 0;
    int exp_adjust[2] = {0,0};
    int exp_acc[2] = {-1, -1};
    /* the current exponent adjust for the accumulators */
    int exponent = 0;
    int    seen_dp  = 0;
    int digit = 0;
    int old_digit = 0;
    int sig_digits = 0; /* noof significant digits seen so far */

/* There is no point in processing more significant digits
 * than the double can hold. Note that NV_DIG is a lower-bound value,
 * while we need an upper-bound value. We add 2 to account for this;
 * since it will have been conservative on both the first and last digit.
 * For example a 32-bit mantissa with an exponent of 4 would have
 * exact values in the set
 *               4
 *               8
 *              ..
 *     17179869172
 *     17179869176
 *     17179869180
 *
 * where for the purposes of calculating NV_DIG we would have to discount
 * both the first and last digit, since neither can hold all values from
 * 0..9; but for calculating the value we must examine those two digits.
 */
#define MAX_SIG_DIGITS (DBL_DIG+2)

/* the max number we can accumulate in a UV, and still safely do 10*N+9 */
#define MAX_ACCUMULATE ( (unsigned) ((UINT_MAX - 9)/10))

    /* leading whitespace */
    while (isSPACE(*s))
      ++s;

    /* sign */
    switch (*s) {
    case '-':
        negative = 1;
        /* fall through */
    case '+':
        ++s;
    }


    /* we accumulate digits into an integer; when this becomes too
     * large, we add the total to double and start again */

    while (1) {
    if (isDIGIT(*s)) {
        seen_digit = 1;
        old_digit = digit;
        digit = *s++ - '0';
        if (seen_dp)
        exp_adjust[1]++;

        /* don't start counting until we see the first significant
         * digit, eg the 5 in 0.00005... */
        if (!sig_digits && digit == 0)
        continue;

        if (++sig_digits > MAX_SIG_DIGITS) {
        /* limits of precision reached */
            if (digit > 5) {
            ++accumulator[seen_dp];
        } else if (digit == 5) {
            if (old_digit % 2) { /* round to even - Allen */
            ++accumulator[seen_dp];
            }
        }
        if (seen_dp) {
            exp_adjust[1]--;
        } else {
            exp_adjust[0]++;
        }
        /* skip remaining digits */
        while (isDIGIT(*s)) {
            ++s;
            if (! seen_dp) {
            exp_adjust[0]++;
            }
        }
        /* warn of loss of precision? */
        }
        else {
        if (accumulator[seen_dp] > MAX_ACCUMULATE) {
            /* add accumulator to result and start again */
            result[seen_dp] = S_mulexp10(result[seen_dp],
                         exp_acc[seen_dp])
            + (double)accumulator[seen_dp];
            accumulator[seen_dp] = 0;
            exp_acc[seen_dp] = 0;
        }
        accumulator[seen_dp] = accumulator[seen_dp] * 10 + digit;
        ++exp_acc[seen_dp];
        }
    }
    else if (!seen_dp && grok_numeric_radix (&s, send)) {
        seen_dp = 1;
        if (sig_digits > MAX_SIG_DIGITS) {
        ++s;
        while (isDIGIT(*s)) {
            ++s;
        }
        break;
        }
    }
    else {
        break;
    }
    }

    result[0] = S_mulexp10(result[0], exp_acc[0]) + (double)accumulator[0];
    if (seen_dp) {
    result[1] = S_mulexp10(result[1], exp_acc[1]) + (double)accumulator[1];
    }

    if (seen_digit && (*s == 'e' || *s == 'E')) {
    bool expnegative = 0;

    ++s;
    switch (*s) {
        case '-':
        expnegative = 1;
        /* fall through */
        case '+':
        ++s;
    }
    while (isDIGIT(*s))
        exponent = exponent * 10 + (*s++ - '0');
    if (expnegative)
        exponent = -exponent;
    }



    /* now apply the exponent */

    if (seen_dp) {
    result[2] = S_mulexp10(result[0],exponent+exp_adjust[0])
        + S_mulexp10(result[1],exponent-exp_adjust[1]);
    } else {
    result[2] = S_mulexp10(result[0],exponent+exp_adjust[0]);
    }

    /* now apply the sign */
    if (negative)
    result[2] = -result[2];

    //*value = result[2];
    return result[2];
}

double __wtof (const wchar_t *str)
{
  size_t l = wcslen (str);
  if (l > 32)
    l = 32;
  char *buf = (char*)_alloca (l+1);
  for (size_t i = 0; i < l; i++)
    buf[i] = (char)str[i];
  buf[l] = 0;
  return __atof (buf);
}

}