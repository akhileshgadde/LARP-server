#include "debug.h"
#include "larp_def.h"
#include "inc_hdrs.h"
#include "htonl3.h"
#include "label_mapping_table.h"

void get_mac(int if_index, unsigned char *t_haddr);
int larp_reply_pkt(struct arphdr *ar_hdr, struct sockaddr_ll *recv_addr);
uint8_t * allocate_ustrmem (int len);
void fill_type_label_stack(struct tlv_type_len *type_len, struct label_stack *l_stack, uint8_t ipaddr[4]);
void fill_type_attribute_tlv(struct tlv_type_len *type_len, struct attr_tlv *a_tlv, uint8_t ipaddr[4]);
uint32_t find_metric(uint32_t ipaddr_32);

int larp_reply_pkt(struct arphdr *ar_hdr, struct sockaddr_ll *recv_addr)
{
  int send_sockfd;
  int i, send_bytes, frame_length;
  struct sockaddr_ll send_addr;
  struct arphdr send_arhdr; /* standard ARP header fields */
  struct tlv_type_len type_len; /* variable of type-len struct */
  struct label_stack *l_stack = (struct label_stack *) malloc(label_count * sizeof(struct label_stack)); /* For label stack */
  struct attr_tlv a_tlv; /* For Attributes TLV */
  unsigned char *s_haddr = allocate_ustrmem (6);
  /*create a RAW PF_PACKET socket for sending out the reply*/
  send_sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
  if (send_sockfd < 0) { /* socket function returns negative value on error */
	printf("Sending socket creation failed\n"); 
        exit(1);
  }
  uint8_t *buffer = allocate_ustrmem (IP_MAXPACKET);
  /* Making the buffer all zeros */
  memset(buffer, 0, ETH_FRAME_LEN);
  /* Get the local interface MAC address based on the received interface index*/
  get_mac (recv_addr->sll_ifindex, s_haddr);
  /*zeroing out the struct sockaddr_ll structure*/
  memset(&send_addr, 0, sizeof(struct sockaddr_ll));
  /* Fill in the values for struct sockaddr_ll structure*/

  send_addr.sll_family = PF_PACKET;
  send_addr.sll_protocol = htons(ETH_P_ARP);
  send_addr.sll_ifindex = recv_addr->sll_ifindex;
  send_addr.sll_halen = 6; /*6 bytes for Mac address */
  /* filling target MAC address from source MAC field of received LARP req */
  send_addr.sll_addr[0] = ar_hdr->ar_sha[0];
  send_addr.sll_addr[1] = ar_hdr->ar_sha[1];
  send_addr.sll_addr[2] = ar_hdr->ar_sha[2];
  send_addr.sll_addr[3] = ar_hdr->ar_sha[3];
  send_addr.sll_addr[4] = ar_hdr->ar_sha[4];
  send_addr.sll_addr[5] = ar_hdr->ar_sha[5];
  /* Not used */
  send_addr.sll_addr[6] = 0x00;
  send_addr.sll_addr[7] = 0x00;  
  #if 0
  for (i=0;i<5;i++)
	printf("%02x:", s_haddr[i]);
  printf("%02x\n", s_haddr[i]);
  #endif
 
  /* Start filling the buffer starting with Ethernet header */
  
  /* Fill in ethernet header*/
  memcpy(buffer, ar_hdr->ar_sha, 6 * sizeof(uint8_t)); /* destination MAC*/
  memcpy(buffer + 6, s_haddr, 6 * sizeof(uint8_t)); /* source MAC */
  /* Fill ETH_TYPE = ETH_P_ARP for ARP */
  buffer[12] = ETH_P_ARP / 256;
  buffer[13] = ETH_P_ARP % 256; 
  /*Fill in ARP header fileds */
  send_arhdr.ar_htype = htons(ARPHRD_LARP);  
  send_arhdr.ar_ptype = htons(ETH_P_IP); /* code for IPV6 can be added as needed*/
  send_arhdr.ar_hln = 6;
  send_arhdr.ar_pln = 4;
  send_arhdr.ar_op = htons(2); /* LARP reply = 2 */
  /* source and destination MAC address */
  memcpy(&send_arhdr.ar_sha, s_haddr, 6 * sizeof(uint8_t));
  memcpy(&send_arhdr.ar_tha, ar_hdr->ar_sha, 6 * sizeof(uint8_t));
  /* Source and destination IP filled using received LARP request */
  memcpy(&send_arhdr.ar_sip, ar_hdr->ar_tip, 4 * sizeof(uint8_t));
  memcpy(&send_arhdr.ar_tip, ar_hdr->ar_sip, 4 * sizeof(uint8_t));
  
  /* Copying ARP header to sending packet buffer */
  memcpy(buffer + 14, &send_arhdr, sizeof(struct arphdr));
  
  /*Fill in type, length and label for TLV_LST */
  fill_type_label_stack(&type_len, l_stack, ar_hdr->ar_tip);
  
  /* copy the tlv TLV_LST struct to sending buffer */
  memcpy(buffer + 14 + ARP_HDR_SIZE, &type_len, TYPE_LEN_SIZE);
  memcpy(buffer + 14 + ARP_HDR_SIZE + TYPE_LEN_SIZE, l_stack,label_count * LABEL_STACK_SIZE);
  
  /* zero the type_len struct so that now the new values can be held for attr_tlv*/
  memset (&type_len, 0, TYPE_LEN_SIZE);
  /* Fill in struct values for attributes TLV */
  fill_type_attribute_tlv(&type_len, &a_tlv, ar_hdr->ar_tip);
  /* Copy the ATTR_TLV struct to the sending buffer */
  memcpy(buffer + 14 + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE, &type_len, TYPE_LEN_SIZE);
  memcpy(buffer + 14 + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE + TYPE_LEN_SIZE, a_tlv->metric, ATTR_TLV_SIZE);  

  
  /* Frame length = Ethernet header + ARP header + type_len + label_stack */
  frame_length = 6 + 6 + 2 + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE + TYPE_LEN_SIZE + attr_count * ATTR_TLV_SIZE;
 
  /* sending the filled packet buffer using sendto*/
  send_bytes = sendto(send_sockfd, buffer, frame_length, 0, (SA *) &send_addr, sizeof(send_addr));
  if(send_bytes <= 0) { /* Return value: 0 is no bytes sent and negative is error */
	perror("Sendto() failed\n");
	exit (1);
  }
  #ifdef DEBUG
  printf("Successfuly sent LARP reply of size: %d\n", send_bytes);
  #endif
  /* freeing dynamically allocated memory */
  free(s_haddr);
  free(l_stack);
}

