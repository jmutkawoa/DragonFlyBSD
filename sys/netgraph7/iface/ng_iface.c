/*
 * ng_iface.c
 */

/*-
 * Copyright (c) 1996-1999 Whistle Communications, Inc.
 * All rights reserved.
 * 
 * Subject to the following obligations and disclaimer of warranty, use and
 * redistribution of this software, in source or object code forms, with or
 * without modifications are expressly permitted by Whistle Communications;
 * provided, however, that:
 * 1. Any and all reproductions of the source or object code must include the
 *    copyright notice above and the following disclaimer of warranties; and
 * 2. No rights are granted, in any manner or form, to use Whistle
 *    Communications, Inc. trademarks, including the mark "WHISTLE
 *    COMMUNICATIONS" on advertising, endorsements, or otherwise except as
 *    such appears in the above copyright notice or in the software.
 * 
 * THIS SOFTWARE IS BEING PROVIDED BY WHISTLE COMMUNICATIONS "AS IS", AND
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, WHISTLE COMMUNICATIONS MAKES NO
 * REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, REGARDING THIS SOFTWARE,
 * INCLUDING WITHOUT LIMITATION, ANY AND ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * WHISTLE COMMUNICATIONS DOES NOT WARRANT, GUARANTEE, OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF, OR THE RESULTS OF THE USE OF THIS
 * SOFTWARE IN TERMS OF ITS CORRECTNESS, ACCURACY, RELIABILITY OR OTHERWISE.
 * IN NO EVENT SHALL WHISTLE COMMUNICATIONS BE LIABLE FOR ANY DAMAGES
 * RESULTING FROM OR ARISING OUT OF ANY USE OF THIS SOFTWARE, INCLUDING
 * WITHOUT LIMITATION, ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * PUNITIVE, OR CONSEQUENTIAL DAMAGES, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES, LOSS OF USE, DATA OR PROFITS, HOWEVER CAUSED AND UNDER ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF WHISTLE COMMUNICATIONS IS ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Archie Cobbs <archie@freebsd.org>
 *
 * $FreeBSD: src/sys/netgraph/ng_iface.c,v 1.48 2008/01/31 08:51:48 mav Exp $
 * $Whistle: ng_iface.c,v 1.33 1999/11/01 09:24:51 julian Exp $
 */

/*
 * This node is also a system networking interface. It has
 * a hook for each protocol (IP, AppleTalk, etc). Packets
 * are simply relayed between the interface and the hooks.
 *
 * Interfaces are named ng0, ng1, etc.  New nodes take the
 * first available interface name.
 *
 * This node also includes Berkeley packet filter support.
 */

#include "opt_inet.h"
#include "opt_inet6.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/random.h>
#include <sys/sockio.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/libkern.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/ifq_var.h>
#include <net/bpf.h>
#include <net/netisr.h>

#include <netinet/in.h>

#include <netgraph7/ng_message.h>
#include <netgraph7/netgraph.h>
#include <netgraph7/ng_parse.h>
#include <netgraph7/cisco/ng_cisco.h>
#include "ng_iface.h"

#ifdef NG_SEPARATE_MALLOC
MALLOC_DEFINE(M_NETGRAPH_IFACE, "netgraph_iface", "netgraph iface node ");
#else
#define M_NETGRAPH_IFACE M_NETGRAPH
#endif

/* This struct describes one address family */
struct iffam {
	sa_family_t	family;		/* Address family */
	const char	*hookname;	/* Name for hook */
};
typedef const struct iffam *iffam_p;

/* List of address families supported by our interface */
static const struct iffam gFamilies[] = {
	{ AF_INET,	NG_IFACE_HOOK_INET	},
	{ AF_INET6,	NG_IFACE_HOOK_INET6	},
	{ AF_ATM,	NG_IFACE_HOOK_ATM	},
	{ AF_NATM,	NG_IFACE_HOOK_NATM	},
};
#define NUM_FAMILIES		NELEM(gFamilies)

