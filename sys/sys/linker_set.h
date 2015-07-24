/*-
 * Copyright (c) 1999 John D. Polstra
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/sys/linker_set.h,v 1.4.2.1 2000/08/02 21:52:20 peter Exp $
 * $DragonFly: src/sys/sys/linker_set.h,v 1.4 2004/08/27 12:08:41 joerg Exp $
 */

#ifndef _SYS_LINKER_SET_H_
#define _SYS_LINKER_SET_H_

#ifndef _SYS_CDEFS_H_
#include <sys/cdefs.h>
#endif

/*
 * The following macros are used to declare global sets of objects, which
 * are collected by the linker into a `struct linker_set' as defined below.
 * For ELF, this is done by constructing a separate segment for each set.
 *
 * In the __MAKE_SET macros below, the lines:
 *
 *   static void const * const __set_##set##_sym_##sym = &sym;
 *
 * are present only to prevent the compiler from producing bogus
 * warnings about unused symbols.
 */
#ifndef __ELF__
#error only ELF is supported
#endif

#if 0

#define __MAKE_SET(set, sym)						\
	static void const * const __set_##set##_sym_##sym = &sym;	\
	__asm(".section set_" #set ",\"aw\"");				\
	__asm(".long " #sym);						\
	__asm(".previous")

#endif

#define __MAKE_SET(set, sym)						\
	__GLOBL(__CONCAT(__start_set_,set));				\
	__GLOBL(__CONCAT(__stop_set_,set));				\
	static void const * const __set_##set##_sym_##sym		\
	__section("set_" #set) __used = &sym

#define TEXT_SET(set, sym) __MAKE_SET(set, sym)
#define DATA_SET(set, sym) __MAKE_SET(set, sym)
#define BSS_SET(set, sym) __MAKE_SET(set, sym)
#define ABS_SET(set, sym) __MAKE_SET(set, sym)
#define SET_ENTRY(set, sym) __MAKE_SET(set, sym)

#define SET_DECLARE(set, ptype)				\
	extern ptype *__CONCAT(__start_set_,set);	\
	extern ptype *__CONCAT(__stop_set_,set)

#define SET_BEGIN(set)		(&__CONCAT(__start_set_,set))
#define SET_LIMIT(set)		(&__CONCAT(__stop_set_,set))

/*
 * Iterate over all the elements of a set.
 *
 * Sets always contain addresses of things, and "pvar" points to words
 * containing those addresses.  Thus is must be declared as "type **pvar",
 * and the address of each set item is obtained inside the loop by "*pvar".
 */
#define SET_FOREACH(pvar, set)				\
	for (pvar = SET_BEGIN(set); pvar < SET_LIMIT(set); pvar++)

#define SET_ITEM(set, i)				\
	((SET_BEGIN(set))[i])

/*
 * Provide a count of the items in a set.
 */
#define SET_COUNT(set)					\
	(SET_LIMIT(set) - SET_BEGIN(set))

#endif /* _SYS_LINKER_SET_H_ */