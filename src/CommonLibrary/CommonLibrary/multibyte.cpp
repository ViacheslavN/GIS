#include "stdafx.h"
#include <stdlib.h>
#include "multibyte.h"

#ifdef DEBUG_NEW
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif//def(_DEBUG)
#endif//def(DEBUG_NEW)

namespace CommonLib
{

const int encoding_mask[] =
{
  ~0x7ff, 
  ~0xffff, 
  ~0x1fffff, 
  ~0x3ffffff
};

const unsigned char encoding_byte[] =
{
  0xc0, 0xe0, 0xf0, 0xf8, 0xfc
};


long
dest_mbsnrtowcs (wchar_t *dst, unsigned char **src, size_t nmc, size_t len)
{
  long written = 0;
  char *run = (char *) *src;
  char *last = run + nmc;
  
  if (dst == NULL)
    /* The LEN parameter has to be ignored if we don't actually write
       anything.  */
    len = 0xffffffff;

  /* Copy all words.  */
  long wcLen = (long)(len / sizeof(wchar_t));
  while (written < wcLen && run < last)
    {
      wchar_t value;
      size_t count;
      unsigned char byte = *run++;

      /* We expect a start of a new multibyte character.  */
      if (byte < 0x80)
	{
	  /* One byte sequence.  */
	  count = 0;
	  value = byte;
	}
      else if ((byte & 0xe0) == 0xc0)
	{
	  count = 1;
	  value = byte & 0x1f;
	}
      else if ((byte & 0xf0) == 0xe0)
	{
	  /* We expect three bytes.  */
	  count = 2;
	  value = byte & 0x0f;
	}
      else if ((byte & 0xf8) == 0xf0)
	{
	  /* We expect four bytes.  */
	  count = 3;
	  value = byte & 0x07;
	}
      else if ((byte & 0xfc) == 0xf8)
	{
	  /* We expect five bytes.  */
	  count = 4;
	  value = byte & 0x03;
	}
      else if ((byte & 0xfe) == 0xfc)
	{
	  /* We expect six bytes.  */
	  count = 5;
	  value = byte & 0x01;
	}
      else
	{
	  /* This is an illegal encoding.  */
	  /* errno = (EILSEQ); */
	  return (size_t) -1;
	}

      /* Read the possible remaining bytes.  */
      while (count-- > 0)
	{
	  byte = *run++;

	  if ((byte & 0xc0) != 0x80)
	    {
	      /* This is an illegal encoding.  */
	      /* errno = (EILSEQ); */
	      return (size_t) -1;
	    }

	  value <<= 6;
	  value |= byte & 0x3f;
	}

      /* Store value is required.  */
      if (dst != NULL)
	*dst++ = value;

      if (value == L'\0')
	{
	  /* Found the end of the string.  */
	  *src = (unsigned char*)run;
	  return written*sizeof(wchar_t);
	}

      /* Increment counter of produced words.  */
      ++written;
    }

  /* Store address of next byte to process.  */
  *src = (unsigned char *) run;

  return written*sizeof(wchar_t);
}


long
dest_mbslen (const unsigned char *src, size_t nmc)
{
  long written = 0;
  char *run = (char *) src;
  char *last = run + nmc;
  
  /* Copy all words.  */
  while (run < last)
    {
      wchar_t value;
      size_t count;
      unsigned char byte = *run++;

      /* We expect a start of a new multibyte character.  */
      if (byte < 0x80)
	{
	  /* One byte sequence.  */
	  count = 0;
	  value = byte;
	}
      else if ((byte & 0xe0) == 0xc0)
	{
	  count = 1;
	  value = byte & 0x1f;
	}
      else if ((byte & 0xf0) == 0xe0)
	{
	  /* We expect three bytes.  */
	  count = 2;
	  value = byte & 0x0f;
	}
      else if ((byte & 0xf8) == 0xf0)
	{
	  /* We expect four bytes.  */
	  count = 3;
	  value = byte & 0x07;
	}
      else if ((byte & 0xfc) == 0xf8)
	{
	  /* We expect five bytes.  */
	  count = 4;
	  value = byte & 0x03;
	}
      else if ((byte & 0xfe) == 0xfc)
	{
	  /* We expect six bytes.  */
	  count = 5;
	  value = byte & 0x01;
	}
      else
	{
	  /* This is an illegal encoding.  */
	  /* errno = (EILSEQ); */
	  return (size_t) -1;
	}

      /* Read the possible remaining bytes.  */
      while (count-- > 0)
	{
	  byte = *run++;

	  if ((byte & 0xc0) != 0x80)
	    {
	      /* This is an illegal encoding.  */
	      /* errno = (EILSEQ); */
	      return (size_t) -1;
	    }

	  value <<= 6;
	  value |= byte & 0x3f;
	}

      /* Store value is required.  */
    if (value == L'\0')
	{
	  /* Found the end of the string.  */
	  return written;
	}

      /* Increment counter of produced words.  */
      ++written;
    }

  return written;
}

long
dest_mbslen (const unsigned char *src)
{
  long written = 0;
  char *run = (char *) src;
  
  /* Copy all words.  */
  for (;;)
    {
      wchar_t value;
      size_t count;
      unsigned char byte = *run++;

      /* We expect a start of a new multibyte character.  */
      if (byte < 0x80)
	{
	  /* One byte sequence.  */
	  count = 0;
	  value = byte;
	}
      else if ((byte & 0xe0) == 0xc0)
	{
	  count = 1;
	  value = byte & 0x1f;
	}
      else if ((byte & 0xf0) == 0xe0)
	{
	  /* We expect three bytes.  */
	  count = 2;
	  value = byte & 0x0f;
	}
      else if ((byte & 0xf8) == 0xf0)
	{
	  /* We expect four bytes.  */
	  count = 3;
	  value = byte & 0x07;
	}
      else if ((byte & 0xfc) == 0xf8)
	{
	  /* We expect five bytes.  */
	  count = 4;
	  value = byte & 0x03;
	}
      else if ((byte & 0xfe) == 0xfc)
	{
	  /* We expect six bytes.  */
	  count = 5;
	  value = byte & 0x01;
	}
      else
	{
	  /* This is an illegal encoding.  */
	  /* errno = (EILSEQ); */
	  return (size_t) -1;
	}

      /* Read the possible remaining bytes.  */
      while (count-- > 0)
	{
	  byte = *run++;

	  if ((byte & 0xc0) != 0x80)
	    {
	      /* This is an illegal encoding.  */
	      /* errno = (EILSEQ); */
	      return (size_t) -1;
	    }

	  value <<= 6;
	  value |= byte & 0x3f;
	}

      /* Store value is required.  */
  if (value == L'\0')
	{
	  return written;
	}

      /* Increment counter of produced words.  */
      ++written;
    }

  return written;
}



long
dest_wcsnrtombs (unsigned char *dst, wchar_t **src, size_t nwc, size_t len)
{
  long written = 0;
  wchar_t *run = src[0];

  if (dst == NULL)
    /* The LEN parameter has to be ignored if we don't actually write
       anything.  */
    len = 0xffffffff;

  while (written < (long)len && nwc-- > 0)
    {
      wchar_t wc = run[0];
      run++;

      if (wc & ~0x7fffffff)
        {
	  /* This is no correct ISO 10646 character.  */
	  /* errno = (EILSEQ); */
          return (size_t) -1;
        }
      else if (!(wc & ~0x7f))
        {
	  /* It's an one byte sequence.  */
          if (dst != NULL)
            *dst++ = (char) wc;
          ++written;
        }
      else
        {
	  long step;

	  for (step = 2; step < 6; ++step)
	    if ((wc & encoding_mask[step - 2]) == 0)
	      break;

	  if (written + step >= (long)len)
	    {
	      /* Too long.  */
	      run -= 1;
	      break;
	    }

	  if (dst != NULL)
	    {
	      long cnt = step;

	      dst[0] = encoding_byte[cnt - 2];

	      --cnt;
	      do
		{
		  dst[cnt] = (unsigned char) (0x80 | (wc & 0x3f));
		  wc >>= 6;
		}
	      while (--cnt > 0);
	      dst[0] |= wc;

	      dst += step;
	    }

	  written += step;
	}
    }

  /* Store position of first unprocessed word.  */
  *src = run;

  return written;
}



long dest_wcsmblen (const wchar_t *src)
{
  if (NULL == src)
    return 0;
  long written = 0;
  const wchar_t *run = src;

  while (run[0] != L'\0')
    {
      wchar_t wc = run[0];
      run++;

      if (wc & ~0x7fffffff)
        {
	  /* This is no correct ISO 10646 character.  */
	  /* errno = (EILSEQ); */
          return (size_t) -1;
        }
      if (!(wc & ~0x7f))
        {
	  /* It's an one byte sequence.  */
          ++written;
        }
      else
        {
          long step;

          for (step = 2; step < 6; ++step)
          if ((wc & encoding_mask[step - 2]) == 0)
            break;

          written += step;
        }
    }
  return written;
}


long
dest_wcsmblen (const wchar_t *src, size_t nwc)
{
  long written = 0;
  const wchar_t *run = src;

  while (run[0] && nwc-- > 0)
    {
      wchar_t wc = run[0];
      run++;

      if (wc & ~0x7fffffff)
        {
	  /* This is no correct ISO 10646 character.  */
	  /* errno = (EILSEQ); */
          return (size_t) -1;
    	}
      if (!(wc & ~0x7f))
        {
	  /* It's an one byte sequence.  */
          ++written;
        }
      else
        {
          long step;
          for (step = 2; step < 6; ++step)
            if ((wc & encoding_mask[step - 2]) == 0)
              break;

          written += step;
        }
    }
  return written;
}

}