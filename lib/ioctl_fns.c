#include "ioctl_fns.h"

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
  close (sockfd);
}

void get_interface_name (int if_index, char *if_name)
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
  memcpy(if_name, if_mac.ifr_name, IFNAMSIZ-1);
  if_name[IFNAMSIZ-1] = '\0'; /* Add Null character in the end */
  close (sockfd);
}
