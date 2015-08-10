#include "process_pkt.h"

int process_larp_req(void *buff, struct sockaddr_ll *addr)
{
   struct arphdr *ar_hdr;
   ar_hdr = (struct arphdr *) buff;
   if (no_check_flag != 1) {
      if(check_larp_pkt(ar_hdr) != 0) { //Packet check failed. Malformed or incorrect packet
        printf("Not a LARP req/malformed packet. Ignoring..\n");  
	return 1;   
      }
   }
   //call larp reply here
   larp_reply_pkt(ar_hdr, addr); 
   #if 0
   printf("ARP hw type: %u\n", ntohs(ar_hdr->ar_htype));
   printf("Received from interface index: %d\n", addr->sll_ifindex);
   printf("SRC ip address: %u.%u.%u.%u\n", ar_hdr->ar_sip[0], ar_hdr->ar_sip[1], ar_hdr->ar_sip[2], ar_hdr->ar_sip[3]); 
   printf("ARP code: %u\n", ntohs(ar_hdr->ar_op));
   #endif
} 

int check_larp_pkt(struct arphdr *ar_hdr)
{
   if (ntohs(ar_hdr->ar_htype) != ARPHRD_LARP) { /*Not a LARP packet*/
	if (print_debugs)
	   printf("ARP htype is not LARP\n");
	return 1;
   }
   if ((ntohs(ar_hdr->ar_ptype) != ETH_P_IP) || (ntohs(ar_hdr->ar_ptype) != ETH_P_IPV6)) { /*Not an IP packet */
	if (print_debugs)
	   printf("ARP protocol type is not IPV4/IPV6\n");
	return 1;
   }
   if(ar_hdr->ar_hln != ETH_ALEN) { /*Incorrect hardware length*/
	if (print_debugs)
	   printf("Incorrect ARP hardware address length\n");
	return 1;
   }
   if((ar_hdr->ar_pln != IP_ADDR_SIZE) || (ar_hdr->ar_pln != IPV6_ADDR_SIZE)) { /*4 for Ipv4 and 16 for IPV6*/
	if (print_debugs)
	   printf("Incorrect protocol address length\n");
	return 1;
   }
   if(ntohs(ar_hdr->ar_op) != 1) { /*1 for LARP req, 2 for reply and 10 for ARP_NAK*/
	if (print_debugs)
	   printf("ARP OP: Not a LARP request packet\n");
	return 1;
   }
   return 0;
}
