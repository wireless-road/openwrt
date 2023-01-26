#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include "can.h"

void print_help()
{
    printf("usage: app [-a {0 | 1 } ] [-h] \n");
}

struct triplet {
    const float vol;
    const float pr;
    const float totPr;
};

int main(int argc, char* argv[])
{
    int ret;
    int addr=0, addr_c, opt;
    int delay = 1000, delay_c;


    while ((opt = getopt(argc, argv, "ha:d:")) != -1)
    {
        switch (opt) {
               case 'h':
                    print_help();
                    exit(EXIT_SUCCESS);
               case 'a':
                   addr_c = atoi(optarg);
                    if(addr_c == 0 || addr_c == 1)
                        addr = addr_c;
                   break;
                case 'd':
                    delay_c = atoi(optarg);
                    if( delay_c > 20 && delay_c < 30000)
                        delay = delay_c;
                    break;
               default: /* '?' */
                   fprintf(stderr, "Usage: %s [-a {0 |1}] [-h]\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
               }
    }

    int addr_base;
    switch(addr)
    {
        case 1:
            addr_base = 0x40;
            break;
        default:
            addr_base = 0x20;

    };

    can_t canNet;

    if( CAN_init_ex(&canNet, 0, addr_base) != 0)
    {
        fprintf(stderr, "failed to execute CAN_init_ex for %d\n", addr_base);
        exit(EXIT_FAILURE);
    }

struct triplet _records[] = 
{
    {000000.00f, 0000.00f, 000000.00f},
    {111111.11f, 1111.11f, 111111.11f},
    {222222.22f, 2222.22f, 222222.22f},
    {333333.33f, 3333.33f, 333333.33f},
    {444444.44f, 4444.44f, 444444.44f},
    {555555.55f, 5555.55f, 555555.55f},
    {666666.66f, 6666.66f, 666666.66f},
    {777777.77f, 7777.77f, 777777.77f},
    {888888.88f, 8888.88f, 888888.88f},
    {999999.99f, 9999.99f, 999999.99f},
    {000000.00f, 0000.00f, 000000.00f},
};
    int nxt = 0;
    while(1)
    {
        canNet.transmit(&canNet, _records[nxt].vol, _records[nxt].pr, _records[nxt].totPr);
        usleep(1000 * delay);
        ++nxt; nxt %= (sizeof(_records)/sizeof(struct triplet));
    }
    
    close(canNet.fd);
    exit(EXIT_SUCCESS);
}

