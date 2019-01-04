/*
** FILE: wtype.h
**
** PURPOSE: Basic header with internal types
**
** Copyright (c) 2019 Zachary Wells, see LICENSE for more details
*/

#ifndef WTYPE_H
#define WTYPE_H

#define REFCOUNTED int refcount

//Just an int, but helps code readability
typedef int bool_t;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

//Stupid micro optimizations
#if defined(__GNUC__)
#define unlikely(e) (__builtin_expect((e) != false, false))
#else
#define unlikely(e) (e)
#endif

#endif
