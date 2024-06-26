// -*- C++ -*-
/* Copyright (C) 1989, 1990, 1991, 1992 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com) */

#include <ctype.h>
#include "cset.h"

cset csalpha(CSET_BUILTIN);
cset csupper(CSET_BUILTIN);
cset cslower(CSET_BUILTIN);
cset csdigit(CSET_BUILTIN);
cset csxdigit(CSET_BUILTIN);
cset csspace(CSET_BUILTIN);
cset cspunct(CSET_BUILTIN);
cset csalnum(CSET_BUILTIN);
cset csprint(CSET_BUILTIN);
cset csgraph(CSET_BUILTIN);
cset cscntrl(CSET_BUILTIN);

#ifdef isascii
#define ISASCII(c) isascii(c)
#else
#define ISASCII(c) (1)
#endif

void
cset::clear()
{
  char *p = v;
  for (int i = 0; i <= UCHAR_MAX; i++)
    p[i] = 0;
}

cset::cset()
{
  clear();
}

cset::cset(const char *s)
{
  clear();
  while (*s)
    v[(unsigned char)*s++] = 1;
}

cset::cset(const unsigned char *s)
{
  clear();
  while (*s)
    v[*s++] = 1;
}

cset::cset(cset_builtin)
{
  // these are initialised by cset_init::cset_init()
}

cset &
cset::operator|=(const cset &cs)
{
  for (int i = 0; i <= UCHAR_MAX; i++)
    if (cs.v[i])
      v[i] = 1;
  return *this;
}


int
cset_init::initialised = 0;

cset_init::cset_init()
{
  if (initialised)
    return;
  initialised = 1;
  for (int i = 0; i <= UCHAR_MAX; i++) {
    csalpha.v[i] = ISASCII(i) && isalpha(i);
    csupper.v[i] = ISASCII(i) && isupper(i);
    cslower.v[i] = ISASCII(i) && islower(i);
    csdigit.v[i] = ISASCII(i) && isdigit(i);
    csxdigit.v[i] = ISASCII(i) && isxdigit(i);
    csspace.v[i] = ISASCII(i) && isspace(i);
    cspunct.v[i] = ISASCII(i) && ispunct(i);
    csalnum.v[i] = ISASCII(i) && isalnum(i);
    csprint.v[i] = ISASCII(i) && isprint(i);
    csgraph.v[i] = ISASCII(i) && isgraph(i);
    cscntrl.v[i] = ISASCII(i) && iscntrl(i);
  }
}