/* Node private data */
struct ng_iface_private {
	struct	ifnet *ifp;		/* Our interface */
	int	unit;			/* Interface unit number */
	node_p	node;			/* Our netgraph node */
	hook_p	hooks[NUM_FAMILIES];	/* Hook for each address family */
};
typedef struct ng_iface_private *priv_p;

/* Interface methods */
static void	ng_iface_start(struct ifnet *ifp, struct ifaltq_subque *);
static int	ng_iface_ioctl(struct ifnet *ifp, u_long cmd, caddr_t data,
			struct ucred *cr);
static int	ng_iface_output(struct ifnet *ifp, struct mbuf *m0,
			struct sockaddr *dst, struct rtentry *rt0);
static void	ng_iface_bpftap(struct ifnet *ifp,
			struct mbuf *m, sa_family_t family);
static int	ng_iface_send(struct ifnet *ifp, struct mbuf *m,
			sa_family_t sa);
#ifdef DEBUG
static void	ng_iface_print_ioctl(struct ifnet *ifp, int cmd, caddr_t data);
#endif

/* Netgraph methods */
static int		ng_iface_mod_event(module_t, int, void *);
static ng_constructor_t	ng_iface_constructor;
static ng_rcvmsg_t	ng_iface_rcvmsg;
static ng_shutdown_t	ng_iface_shutdown;
static ng_newhook_t	ng_iface_newhook;
static ng_rcvdata_t	ng_iface_rcvdata;
static ng_disconnect_t	ng_iface_disconnect;

/* Helper stuff */
static iffam_p	get_iffam_from_af(sa_family_t family);
static iffam_p	get_iffam_from_hook(priv_p priv, hook_p hook);
static iffam_p	get_iffam_from_name(const char *name);
static hook_p  *get_hook_from_iffam(priv_p priv, iffam_p iffam);

/* Parse type for struct ng_cisco_ipaddr */
static const struct ng_parse_struct_field ng_cisco_ipaddr_type_fields[]
	= NG_CISCO_IPADDR_TYPE_INFO;
static const struct ng_parse_type ng_cisco_ipaddr_type = {
	&ng_parse_struct_type,
	&ng_cisco_ipaddr_type_fields
};

/* List of commands and how to convert arguments to/from ASCII */
static const struct ng_cmdlist ng_iface_cmds[] = {
	{
	  NGM_IFACE_COOKIE,
	  NGM_IFACE_GET_IFNAME,
	  "getifname",
	  NULL,
	  &ng_parse_string_type
	},
	{
	  NGM_IFACE_COOKIE,
	  NGM_IFACE_POINT2POINT,
	  "point2point",
	  NULL,
	  NULL
	},
	{
	  NGM_IFACE_COOKIE,
	  NGM_IFACE_BROADCAST,
	  "broadcast",
	  NULL,
	  NULL
	},
	{
	  NGM_CISCO_COOKIE,
	  NGM_CISCO_GET_IPADDR,
	  "getipaddr",
	  NULL,
	  &ng_cisco_ipaddr_type
	},
	{
	  NGM_IFACE_COOKIE,
	  NGM_IFACE_GET_IFINDEX,
	  "getifindex",
	  NULL,
	  &ng_parse_uint32_type
	},
	{ 0 }
};

/* Node type descriptor */
static struct ng_type typestruct = {
	.version =	NG_ABI_VERSION,
	.name =		NG_IFACE_NODE_TYPE,
	.mod_event =	ng_iface_mod_event,
	.constructor =	ng_iface_constructor,
	.rcvmsg =	ng_iface_rcvmsg,
	.shutdown =	ng_iface_shutdown,
	.newhook =	ng_iface_newhook,
	.rcvdata =	ng_iface_rcvdata,
	.disconnect =	ng_iface_disconnect,
	.cmdlist =	ng_iface_cmds,
};
NETGRAPH_INIT(iface, &typestruct);

/* We keep a bitmap indicating which unit numbers are free.
   One means the unit number is free, zero means it's taken. */
static int	*ng_iface_units = NULL;
static int	ng_iface_units_len = 0;

