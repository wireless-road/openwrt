#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/can.h>
#include <time.h>

#include "can_defs.h"
#include "cprotocol.h"
#include "stm32_crc32.h"


unsigned char can_interface[5] = "can0";
char file_name[256] = "";
FILE* file_read;
long int file_len;
uint32_t can_addr = (1 << CAN_ADDR_BIT_POSITION);


int CAN_socker_init(char* interface) {
    int sc;
    struct ifreq ifr;
    struct sockaddr_can caddr;
    socklen_t caddrlen = sizeof(caddr);

    memset(&caddr, 0, sizeof(caddr));
    strcpy(ifr.ifr_name, interface);

    if ((sc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        printf("CAN socket error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    caddr.can_family = AF_CAN;
    if (ioctl(sc, SIOCGIFINDEX, &ifr) < 0) {
        printf("CAN setup error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    caddr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sc, (struct sockaddr *)&caddr, caddrlen) < 0) {
        printf("CAN bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return sc;
}

void print_usage(char *prg) {
    printf("\nUsage: %s -l <port> -d <port> -i <can interface>\n", prg);
    printf("   Version 0.93\n");
    printf("\n");
    printf("         -l <port>           listening UDP port for the server - default 15731\n");
    printf("         -d <port>           destination UDP port for the server - default 15730\n");
    printf("         -i <can int>        can interface - default can0\n");
    printf("         -f                  running in foreground\n\n");
    printf("         -v                  verbose output (in foreground)\n\n");
}

void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "a:f:r:i:h?")) != -1) {
        switch (opt) {
            case 'a':
            	can_addr = strtoul(optarg, (char **)NULL, 10);
                break;
            case 'i':
                strncpy(can_interface, optarg, sizeof(can_interface));
                break;
            case 'f':
				strncpy(file_name, optarg, sizeof(file_name));
                break;
            case 'r':
                break;
            case 'h':
            case '?':
                print_usage(basename(argv[0]));
                exit(EXIT_SUCCESS);
            default:
                printf("Unknown option %c\n", opt);
                print_usage(basename(argv[0]));
                exit(EXIT_FAILURE);
        }
    }
}

int32_t send_to_can(int can_sc, struct can_frame* frame)
{
	int32_t ret_val = 0;

	if (write(can_sc, frame, sizeof(*frame)) != sizeof(*frame))
	{
		printf("CAN write error: %s\n", strerror(errno));
		ret_val = -1;
	}

	return ret_val;
}

int32_t wait_can_response(int can_sc, struct can_frame* frame)
{
	int32_t ret_val = 0;
	fd_set readfds;

	/* 5 seconds timeout */
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(can_sc, &readfds);

	int32_t res = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
	if(res)
	{
		if (FD_ISSET(can_sc, &readfds)) // received a CAN frame
		{
			if (read(can_sc, frame, sizeof(*frame)) < 0)
			{
				fprintf(stderr, "CAN read error: %s\n", strerror(errno));
				ret_val = -1;
			}
		}else{
			ret_val = -1;
		}
	}else{
		ret_val = -1;
	}

	return ret_val;
}

uint32_t can_check_addr(struct can_frame* p_frame)
{
	uint32_t ret_val = 0;

	uint32_t addr = p_frame->can_id;
	addr &= CAN_ADDR_MASK;
	if(addr != CAN_MASTER_ADDR)
	{
		ret_val = 1;
	}

	return ret_val;
}

uint64_t timespec_to_ms(struct timespec ts)
{
	uint64_t val, val_l, val_h;
	val_h = 1000 * ts.tv_sec;
	val_l = ts.tv_nsec / 1000000;

	val = val_h + val_l;

	return val;
}

uint32_t check_timeout(uint64_t t_start, uint64_t timeinterval)
{
	uint32_t ret_val = 0;
	struct timespec ts;

	clock_gettime(CLOCK_REALTIME, &ts);

	uint64_t time_now = timespec_to_ms(ts);
	uint64_t time_delta = time_now - t_start;
	if(time_delta >= timeinterval)
	{
		ret_val = 1;
	}

	return ret_val;
}

int32_t check_input_file(char* file_name)
{
	uint32_t file_name_len = strlen(file_name);
	if(file_name_len == 0)
	{
		printf("file name not set\n");
		return -1;
	}

	file_read = fopen(file_name, "r+b");
	if(file_read == NULL)
	{
		printf("can not open file\n");
		return -1;
	}

	int res = fseek(file_read, 0, SEEK_END);
	if(res)
	{
		printf("can not seek position 0\n");
		return -1;
	}

	file_len = ftell(file_read);
	if(file_len == -1)
	{
		printf("get file size error\n");
		return -1;
	}

	printf("file size - %ld\n", file_len);

	res = fseek(file_read, LEN_INFO_FILE_POSITION, SEEK_SET);
	if(res)
	{
		printf("can not seek position 1\n");
		return -1;
	}

	uint32_t mem_len;
	size_t bytes_read = fread(&mem_len, sizeof(mem_len), 1, file_read);
	if(bytes_read != 1)
	{
		printf("bytes_read - %u\n", bytes_read);
		printf("can not read file\n");
		return -1;
	}

	mem_len *= 4UL;
	printf("mem_len - %u\n", mem_len);

	if(mem_len > MCU_MEM_SZ)
	{
		printf("file length is big\n");
		return -1;
	}

	if(mem_len != (uint32_t)file_len)
	{
		printf("file size is big\n");
		return -1;
	}

	res = fseek (file_read, 0, SEEK_SET);
	if(res)
	{
		printf("can not seek position 2\n");
		return -1;
	}

	return 0;
}

int32_t calc_CRC32_input_file(void)
{
	uint32_t crc32 = CRC_INITIALVALUE;
	uint8_t crc_data[1];
	uint32_t i;
	uint32_t sz = (uint32_t)file_len;
	sz -= 4U;

	printf("CRC32 calculating\n");

	for(i=0; i<sz; ++i)
	{
		size_t bytes_read = fread(crc_data, 1, 1, file_read);
		crc32 = stm32_sw_crc32_by_byte(crc32, crc_data, 1);
	}

	fwrite(&crc32, sizeof(crc32), 1, file_read);

	fclose(file_read);
	file_read = fopen(file_name, "rb");

	if(file_read == NULL)
	{
		printf("can not open file\n");
		return -1;
	}

	return 0;
}

int main(int argc, char **argv) {
    struct can_frame cframe;
    uint8_t rcv_data[8];
    int sc;		// CAN socket
    uint32_t addr;

    printf("--------------------------------------------------------\n");
    printf("------ HELLO CAN bootloader ----------------------------\n");
    printf("--------------------------------------------------------\n");    

    parse_args(argc, argv);

    printf("can_interface - %s\n", can_interface);
    printf("file_name - %s\n", file_name);
    printf("can addr - %u\n", can_addr);

    if(can_addr > CAN_ADD_MAX)
    {
    	printf("wrong CAN address\n");
    	printf("Exit\n");
		return 0;
    }

    can_addr <<= CAN_ADDR_BIT_POSITION;
    can_addr &= CAN_ADDR_MASK;

    int32_t res = check_input_file(file_name);

    if(res != 0)
    {
    	fclose(file_read);
    	printf("Exit\n");
    	return 0;
    }

    res = calc_CRC32_input_file();
    if(res != 0)
	{
		fclose(file_read);
		printf("calc_CRC32 error\n");
		printf("Exit\n");
		return 0;
	}

    sc = CAN_socker_init(can_interface);

    t_boot_state boot_state = ST_BOOT_CONNECT;

    uint32_t finish = 1;
    struct timespec ts;
    uint64_t time_start;

    can_protocol_make_connect_cmd(can_addr, &cframe);

    res = send_to_can(sc, &cframe);
	if(res != 0)
	{
		finish = 0;
	}else{

		clock_gettime(CLOCK_REALTIME, &ts);

		time_start = timespec_to_ms(ts);
	}

    while(finish)
    {
		int32_t res = wait_can_response(sc, &cframe);
		if(res == 0)
		{
			res = can_check_addr(&cframe);
			if(res == 0)
			{
				res = can_protocol(file_read, can_addr, &cframe);
				if(res == 1)
				{
					res = send_to_can(sc, &cframe);
					if(res != 0)
					{
						finish = 0;
					}else{

						clock_gettime(CLOCK_REALTIME, &ts);

						time_start = timespec_to_ms(ts);
					}
				}else if(res == 3)
				{
					// update successful
					// exit from program
					finish = 0;
				}
				else{
					uint32_t tout = check_timeout(time_start, 5000ULL);
					if(tout)
					{
						printf("reply timeout\n");
						finish = 0;
					}
				}
			}else{
				uint32_t tout = check_timeout(time_start, 5000ULL);
				if(tout)
				{
					printf("reply timeout 2\n");
					finish = 0;
				}
			}
		}else{
			printf("reply timeout 1\n");
			finish = 0;
		}
    }
	printf("close file\n");
    fclose(file_read);
    printf("close can\n");
    close(sc);
    printf("Exit\n");
    return 0;
}
