#include "server_def.h"
#define MPLS_HDR_SIZE 4

typedef struct {
  #if defined(__BIG_ENDIAN_BITFIELD)
      u_int32_t   label:20,
                  exp:3,
                  stack:1,
                  ttl:8;
  #elif defined(__LITTLE_ENDIAN_BITFIELD)
      u_int32_t   ttl:8,
                  stack:1,
                  exp:3,
                  label:20;
  #endif
} mpls_t;

typedef struct {
    u_int8_t    ip_vhl;         /* version and hl */
    u_int8_t    ip_tos;         /* type of service */
    u_int16_t   ip_len;         /* total length */
    u_int16_t   ip_id;          /* identification */
    u_int16_t   ip_off;         /* fragment offset field */
    u_int8_t    ip_ttl;         /* time to live */
    u_int8_t    ip_proto;       /* protocol */
    u_int16_t   ip_sum;         /* checksum */
    u_int8_t    src_addr[4]     /* source address */
    u_int8_t    dest_addr[4];   /* destination address */
} ip_t;

int process_mpls_data (void *buff, struct sockaddr_ll *addr);