#define UNITS_BITSPERWORD	(sizeof(*ng_iface_units) * NBBY)


/************************************************************************
			HELPER STUFF
 ************************************************************************/

/*
 * Get the family descriptor from the family ID
 */
static __inline iffam_p
get_iffam_from_af(sa_family_t family)
{
	iffam_p iffam;
	int k;

	for (k = 0; k < NUM_FAMILIES; k++) {
		iffam = &gFamilies[k];
		if (iffam->family == family)
			return (iffam);
	}
	return (NULL);
}

/*
 * Get the family descriptor from the hook
 */
static __inline iffam_p
get_iffam_from_hook(priv_p priv, hook_p hook)
{
	int k;

	for (k = 0; k < NUM_FAMILIES; k++)
		if (priv->hooks[k] == hook)
			return (&gFamilies[k]);
	return (NULL);
}

/*
 * Get the hook from the iffam descriptor
 */

static __inline hook_p *
get_hook_from_iffam(priv_p priv, iffam_p iffam)
{
	return (&priv->hooks[iffam - gFamilies]);
}

/*
 * Get the iffam descriptor from the name
 */
static __inline iffam_p
get_iffam_from_name(const char *name)
{
	iffam_p iffam;
	int k;

	for (k = 0; k < NUM_FAMILIES; k++) {
		iffam = &gFamilies[k];
		if (!strcmp(iffam->hookname, name))
			return (iffam);
	}
	return (NULL);
}

/*
 * Find the first free unit number for a new interface.
 * Increase the size of the unit bitmap as necessary.
 */
static __inline__ int
ng_iface_get_unit(int *unit)
{
	int index, bit;

	for (index = 0; index < ng_iface_units_len
	    && ng_iface_units[index] == 0; index++);
	if (index == ng_iface_units_len) {		/* extend array */
		int i, *newarray, newlen;

		newlen = (2 * ng_iface_units_len) + 4;
		newarray = kmalloc(newlen * sizeof(*ng_iface_units),
		    M_NETGRAPH_IFACE, M_NOWAIT);
		if (newarray == NULL)
			return (ENOMEM);
		bcopy(ng_iface_units, newarray,
		    ng_iface_units_len * sizeof(*ng_iface_units));
		for (i = ng_iface_units_len; i < newlen; i++)
			newarray[i] = ~0;
		if (ng_iface_units != NULL)
			kfree(ng_iface_units, M_NETGRAPH_IFACE);
		ng_iface_units = newarray;
		ng_iface_units_len = newlen;
	}
	bit = ffs(ng_iface_units[index]) - 1;
	KASSERT(bit >= 0 && bit <= UNITS_BITSPERWORD - 1,
	    ("%s: word=%d bit=%d", __func__, ng_iface_units[index], bit));
	ng_iface_units[index] &= ~(1 << bit);
	*unit = (index * UNITS_BITSPERWORD) + bit;
	return (0);
}

/*
 * Free a no longer needed unit number.
 */
static __inline__ void
ng_iface_free_unit(int unit)
{
	int index, bit;

	index = unit / UNITS_BITSPERWORD;
	bit = unit % UNITS_BITSPERWORD;
	KASSERT(index < ng_iface_units_len,
	    ("%s: unit=%d len=%d", __func__, unit, ng_iface_units_len));
	KASSERT((ng_iface_units[index] & (1 << bit)) == 0,
	    ("%s: unit=%d is free", __func__, unit));
	ng_iface_units[index] |= (1 << bit);
	/*
	 * XXX We could think about reducing the size of ng_iface_units[]
	 * XXX here if the last portion is all ones
	 */
}

/************************************************************************
			INTERFACE STUFF
 ************************************************************************/

/*
 * Process an ioctl for the virtual interface
 */
