#include "../hdrs/process_larp_req.h"

int process_larp_req(void *buff, struct sockaddr_ll *addr, int ifindex, ssize_t len)
{
   struct arphdr *ar_hdr;
   int pkt_len = (int) len;
   char if_name[IFNAMSIZ]; /* to store the interface name */
   ar_hdr = (struct arphdr *) buff;
   if (no_check_flag != 1) {
      if(check_larp_pkt(ar_hdr) != 0) { //Packet check failed. Malformed or incorrect packet
        if (print_msgs)
	    printf("Not a LARP req/malformed packet. Ignoring..\n");  
	return 1;   
      }
   }
   get_interface_name (ifindex, if_name);
   printf("Received LARP request from %u.%u.%u.%u for target %u.%u.%u.%u on interface: %s\n", ar_hdr->ar_sip[0], ar_hdr->ar_sip[1],ar_hdr->ar_sip[2],ar_hdr->ar_sip[3], ar_hdr->ar_tip[0],ar_hdr->ar_tip[1],ar_hdr->ar_tip[2],ar_hdr->ar_tip[3], if_name);
   if (hex_dump_flag)
      hexDump (ntohs(ar_hdr->ar_op), buff, pkt_len); 
   //call larp reply here
   larp_reply_pkt(ar_hdr, addr); 
} 

/* check for the correctness of fields in LARP request packet */
int check_larp_pkt(struct arphdr *ar_hdr)
{
   uint32_t src_ip32, dst_ip32;
   if (ntohs(ar_hdr->ar_htype) != ARPHRD_LARP) { /*Not a LARP packet*/
	if (print_msgs)
	   printf("ARP htype is not LARP\n");
	return 1;
   }
   if ((ntohs(ar_hdr->ar_ptype) != ETH_P_IP) && (ntohs(ar_hdr->ar_ptype) != ETH_P_IPV6)) { /*Not an IP packet */
	if (print_msgs)
	   printf("ARP protocol type is not IPV4/IPV6\n");
	return 1;
   }
   if(ar_hdr->ar_hln != ETH_ALEN) { /*Incorrect hardware length*/
	if (print_msgs)
	   printf("Incorrect ARP hardware address length\n");
	return 1;
   }
   if((ar_hdr->ar_pln != IP_ADDR_SIZE) && (ar_hdr->ar_pln != IPV6_ADDR_SIZE)) { /*4 for Ipv4 and 16 for IPV6*/
	if (print_msgs)
	   printf("Incorrect protocol address length\n");
	return 1;
   }
   if(ntohs(ar_hdr->ar_op) != 1) { /*1 for LARP req, 2 for reply and 10 for ARP_NAK*/
	if (print_msgs)
	   printf("ARP OP: Not a LARP request packet\n");
	return 1;
   }
   u32fromu8(ar_hdr->ar_sip, &src_ip32);
   u32fromu8(ar_hdr->ar_tip, &dst_ip32);
   if (src_ip32 == dst_ip32)
	return 1;
   return 0;
}
