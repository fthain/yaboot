#include "string.h"

char *
stpncpy (char *dest, const char *src, size_t n)
{
  char *res;

  res = NULL;
  while (n != 0) {
    *dest = *src;
    if (*src != 0)
      src++;
    else if (res == NULL)
      res = dest;
    dest++;
    n--;
  }
  if (res == NULL)
    res = dest;
  return res;
}