static int
ng_iface_ioctl(struct ifnet *ifp, u_long command, caddr_t data,
    struct ucred *cr)
{
	struct ifreq *const ifr = (struct ifreq *) data;
	int error = 0;

#ifdef DEBUG
	ng_iface_print_ioctl(ifp, command, data);
#endif
	crit_enter();
	switch (command) {

	/* These two are mostly handled at a higher layer */
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		ifp->if_flags |= IFF_RUNNING;
		ifq_clr_oactive(&ifp->if_snd);
		break;
	case SIOCGIFADDR:
		break;

	/* Set flags */
	case SIOCSIFFLAGS:
		/*
		 * If the interface is marked up and stopped, then start it.
		 * If it is marked down and running, then stop it.
		 */
		if (ifr->ifr_flags & IFF_UP) {
			if (!(ifp->if_flags & IFF_RUNNING)) {
				ifq_clr_oactive(&ifp->if_snd);
				ifp->if_flags |= IFF_RUNNING;
			}
		} else {
			if (ifp->if_flags & IFF_RUNNING) {
				ifp->if_flags &= ~IFF_RUNNING;
				ifq_clr_oactive(&ifp->if_snd);
			}
		}
		break;

	/* Set the interface MTU */
	case SIOCSIFMTU:
		if (ifr->ifr_mtu > NG_IFACE_MTU_MAX
		    || ifr->ifr_mtu < NG_IFACE_MTU_MIN)
			error = EINVAL;
		else
			ifp->if_mtu = ifr->ifr_mtu;
		break;

	/* Stuff that's not supported */
	case SIOCADDMULTI:
	case SIOCDELMULTI:
		error = 0;
		break;
	case SIOCSIFPHYS:
		error = EOPNOTSUPP;
		break;

	default:
		error = EINVAL;
		break;
	}
	crit_exit();
	return (error);
}

/*
 * This routine is called to deliver a packet out the interface.
 * We simply look at the address family and relay the packet to
 * the corresponding hook, if it exists and is connected.
 */

static int
ng_iface_output(struct ifnet *ifp, struct mbuf *m,
		struct sockaddr *dst, struct rtentry *rt0)
{
	uint32_t af;
	int error;

	/* Check interface flags */
	if (!((ifp->if_flags & IFF_UP) && (ifp->if_flags & IFF_RUNNING))) {
		m_freem(m);
		return (ENETDOWN);
	}

	/* BPF writes need to be handled specially. */
	if (dst->sa_family == AF_UNSPEC) {
		bcopy(dst->sa_data, &af, sizeof(af));
		dst->sa_family = af;
	}

	/* Berkeley packet filter */
	ng_iface_bpftap(ifp, m, dst->sa_family);

	if (ifq_is_enabled(&ifp->if_snd)) {
		M_PREPEND(m, sizeof(sa_family_t), MB_DONTWAIT);
		if (m == NULL) {
			IFNET_STAT_INC(ifp, oerrors, 1);
			return (ENOBUFS);
		}
		*(sa_family_t *)m->m_data = dst->sa_family;
		error = ifq_dispatch(ifp, m, NULL);
	} else
		error = ng_iface_send(ifp, m, dst->sa_family);

	return (error);
}

/*
 * Start method is used only when ALTQ is enabled.
 */
static void
ng_iface_start(struct ifnet *ifp, struct ifaltq_subque *ifsq)
{
	sa_family_t sa;

	KASSERT(ifq_is_enabled(&ifp->if_snd), ("%s without ALTQ", __func__));

	for(;;) {
		struct mbuf *m;

		m = ifq_dequeue(&ifp->if_snd);
		if (m == NULL)
			break;
		sa = *mtod(m, sa_family_t *);
		m_adj(m, sizeof(sa_family_t));
		ng_iface_send(ifp, m, sa);
	}
}

/*
 * Flash a packet by the BPF (requires prepending 4 byte AF header)
 * Note the phoney mbuf; this is OK because BPF treats it read-only.
 */
static void
ng_iface_bpftap(struct ifnet *ifp, struct mbuf *m, sa_family_t family)
{
	int32_t family4 = (int32_t)family;

	KASSERT(family != AF_UNSPEC, ("%s: family=AF_UNSPEC", __func__));

	if (ifp->if_bpf) {
		bpf_gettoken();
		if (ifp->if_bpf)
			bpf_ptap(ifp->if_bpf, m, &family4, sizeof(family4));
		bpf_reltoken();
	}
}

