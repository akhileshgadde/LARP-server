#include "../hdrs/process_data_pkt.h"

/* Parse and process the MPLS data packets */
int process_mpls_data (void *buff, struct sockaddr_ll *addr, int pkt_len)
{
  mpls_t *mpls_hdr; /* MPLS header struct variable */
  uint32_t offset = 0; /* to increment in  the MPLS label stack */
  int bos = 0; /* Flag for bottom of stack */
  int l_lab_ct = 0; /* count number of labels */
  /* Allocate memory to hold all labels */
  uint32_t *mpls_label = (uint32_t *) calloc (0, label_count * sizeof(uint32_t));
  uint32_t *label_cache = (uint32_t *) calloc (0, label_count * sizeof(uint32_t));
  int i = 0;
  printf("Inside process_mpls_data\n");
  hexDump(MPLS_DATA, buff, pkt_len); 
  /* add code for repeated parsing of labels */
  while (bos == 0) /*label_count reached or bottom of stack flag set */
  {
     mpls_hdr = (mpls_t *) buff + offset;
     mpls_label[i] = mpls_hdr->label;
     bos = mpls_hdr->stack;
     offset += MPLS_HDR_SIZE; 
     l_lab_ct++;
     i++;
  }
  if(l_lab_ct != label_count)
  {
	printf("Incorrect label count in MPLS data pkt, l_lab_ct:%d, label_count:%d\n", l_lab_ct, label_count);
        valid_incorrect_label++;
	return 1;
  }
  /* start of Ip header */
  ip_t *ip_hdr = (ip_t *) (buff + offset);
  uint32_t src_ipaddr, dst_ipaddr;
  /* convert ip address[4] to an unsigned int32 */
  u32fromu8(ip_hdr->src_addr, &src_ipaddr);
  u32fromu8(ip_hdr->dest_addr, &dst_ipaddr);
  char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN]; /* to print IP address in presentation format */
  if (inet_ntop(AF_INET, &src_ipaddr, src_ip, sizeof(src_ip)) == NULL)
  {
	printf("Inet_ntop error: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
  }
  if (inet_ntop(AF_INET, &dst_ipaddr, dst_ip, sizeof(dst_ip)) == NULL)
  {
       printf("Inet_ntop error: %s\n", strerror(errno));
       exit(EXIT_FAILURE);
  }  
  printf("Received MPLS data packet from %s for %s\n", src_ip, dst_ip);
  if (find_label(dst_ipaddr) != NULL) {
	memcpy(label_cache, find_label(dst_ipaddr), label_count * sizeof(uint32_t));
	check_label_correctness(label_cache, mpls_label);
  }
  else {
	if (print_msgs)
	        printf("No entry for Ip address %s in L-ARP cache table\n", dst_ip);
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
  printf("\nInvalid MPLS packets count: %u, Valid MPLS packets but incorrect label: %u, Valid MPLS packets and correct label: %u\n", invalid_pkt_ct, valid_incorrect_label, valid_incorrect_label);
}  
