/*
 * antiarp.c
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#define DEBUG

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <net/arp.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/etherdevice.h>
#include <linux/sockios.h>
#include <linux/net.h>
#include <net/net_namespace.h>
#include <uapi/linux/if_arp.h>
#include <uapi/linux/if_ether.h>
#include <net/icmp.h>

#define MODULENAME "antiarp"

MODULE_LICENSE("GPL");

static int __init antiarp_init(void)
{
	struct net *ns;
	for_each_net(ns) {
		struct net_device *dev;
		struct netdev_hw_addr_list *l;
		struct netdev_hw_addr *dha;
		struct in_device *ind;
		struct in_ifaddr *list;
		__be32 sip = (__be32) 16820416; /* The gateway IP: 192.168.0.1, you must change it to your gateway ip or the ip you want to ack. */
		unsigned char sha[] = {'\0', '\21', '\42', '\63', '\104', '\125', '\0'}; /* The hardware address of gateway:00:11:22:33:44:55, you must change it to your gateway hardware address of the address you want to ack. */
		int j = 0;

		/* List hardware addresses of all devices. */
		for_each_netdev(ns, dev) {
			l = &dev->dev_addrs;

			netdev_hw_addr_list_for_each(dha, l) {
				int i = 0;
				pr_debug("Hardware address is: ");
				for (; i < MAX_ADDR_LEN; i++)
					pr_debug("%hhx ", dha->addr[i]);
			}
			pr_debug("The conunt is %d.\n", netdev_hw_addr_list_count(l));
		}

		dev = __dev_get_by_name(ns, "eth0"); /* This function can get ethernet device using the device name. You also can use the code in the last comment block. */
		if (!dev)
			continue;

		l = &dev->dev_addrs;
		ind = dev->ip_ptr;
		list = ind->ifa_list;
		dha = list_first_entry(&(l)->list, typeof(*dha), list);
		while (j <= 10000) {
			arp_send(ARPOP_REPLY, ETH_P_ARP, sip, dev, list->ifa_address, sha, dha->addr, sha);
			j++;
		}
	}
	return 0;
}

static void __exit antiarp_exit(void)
{
}

module_init(antiarp_init);
module_exit(antiarp_exit);
