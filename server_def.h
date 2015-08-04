#ifndef SERVER_DEF_H
#define SERVER_DEF_H
#include "inc_hdrs.h"
//#include "process_pkt.h"
#include "debug.h"

void * msg_recv(void *sockfd);
//int process_larp_req(void *buff, struct sockaddr_ll *addr);
//int check_larp_pkt(struct arphdr *ar_hdr);

/*Arp header structure*/
struct arphdr
{
    uint16_t ar_htype;		/* Format of hardware address.  */
    uint16_t ar_ptype;		/* Format of protocol address.  */
    uint8_t  ar_hln;		/* Length of hardware address.  */
    uint8_t  ar_pln;		/* Length of protocol address.  */
    uint16_t ar_op;		/* ARP opcode (command).  */
    uint8_t  ar_sha[ETH_ALEN];	/* Sender hardware address.  */
    uint8_t  ar_sip[4];		/* Sender IP address.  */
    uint8_t  ar_tha[ETH_ALEN];	/* Target hardware address.  */
    uint8_t  ar_tip[4];		/* Target IP address.  */
};

/*structure for type and length in TLVs */
struct tlv_type_len
{
   uint8_t type;
   uint8_t len;
   #if 0
   uint8_t label_beg, label_mid;
   uint8_t label_end:4, entropy:1, dummy:3;
   #endif
};

/* Label stack structure */
struct label_stack
{
  uint8_t label_beg, label_mid;
  uint8_t label_end:4;
  uint8_t entropy:1;
  uint8_t dummy:3;
};

/* Attributes TLV structure */
struct attr_tlv
{
  uint32_t metric;
};

#if 0
struct ip_label_table
{
  uint32_t ipaddr;
  uint32_t label[11];
  int label_count;
  uint32_t timestamp;
  struct ip_label_table *next;
};
#endif

#endif
