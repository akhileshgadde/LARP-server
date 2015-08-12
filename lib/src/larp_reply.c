#include "../hdrs/larp_reply.h"

int larp_reply_pkt(struct arphdr *ar_hdr, struct sockaddr_ll *recv_addr)
{
  int send_sockfd;
  int i, send_bytes, frame_length;
  uint32_t ipaddr_n32;
  //char ipaddr_p[INET_ADDRSTRLEN]; /* to print IP address in presentation format */
  struct sockaddr_ll send_addr;
  struct arphdr send_arhdr; /* standard ARP header fields */
  struct tlv_type_len type_len; /* variable of type-len struct */
  struct label_stack *l_stack = (struct label_stack *) malloc(label_count * sizeof(struct label_stack)); /* For label stack */
  struct attr_tlv a_tlv; /* For Attributes TLV */
  char if_name[IFNAMSIZ];
  uint32_t metric_val;
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
  send_addr.sll_halen = ETH_ALEN; /*6 bytes for Mac address */
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
  memcpy(buffer, ar_hdr->ar_sha, ETH_ALEN * sizeof(uint8_t)); /* destination MAC*/
  memcpy(buffer + ETH_ALEN, s_haddr, ETH_ALEN * sizeof(uint8_t)); /* source MAC */
  /* Fill ETH_TYPE = ETH_P_ARP for ARP */
  buffer[12] = ETH_P_ARP / 256;
  buffer[13] = ETH_P_ARP % 256; 
  /*Fill in ARP header fileds */
  send_arhdr.ar_htype = htons(ARPHRD_LARP);  
  send_arhdr.ar_ptype = htons(ETH_P_IP); /* code for IPV6 can be added as needed*/
  send_arhdr.ar_hln = ETH_ALEN;
  send_arhdr.ar_pln = IP_ADDR_SIZE;
  send_arhdr.ar_op = htons(LARP_REPLY_OP); /* LARP reply = 2 */
  /* source and destination MAC address */
  memcpy(&send_arhdr.ar_sha, s_haddr, ETH_ALEN * sizeof(uint8_t));
  memcpy(&send_arhdr.ar_tha, ar_hdr->ar_sha, ETH_ALEN * sizeof(uint8_t));
  /* Source and destination IP filled using received LARP request */
  memcpy(&send_arhdr.ar_sip, ar_hdr->ar_tip, IP_ADDR_SIZE * sizeof(uint8_t));
  memcpy(&send_arhdr.ar_tip, ar_hdr->ar_sip, IP_ADDR_SIZE * sizeof(uint8_t));
  
  /* Copying ARP header to sending packet buffer */
  memcpy(buffer + ETH_HDR_SIZE, &send_arhdr, sizeof(struct arphdr));
  
  /*Fill in type, length and label for TLV_LST */
  fill_type_label_stack(&type_len, l_stack, ar_hdr->ar_tip);
  
  /* copy the tlv TLV_LST struct to sending buffer */
  memcpy(buffer + ETH_HDR_SIZE + ARP_HDR_SIZE, &type_len, TYPE_LEN_SIZE);
  memcpy(buffer + ETH_HDR_SIZE + ARP_HDR_SIZE + TYPE_LEN_SIZE, l_stack, label_count * LABEL_STACK_SIZE);
  
  /*Add Attribute TLV to sending buffer only if it is enabled */
  if(attr_tlv_flag) {
        /* zero the type_len struct so that now the new values can be held for attr_tlv*/
        memset (&type_len, 0, TYPE_LEN_SIZE);
  
  	/* Fill in struct values for attributes TLV */
        fill_type_attribute_tlv(&type_len, &a_tlv, ar_hdr->ar_tip);
        /* Copy the ATTR_TLV struct to the sending buffer */
        memcpy(buffer + ETH_HDR_SIZE + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE, &type_len, TYPE_LEN_SIZE);
        memcpy(buffer + ETH_HDR_SIZE + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE + TYPE_LEN_SIZE, &a_tlv, ATTR_TLV_SIZE);  
  }
  
  /* Frame length = Ethernet header + ARP header + type_len + label_stack */
  frame_length = ETH_HDR_SIZE + ARP_HDR_SIZE + TYPE_LEN_SIZE + label_count * LABEL_STACK_SIZE + attr_tlv_flag * (TYPE_LEN_SIZE + ATTR_TLV_SIZE);
 
  /* sending the filled packet buffer using sendto*/
  send_bytes = sendto(send_sockfd, buffer, frame_length, 0, (SA *) &send_addr, sizeof(send_addr));
  if(send_bytes <= 0) { /* Return value: 0 is no bytes sent and negative is error */
	perror("Sendto() for LARP reply failed\n");
	exit (1);
  }
  get_interface_name (send_addr.sll_ifindex, if_name);
  printf("Sent LARP reply to %u.%u.%u.%u for target %u.%u.%u.%u on interface: %s with label(s): ", send_arhdr.ar_tip[0], send_arhdr.ar_tip[1], send_arhdr.ar_tip[2], send_arhdr.ar_tip[3], send_arhdr.ar_sip[0], send_arhdr.ar_sip[1], send_arhdr.ar_sip[2], send_arhdr.ar_sip[3],if_name);
  u32fromu8(send_arhdr.ar_sip, &ipaddr_n32);
  uint32_t *label_stk = (uint32_t *) calloc (0, label_count * sizeof(uint32_t));
  memcpy(label_stk, find_label(ipaddr_n32), label_count * sizeof(uint32_t));
  print_label_stack(label_stk);
  metric_val = find_metric(ipaddr_n32);
  if (metric_val != 0)
        printf("and with metric: %u\n", find_metric(ipaddr_n32)); 
  else
	printf("and with no ATTR_TLV.\n");
  if (hex_dump_flag) /* print hex_dump of packet if flag enabled */
     hexDump (ntohs(send_arhdr.ar_op), buffer+14 , frame_length - 14);
  /* freeing dynamically allocated memory */
  free (label_stk);
  free(s_haddr);
  free(l_stack);
}

