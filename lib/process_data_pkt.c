#include "process_data_pkt.h"

/* Parse and process the MPLS data packets */
int process_mpls_data (void *buff, struct sockaddr_ll *addr)
{
  mpls_t *mpls_hdr; /* MPLS header struct variable */
  uint32_t offset = 0; /* to increment in  the MPLS label stack */
  int bos = 0; /* Flag for bottom of stack */
  /* Allocate memory to hold all labels */
  uint32_t *mpls_label = (uint32_t *) calloc (0, label_count * sizeof(uint32_t));
  uint32_t *label_cache = (uint32_t *) calloc (0, label_count * sizeof(uint32_t));
  int i = 0;
  /* add code for repeated parsing of labels */
  while (i < label_count || bos == 1) /*label_count reached or bottom of stack flag set */
  {
     mpls_hdr = (mpls_t *) buff + offset;
     mpls_label[i] = mpls_hdr->label;
     bos = mpls_hdr->stack;
     offset += MPLS_HDR_SIZE; 
     i++;
  }
  /* start of Ip header */
  ip_t *ip_hdr = (ip_t *) buff + offset;
  uint32_t ipaddr;
  /* convert ip address[4] to an unsigned int32 */
  u32fromu8(ip_hdr->src_addr, &ipaddr);
  char ip[INET_ADDRSTRLEN]; /* to print IP address in presentation format */
  printf("Received MPLS data packet from %s\n", inet_ntop(AF_INET, &ipaddr, ip, sizeof(ip)));
  if (find_label(ipaddr) != NULL) {
	memcpy(label_cache, find_label(ipaddr), label_count * sizeof(uint32_t));
	check_label_correctness(label_cache, mpls_label);
  }
  else {
        printf("No entry for Ip address %s in L-ARP cache table\n", ip);
	invalid_pkt_ct++;
  }
  if (print_debugs)
	print_pkt_stats();
  free(mpls_label);
  free(label_cache);
}

void check_label_correctness (uint32_t *local_cache, uint32_t *data_pkt_labels)
{
  int i=0;
  int no_match_flag = 0;
  while (i < label_count)
  {
     if  (local_cache[i] != data_pkt_labels[i]) {
	 no_match_flag = 1;
	 break;
     }
     i++;
  }
  if (no_match_flag == 1) /* Atleast one incorrect label in the packet */
	valid_incorrect_label++;
  else /* all labels are correct */
	valid_correct_label++;
}

void print_pkt_stats()
{
  printf("\nInvalid MPLS packets count: %u\n", invalid_pkt_ct);
  printf("Valid MPLS packets but incorrect label: %u\n", valid_incorrect_label);
  printf("Valid MPLS packets and correct label: %u\n\n", valid_correct_label);
}  