void get_mac(int if_index, unsigned char *s_haddr)
{
  int sockfd;
  struct ifreq if_mac;
  if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1)
      perror("get_mac Socket");
  memset(&if_mac, 0, sizeof(struct ifreq));
  if_mac.ifr_ifindex = if_index;
   /* Use ioctl() to look up interface name from Interface index*/
  if (ioctl(sockfd, SIOCGIFNAME, &if_mac) < 0)
      perror("IOCTL: SIOCGIFNAME error");
  /*  Use ioctl() to look up MAC address from interface name */
  if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
      perror("IOCTL: SIOCGIFHWADDR error");
  memcpy(s_haddr, if_mac.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));
  //return ((unsigned char ) if_mac.ifr_hwaddr.sa_data);
}

/* Allocate memory for an array of unsigned chars */
uint8_t * allocate_ustrmem (int len)
{
  void *tmp;
  if (len <= 0) {
    fprintf (stderr, "ERROR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", len);
    exit (EXIT_FAILURE);
  }
  tmp = (uint8_t *) malloc (len * sizeof (uint8_t));
  if (tmp != NULL) {
    memset (tmp, 0, len * sizeof (uint8_t));
    return (tmp);
  } else {
    fprintf (stderr, "ERROR: Cannot allocate memory for array allocate_ustrmem().\n");
    exit (EXIT_FAILURE);
  }
}

/* Fill the values for label stack TLV */
void fill_type_label_stack(struct tlv_type_len *type_len, struct label_stack *l_stack, uint8_t ipaddr[4])
{
  struct ip_label_table *l_entry;
  type_len->type = 1; /* 1 for TLV_LST */
  type_len->len = label_count * 3; /* Length(in bytes) = (number of labels * 3) */
  l_entry = get_label_map(ipaddr);
  /* label present in l_entry struct to be put in label_stack struct, htonl3 converts it to network byte order */
  htonl3(l_entry->label[0], l_stack);
}

/* Fill the values for attribute type TLV */
void fill_type_attribute_tlv(struct tlv_type_len *type_len, struct attr_tlv *a_tlv, uint8_t ipaddr[4])
{
  char ip[INET_ADDRSTRLEN];
  uint32_t ipaddr_32;
  type_len->type = 2; /*2 for ATTR_TLV */
  type_len->len = 4; /* 4 bytes for metric */
  /*convert the ip address stored in uint8_t[4] to uint32_t */
  u32fromu8(ipaddr, &ipaddr_32);
  //pthread_mutex_lock(&mutex_lock);
  printf("Metric for ipaddr: %s: %06x,metric in nw format: %06x\n", inet_ntop(AF_INET, &ipaddr_32, ip, sizeof(ip)), find_metric(ipaddr_32), htonl(find_metric(ipaddr_32)));
  a_tlv->metric = htonl(find_metric(ipaddr_32)); /* convert metric to nw byte order and store in structure */
  printf("a_tlv: %06x\n", a_tlv->metric);
  //pthread_mutex_unlock(&mutex_lock);
}

/* To find the metric value for ipaddr */
uint32_t find_metric(uint32_t ipaddr_32)
{
  struct ip_label_table *curr = table_head;
  //pthread_mutex_lock(&mutex_lock);
  while (curr != NULL)
  {
	if (ipaddr_32 == curr->ipaddr) {  /* Entry for IP found in LARP table */
		//pthread_mutex_unlock(&mutex_lock);
		return curr->metric;
	}
	else
		curr = curr->next;
  }
  //pthread_mutex_unlock(&mutex_lock);
  return 10; /* default Metric */
}
