#include "debug.h"
#include "larp_def.h"

void htonl3(uint32_t label, struct label_stack *l_stack);
void print_label_bytes(struct label_stack *l_stack);


void htonl3(uint32_t label, struct label_stack *l_stack)
{
  /* bits numbered from low-order bytes */
  l_stack->label_beg = (label) & 0xff; /* bits 16-23 */
  l_stack->label_mid = (label >> 8)  & 0xff; /* bits 8-15 */
  l_stack->label_end = (label >> 16) & 0x0f; /* bits 0-3 */
  l_stack->entropy   = (label >> 20) & 0x01;  /* 5th bit for entropy flag, bit numbers starts with 0  */
  l_stack->dummy     = 0; /* 6,7,8th bits not considered. Taken as dummy */
  #ifdef DEBUG
  print_label_bytes(l_stack);
  #endif 
}

void print_label_bytes(struct label_stack *l_stack)
{
  printf("label_beg: %u\n", l_stack->label_beg);
  printf("label_mid: %u\n", l_stack->label_mid);
  printf("label_end: %u\n", l_stack->label_end);
  printf("entropy flag: %u\n", l_stack->entropy);
  printf("Dummy bits: %u\n", l_stack->dummy);
} 
