/*-
 * Copyright (c) 2001 Mike Barcroft <mike@FreeBSD.org>
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
 * $FreeBSD: src/include/inttypes.h,v 1.8 2002/09/22 08:06:45 tjr Exp $
 * $DragonFly: src/include/inttypes.h,v 1.1 2003/11/09 02:22:28 dillon Exp $
 */

#ifndef _INTTYPES_H_
#define	_INTTYPES_H_

#include <machine/inttypes.h>
#include <stdint.h>

typedef struct {
	intmax_t	quot;		/* Quotient. */
	intmax_t	rem;		/* Remainder. */
} imaxdiv_t;

__BEGIN_DECLS
intmax_t	imaxabs(intmax_t) __pure2;
imaxdiv_t	imaxdiv(intmax_t, intmax_t) __pure2;

intmax_t	strtoimax(const char * __restrict, char ** __restrict, int);
uintmax_t	strtoumax(const char * __restrict, char ** __restrict, int);
#ifndef __cplusplus
intmax_t	wcstoimax(const __wchar_t * __restrict,
		    __wchar_t ** __restrict, int);
uintmax_t	wcstoumax(const __wchar_t * __restrict,
		    __wchar_t ** __restrict, int);
#endif
__END_DECLS

#endif /* !_INTTYPES_H_ */