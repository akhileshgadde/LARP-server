#include "parse_cmdline_args.h"
/*count for number of labels - given in command line by user */
int label_count = 1;

/* count for number of attributes in ATTR_TLV */
uint32_t attr_metric = 0;

/* enable/disable ATTR_TLV */
int attr_tlv_flag = 0; /*disabled by default */

/* entropy flag */
int entropy_flag = 0; /* disabled by default */

/* No check of ARP Htype */
int no_check_flag = 0; /* disabled by default */

/* print messages enable/disable */
int print_msgs = 0; /* disabled by default */

/* print debugs */
int print_debugs = 0; /* disabled by default */

/* To parse the command line arguments and set flags accordingly */
int parse_cmdline (int argc, char *argv[])
{
  int long_index = 0;
  int opt = 0;
  while ((opt = getopt_long(argc, argv, "l:am:e:npd", long_options, &long_index)) != -1)
  {
     switch (opt) {
	case 'l': //printf("Label_count: %d\n", atoi(optarg));
		  label_count = strtol(optarg, NULL, 0);
		  break;
	case 'a': attr_tlv_flag = 1; /*enable ATTR_TLV */
		  break;
		  //if(optarg) { /*metric is optional, else a randomly generated metric value is used */
			//attr_metric = strtoul(optarg, 0, 10);
			//printf("ATTR_METRIC: %u\n", attr_metric);
		  //}
	case 'm': attr_metric = strtoul(optarg, NULL, 0);
//		  printf("ATTR_METRIC in parse_cmd_args: %u\n", attr_metric);
		  break;
	case 'e': entropy_flag = strtol(optarg, NULL, 0);
		  break;
	case 'n': no_check_flag = 1; /* enable no check flag */
		  break;
	case 'p': print_msgs = 1; /* Enable print messages */
		  break;
	case 'd': print_debugs = 1; /* enable debugs */
		  break;
	default:  print_correct_usage();
		  exit (EXIT_FAILURE);
     }
  }
  if(argc == 1) /* to check if any options were provided */
	return 1;
  else
	return 0;
}		

void print_correct_usage()
{
  printf("Usage:\n ./server [-n] [-p] [-d] [-a] [-m num] [-l <num>] [-e <1/0>]\n");
  printf("\t\tor\n");
  printf("./server [--no_check] [--print_msgs] [--print_debugs] [--attr_tlv] [--attr_metric num] [label_count <num>] [--entropy_flag <1/0>]\n");
}
  	 
