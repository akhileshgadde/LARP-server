#ifndef SERVER_DEF_H
#define SERVER_DEF_H

#include "inc_hdrs.h"
#include "debug.h"
#include "label_mapping_table.h"
#define ARP_REQ_SIZE 46 /* size of ARP request */

void * msg_recv(void *sockfd);

/*Arp header structure*/
struct arphdr
{
    uint16_t ar_htype;		/* Format of hardware address.  */
    uint16_t ar_ptype;		/* Format of protocol address.  */
    uint8_t  ar_hln;		/* Length of hardware address.  */
    uint8_t  ar_pln;		/* Length of protocol address.  */
    uint16_t ar_op;		/* ARP opcode (command).  */
    uint8_t  ar_sha[ETH_ALEN];	/* Sender hardware address.  */
    uint8_t  ar_sip[IP_ADDR_SIZE];		/* Sender IP address.  */
    uint8_t  ar_tha[ETH_ALEN];	/* Target hardware address.  */
    uint8_t  ar_tip[IP_ADDR_SIZE];		/* Target IP address.  */
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
  #if defined(__BIG_ENDIAN_BITFIELD)
  uint8_t label_beg, label_mid;
  uint8_t label_end:4; /* last 4 bits of 20 byte label */
  uint8_t entropy:1; /* 1 bit entropy flag */
  uint8_t dummy:3; /* 3 dummy bits - unused currently */
  
  #elif defined(__LITTLE_ENDIAN_BITFIELD)
  uint8_t label_beg, label_mid;
  uint8_t dummy:3; /* 3 dummy bits - unused currently */
  uint8_t entropy:1; /* 1 bit entropy flag */
  uint8_t label_end:4; /* last 4 bits of 20 byte label */
  #endif
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