void print_label_stack(uint32_t *label_stk)
{
  int i;
  if (label_stk != NULL) 
  {
      for (i = 0; i < label_count-1; i++)
           printf("%u, ",label_stk[i]);
      printf("%u ", label_stk[i]);
  }
  else
	printf("No Label entry.");
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
void fill_type_label_stack(struct tlv_type_len *type_len, struct label_stack *l_stack, uint8_t ipaddr[IP_ADDR_SIZE])
{
  struct ip_label_table *l_entry;
  int i;
  type_len->type = TLV_LST_TYPE; /* 1 for TLV_LST */
  type_len->len = label_count * LABEL_STACK_SIZE; /* Length(in bytes) = (number of labels * 3) */
  l_entry = get_label_entry(ipaddr);
  /* label present in l_entry struct to be put in label_stack struct, htonl3 converts it to network byte order */
  for (i=0; i < label_count; i++)
	htonl3(l_entry->label[i], &l_stack[i]);
}

/* Fill the values for attribute type TLV */
void fill_type_attribute_tlv(struct tlv_type_len *type_len, struct attr_tlv *a_tlv, uint8_t ipaddr[IP_ADDR_SIZE])
{
  char ip[INET_ADDRSTRLEN];
  uint32_t ipaddr_32;
  type_len->type = TLV_ATTR_TYPE; /*2 for ATTR_TLV */
  type_len->len = ATTR_TLV_SIZE; /* 4 bytes for metric */
  /*convert the ip address stored in uint8_t[4] to uint32_t */
  u32fromu8(ipaddr, &ipaddr_32);
  //pthread_mutex_lock(&mutex_lock);
  if (print_msgs) {
  printf("Metric for ipaddr: %s: Decimal: %u, Hex:%06x,metric in nw format: %06x\n", inet_ntop(AF_INET, &ipaddr_32, ip, sizeof(ip)), find_metric(ipaddr_32), find_metric(ipaddr_32), htonl(find_metric(ipaddr_32)));
  }
 // if(attr_metric == 0) /* No Metric given by user */
  a_tlv->metric = htonl(find_metric(ipaddr_32)); /* convert metric to nw byte order and store in structure */
 // else
 //	a_tlv->metric = htonl(attr_metric);
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
  return DEFAULT_METRIC; /* default Metric */
}
