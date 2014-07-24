#include "stdafx.h"

#include "toupper.incl"
#include "tolower.incl"

namespace CommonLib {

wchar_t
toupper (wchar_t c)
{
  int ret = 0;
  return (toupper_gethash (c, ret))?ret:c;
}

wchar_t
tolower (wchar_t c)
{
  int ret = 0;
  return (tolower_gethash (c, ret))?ret:c;
}

int
toupper (wchar_t *c)
{
  int len = 0;
  while (*c)
    {
      *c = toupper (*c);
      len++;
      c++;
    }
  return len;
}

int
tolower (wchar_t *c)
{
  int len = 0;
  while (*c)
    {
      *c = tolower (*c);
      len++;
      c++;
    }
  return len;
}

};