/*
 * This routine does actual delivery of the packet into the
 * netgraph(4). It is called from ng_iface_start() and
 * ng_iface_output().
 */
static int
ng_iface_send(struct ifnet *ifp, struct mbuf *m, sa_family_t sa)
{
	const priv_p priv = (priv_p) ifp->if_softc;
	const iffam_p iffam = get_iffam_from_af(sa);
	int error;
	int len;

	/* Check address family to determine hook (if known) */
	if (iffam == NULL) {
		m_freem(m);
		log(LOG_WARNING, "%s: can't handle af%d\n", ifp->if_xname, sa);
		return (EAFNOSUPPORT);
	}

	/* Copy length before the mbuf gets invalidated. */
	len = m->m_pkthdr.len;

	/* Send packet. If hook is not connected,
	   mbuf will get freed. */
	NG_SEND_DATA_ONLY(error, *get_hook_from_iffam(priv, iffam), m);

	/* Update stats. */
	if (error == 0) {
		IFNET_STAT_INC(ifp, obytes, len);
		IFNET_STAT_INC(ifp, opackets, 1);
	}

	return (error);
}

#ifdef DEBUG
/*
 * Display an ioctl to the virtual interface
 */

static void
ng_iface_print_ioctl(struct ifnet *ifp, int command, caddr_t data)
{
	char   *str;

	switch (command & IOC_DIRMASK) {
	case IOC_VOID:
		str = "IO";
		break;
	case IOC_OUT:
		str = "IOR";
		break;
	case IOC_IN:
		str = "IOW";
		break;
	case IOC_INOUT:
		str = "IORW";
		break;
	default:
		str = "IO??";
	}
	log(LOG_DEBUG, "%s: %s('%c', %d, char[%d])\n",
	       ifp->if_xname,
	       str,
	       IOCGROUP(command),
	       command & 0xff,
	       IOCPARM_LEN(command));
}
#endif /* DEBUG */

/************************************************************************
			NETGRAPH NODE STUFF
 ************************************************************************/

/*
 * Constructor for a node
 */
static int
ng_iface_constructor(node_p node)
{
	struct ifnet *ifp;
	priv_p priv;
	int error;

	/* Allocate node and interface private structures */
	priv = kmalloc(sizeof(*priv), M_NETGRAPH_IFACE,
		       M_WAITOK | M_NULLOK | M_ZERO);
	if (priv == NULL)
		return (ENOMEM);
	ifp = kmalloc(sizeof(*ifp), M_NETGRAPH_IFACE, M_WAITOK | M_NULLOK | M_ZERO);
	if (ifp == NULL) {
		kfree(priv, M_NETGRAPH_IFACE);
		return (ENOMEM);
	}

	/* Link them together */
	ifp->if_softc = priv;
	priv->ifp = ifp;

	/* Get an interface unit number */
	if ((error = ng_iface_get_unit(&priv->unit)) != 0) {
		kfree(ifp, M_NETGRAPH_IFACE);
		kfree(priv, M_NETGRAPH_IFACE);
		return (error);
	}


	/* Link together node and private info */
	NG_NODE_SET_PRIVATE(node, priv);
	priv->node = node;

	/* Initialize interface structure */
	if_initname(ifp, NG_IFACE_IFACE_NAME, priv->unit);
	ifp->if_output = ng_iface_output;
	ifp->if_start = ng_iface_start;
	ifp->if_ioctl = ng_iface_ioctl;
	ifp->if_watchdog = NULL;
	ifp->if_mtu = NG_IFACE_MTU_DEFAULT;
	ifp->if_flags = (IFF_SIMPLEX|IFF_POINTOPOINT|IFF_NOARP|IFF_MULTICAST);
	ifp->if_type = IFT_PROPVIRTUAL;		/* XXX */
	ifp->if_addrlen = 0;			/* XXX */
	ifp->if_hdrlen = 0;			/* XXX */
	ifp->if_baudrate = 64000;		/* XXX */
	ifq_set_maxlen(&ifp->if_snd, IFQ_MAXLEN);
	ifq_set_ready(&ifp->if_snd);

	/* Give this node the same name as the interface (if possible) */
	if (ng_name_node(node, ifp->if_xname) != 0)
		log(LOG_WARNING, "%s: can't acquire netgraph name\n",
		    ifp->if_xname);

	/* Attach the interface */
	if_attach(ifp, NULL);
	bpfattach(ifp, DLT_NULL, sizeof(u_int32_t));

	/* Done */
	return (0);
}

