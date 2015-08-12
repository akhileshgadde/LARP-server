#include "../hdrs/htonl3.h"

void htonl3(uint32_t label, struct label_stack *l_stack)
{
  if (print_msgs)
      printf("label before bit shifting: %u\n", label);
  label = (label << 12);
  /* bits numbered from low-order bytes */
  l_stack->label_beg = (label >> 24) & 0xff; /* bits 16-23 */
  l_stack->label_mid = (label >> 16)  & 0xff; /* bits 8-15 */
  l_stack->label_end = (label >> 12) & 0x0f; /* bits 0-3 */
  if (entropy_flag)
  	l_stack->entropy   = 1;  
  else
  	l_stack->entropy   = 0;
  
  l_stack->dummy     = 0; /* 6,7,8th bits not considered. Taken as dummy */
  if(print_debugs)
	  print_label_bytes(label, l_stack); 
}

void print_label_bytes(uint32_t label, struct label_stack *l_stack)
{
  printf("Label_32bit: %u\n", label);
  printf("label_beg: %u\n", l_stack->label_beg);
  printf("label_mid: %u\n", l_stack->label_mid);
  printf("label_end: %u\n", l_stack->label_end);
  printf("entropy flag: %u\n", l_stack->entropy);
  printf("Dummy bits: %u\n", l_stack->dummy);
} 
