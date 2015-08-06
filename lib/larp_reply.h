#include "htonl3.h"

void get_mac(int if_index, unsigned char *t_haddr);
int larp_reply_pkt(struct arphdr *ar_hdr, struct sockaddr_ll *recv_addr);
uint8_t * allocate_ustrmem (int len);
void fill_type_label_stack(struct tlv_type_len *type_len, struct label_stack *l_stack, uint8_t ipaddr[4]);
void fill_type_attribute_tlv(struct tlv_type_len *type_len, struct attr_tlv *a_tlv, uint8_t ipaddr[4]);
uint32_t find_metric(uint32_t ipaddr_32);
