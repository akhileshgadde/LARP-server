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

/*label_range enabled/disabled flag */
int label_range_en = 0;

/*label range min and max values */
uint32_t label_range_min = 0;
uint32_t label_range_max = 0;

/*enable/disable hexdump flag */
int hex_dump_flag = 0;

/* To parse the command line arguments and set flags accordingly */
int parse_cmdline (int argc, char *argv[])
{
  int long_index = 0;
  int opt = 0;
  char *result = NULL;
  while ((opt = getopt_long(argc, argv, "l:ar:m:e:nhpd", long_options, &long_index)) != -1)
  {
     switch (opt) {
	case 'l': //printf("Label_count: %d\n", atoi(optarg));
		  label_count = strtol(optarg, NULL, 0);
		  if(label_count <= 0)
		  {
			printf("Invalid label count\n");
			print_correct_usage();
			exit (EXIT_FAILURE);
		  }
		  break;
	case 'a': attr_tlv_flag = 1; /*enable ATTR_TLV */
		  break;
		  //if(optarg) { /*metric is optional, else a randomly generated metric value is used */
			//attr_metric = strtoul(optarg, 0, 10);
			//printf("ATTR_METRIC: %u\n", attr_metric);
		  //}
	case 'h': hex_dump_flag = 1;
		  break;
	case 'm': attr_metric = strtoul(optarg, NULL, 0);
//		  printf("ATTR_METRIC in parse_cmd_args: %u\n", attr_metric);
		  if (!((attr_metric >= METRIC_MIN) && (attr_metric <= METRIC_MAX)))
		  {
			printf("Invalid metric value\n");
			print_correct_usage();
			exit (EXIT_FAILURE);
		  }
		  break;
	case 'e': entropy_flag = strtol(optarg, NULL, 0);
		  if ((entropy_flag != 0) && (entropy_flag != 1))
		  {
			printf("Invalid entropy flag\n");
			print_correct_usage();
			exit (EXIT_FAILURE);
		  }
		  break;
	case 'n': no_check_flag = 1; /* enable no check flag */
		  break;
	case 'p': print_msgs = 1; /* Enable print messages */
		  break;
	case 'd': print_debugs = 1; /* enable debugs */
		  break;
	case 'r': result = strtok(optarg,", -");
		  if  (result)
			label_range_min = strtoul(result, NULL, 0);	
		  result = strtok(NULL, ",");
		  if  (result)
			label_range_max = strtoul(result, NULL, 0);
		  if ((label_range_min == 0) || label_range_max == 0 || label_range_min > label_range_max)
		  {
			printf("Label range is not specified/incorrect\n");
			print_correct_usage();
			exit (EXIT_FAILURE);
		  }
		  //printf("Label range: %u-%u\n", label_range_min, label_range_max);
		  label_range_en = 1; /* enable label range flag */
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
  printf("Usage: ./server [-n] [-p] [-d] [-h] [-a] [-m num] [-l num] [-r min,max] [-e 1/0]\n");
  printf("\t\t\tor\n");
  printf("       ./server [--no_check] [--print_msgs] [--print_debugs] [--print_hex_dump] [--attr_tlv] [--attr_metric num] [--label_count num] [--label_range min,max] [--entropy_flag 1/0]\n");
}
  	 
