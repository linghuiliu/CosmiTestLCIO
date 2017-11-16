#ifndef STRO_HH
#define STRO_HH 1

#define USE_LONGLONG


/* Copyright (C) 2000 MySQL AB
  
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/
 
/*
strtol,strtoul,strtoll,strtoull
convert string to long, unsigned long, long long or unsigned long long.
strtoxx(char *src,char **ptr,int base)
converts the string pointed to by src to an long of appropriate long and
returnes it. It skips leading spaces and tabs (but not newlines, formfeeds,
backspaces), then it accepts an optional sign and a sequence of digits
in the specified radix.
If the value of ptr is not (char **)NULL, a pointer to the character
terminating the scan is returned in the location pointed to by ptr.
Trailing spaces will NOT be skipped.
     
If an error is detected, the result will be LONG_MIN, 0 or LONG_MAX,
(or LONGLONG..)  and errno will be set to
EDOM	if there are no digits
ERANGE	if the result would overflow.
the ptr will be set to src.
This file is based on the strtol from the the GNU C Library.
it can be compiled with the UNSIGNED and/or LONGLONG flag set
*/


  //#if !defined(_global_h) || !defined(_m_string_h)
  //#  error  Calling file must include 'my_global.h' and 'm_string.h'
  ///* see 'strtoll.c' and 'strtoull.c' for the reasons */
  //#endif

#include <sys/types.h>
  //#include <sys.h>			/* defines errno */
#include <errno.h>

#undef strtoull
#undef strtoll
#undef strtoul
#undef strtol
#ifdef USE_LONGLONG
#define UTYPE_MAX (~(ulonglong) 0)
#define TYPE_MIN LONGLONG_MIN
#define TYPE_MAX LONGLONG_MAX
#define longtype int64_t
#define ulongtype u_int64_t
#ifdef USE_UNSIGNED
#define function ulongtype strtoull
#else
#define function longtype strtoll
#endif
#else
#define UTYPE_MAX (ulong) ~0L
#define TYPE_MIN LONG_MIN
#define TYPE_MAX LONG_MAX
#define longtype long
#define ulongtype unsigned long
#ifdef USE_UNSIGNED
#define function ulongtype strtoul
#else
#define function longtype strtol
#endif
#endif


  function(const char*, char**, int); 


#endif