/*
 * Give our ok for a hook to be added
 */
static int
ng_iface_newhook(node_p node, hook_p hook, const char *name)
{
	const iffam_p iffam = get_iffam_from_name(name);
	hook_p *hookptr;

	if (iffam == NULL)
		return (EPFNOSUPPORT);
	hookptr = get_hook_from_iffam(NG_NODE_PRIVATE(node), iffam);
	if (*hookptr != NULL)
		return (EISCONN);
	*hookptr = hook;
	NG_HOOK_HI_STACK(hook);
	return (0);
}

/*
 * Receive a control message
 */
static int
ng_iface_rcvmsg(node_p node, item_p item, hook_p lasthook)
{
	const priv_p priv = NG_NODE_PRIVATE(node);
	struct ifnet *const ifp = priv->ifp;
	struct ng_mesg *resp = NULL;
	int error = 0;
	struct ng_mesg *msg;

	NGI_GET_MSG(item, msg);
	switch (msg->header.typecookie) {
	case NGM_IFACE_COOKIE:
		switch (msg->header.cmd) {
		case NGM_IFACE_GET_IFNAME:
			NG_MKRESPONSE(resp, msg, IFNAMSIZ, M_WAITOK | M_NULLOK);
			if (resp == NULL) {
				error = ENOMEM;
				break;
			}
			strlcpy(resp->data, ifp->if_xname, IFNAMSIZ);
			break;

		case NGM_IFACE_POINT2POINT:
		case NGM_IFACE_BROADCAST:
		    {

			/* Deny request if interface is UP */
			if ((ifp->if_flags & IFF_UP) != 0)
				return (EBUSY);

			/* Change flags */
			switch (msg->header.cmd) {
			case NGM_IFACE_POINT2POINT:
				ifp->if_flags |= IFF_POINTOPOINT;
				ifp->if_flags &= ~IFF_BROADCAST;
				break;
			case NGM_IFACE_BROADCAST:
				ifp->if_flags &= ~IFF_POINTOPOINT;
				ifp->if_flags |= IFF_BROADCAST;
				break;
			}
			break;
		    }

		case NGM_IFACE_GET_IFINDEX:
			NG_MKRESPONSE(resp, msg, sizeof(uint32_t), M_WAITOK | M_NULLOK);
			if (resp == NULL) {
				error = ENOMEM;
				break;
			}
			*((uint32_t *)resp->data) = priv->ifp->if_index;
			break;

		default:
			error = EINVAL;
			break;
		}
		break;
	case NGM_CISCO_COOKIE:
		switch (msg->header.cmd) {
		case NGM_CISCO_GET_IPADDR:	/* we understand this too */
		    {
			struct ifaddr_container *ifac;

			/* Return the first configured IP address */
			TAILQ_FOREACH(ifac, &ifp->if_addrheads[mycpuid], ifa_link) {
				struct ifaddr *ifa = ifac->ifa;
				struct ng_cisco_ipaddr *ips;

				if (ifa->ifa_addr->sa_family != AF_INET)
					continue;
				NG_MKRESPONSE(resp, msg, sizeof(ips), M_WAITOK | M_NULLOK);
				if (resp == NULL) {
					error = ENOMEM;
					break;
				}
				ips = (struct ng_cisco_ipaddr *)resp->data;
				ips->ipaddr = ((struct sockaddr_in *)
						ifa->ifa_addr)->sin_addr;
				ips->netmask = ((struct sockaddr_in *)
						ifa->ifa_netmask)->sin_addr;
				break;
			}

			/* No IP addresses on this interface? */
			if (ifac == NULL)
				error = EADDRNOTAVAIL;
			break;
		    }
		default:
			error = EINVAL;
			break;
		}
		break;
	case NGM_FLOW_COOKIE:
		switch (msg->header.cmd) {
		case NGM_LINK_IS_UP:
			ifp->if_flags |= IFF_RUNNING;
			break;
		case NGM_LINK_IS_DOWN:
			ifp->if_flags &= ~IFF_RUNNING;
			break;
		default:
			break;
		}
		break;
	default:
		error = EINVAL;
		break;
	}
	NG_RESPOND_MSG(error, node, item, resp);
	NG_FREE_MSG(msg);
	return (error);
}

