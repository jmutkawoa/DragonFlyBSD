# $FreeBSD: src/libexec/telnetd/Makefile,v 1.31 2006/07/31 19:15:09 yar Exp $

# Do not define -DKLUDGELINEMODE, as it does not interact well with many
# telnet implementations.

PROG=		telnetd
MAN=		telnetd.8

SRCS=		authenc.c global.c slc.c state.c sys_term.c telnetd.c \
		termstat.c utility.c

WARNS?=		2

CFLAGS+=	-DLINEMODE -DUSE_TERMIO -DDIAGNOSTICS -DOLD_ENVIRON -DENV_HACK \
		-I${.CURDIR}/../../lib

.if !defined(NO_INET6)
CFLAGS+=	-DINET6
.endif

LIBTELNET=	${.OBJDIR}/../../lib/libtelnet/libtelnet.a

DPADD=		${LIBUTIL} ${LIBNCURSES} ${LIBTELNET}
LDADD=		-lutil -lprivate_ncurses ${LIBTELNET}
LDFLAGS+=	${PRIVATELIB_LDFLAGS}
CFLAGS+=	-I${_SHLIBDIRPREFIX}/usr/include/priv/ncurses

CFLAGS+=	-DAUTHENTICATION -DENCRYPTION
CFLAGS+=	-I${.CURDIR}/../../crypto/libressl/include
DPADD+=		${LIBRECRYPTO} ${LIBCRYPT} ${LIBPAM}
LDADD+=		-lprivate_crypto -lcrypt ${MINUSLPAM}

.include <bsd.prog.mk>
