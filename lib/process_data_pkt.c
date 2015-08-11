#include "process_data_pkt.h"

/* Parse and process the MPLS data packets */
int process_mpls_data (void *buff, struct sockaddr_ll *addr)
{
  mpls_t *mpls_hdr;
  /* add code for repeated parsing of labels */
  mpls_hdr = (mpls_t *) buff;
  uint32_t *label = (uint32_t *) malloc(label_count * sizeof(uint32_t));
  int i = 0;
  
  mpls_hdr->label;
  ip_t *ip_hdr = (ip_t *) buff + MPLS_HDR_SIZE;
  uint32_t ipaddr;
  u32fromu8(ip_hdr->src_addr, &ipaddr);
  char ip[INET_ADDRSTRLEN];
  if (print_msgs)
  {
	printf("Received MPLS data packet from %s with label: %u\n", inet_ntop(AF_INET, &ipaddr, ip, sizeof(ip)), label);
  }
//  memcpy(label, find_label(ipaddr), label_count * sizeof(uint32_t));
    

}