/*
 * Recive data from a hook. Pass the packet to the correct input routine.
 */
static int
ng_iface_rcvdata(hook_p hook, item_p item)
{
	const priv_p priv = NG_NODE_PRIVATE(NG_HOOK_NODE(hook));
	const iffam_p iffam = get_iffam_from_hook(priv, hook);
	struct ifnet *const ifp = priv->ifp;
	struct mbuf *m;
	int isr;

	NGI_GET_M(item, m);
	NG_FREE_ITEM(item);
	/* Sanity checks */
	KASSERT(iffam != NULL, ("%s: iffam", __func__));
	M_ASSERTPKTHDR(m);
	if ((ifp->if_flags & IFF_UP) == 0) {
		NG_FREE_M(m);
		return (ENETDOWN);
	}

	/* Update interface stats */
	IFNET_STAT_INC(ifp, ipackets, 1);
	IFNET_STAT_INC(ifp, ibytes, m->m_pkthdr.len);

	/* Note receiving interface */
	m->m_pkthdr.rcvif = ifp;

	/* Berkeley packet filter */
	ng_iface_bpftap(ifp, m, iffam->family);

	/* Send packet */
	switch (iffam->family) {
#ifdef INET
	case AF_INET:
		isr = NETISR_IP;
		break;
#endif
#ifdef INET6
	case AF_INET6:
		isr = NETISR_IPV6;
		break;
#endif
	default:
		m_freem(m);
		return (EAFNOSUPPORT);
	}
#if 0
	/* First chunk of an mbuf contains good junk */
	if (harvest.point_to_point)
		random_harvest(m, 16, 3, 0, RANDOM_NET);
#endif
	netisr_queue(isr, m);
	return (0);
}

/*
 * Shutdown and remove the node and its associated interface.
 */
static int
ng_iface_shutdown(node_p node)
{
	const priv_p priv = NG_NODE_PRIVATE(node);

	bpfdetach(priv->ifp);
	if_detach(priv->ifp);
	kfree(priv->ifp, M_NETGRAPH_IFACE);
	priv->ifp = NULL;
	ng_iface_free_unit(priv->unit);
	kfree(priv, M_NETGRAPH_IFACE);
	NG_NODE_SET_PRIVATE(node, NULL);
	NG_NODE_UNREF(node);
	return (0);
}

/*
 * Hook disconnection. Note that we do *not* shutdown when all
 * hooks have been disconnected.
 */
static int
ng_iface_disconnect(hook_p hook)
{
	const priv_p priv = NG_NODE_PRIVATE(NG_HOOK_NODE(hook));
	const iffam_p iffam = get_iffam_from_hook(priv, hook);

	if (iffam == NULL)
		panic(__func__);
	*get_hook_from_iffam(priv, iffam) = NULL;
	return (0);
}

/*
 * Handle loading and unloading for this node type.
 */
static int
ng_iface_mod_event(module_t mod, int event, void *data)
{
	int error = 0;

	switch (event) {
	case MOD_LOAD:
		break;
	case MOD_UNLOAD:
		break;
	default:
		error = EOPNOTSUPP;
		break;
	}
	return (error);
}