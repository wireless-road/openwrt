#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

//#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

void print_help()
{
    printf("usage: app [-a {0 | 1 } ] [-h] \n");
}

int main(int argc, char* argv[])
{
    int ret;
    rk_t rk_One, rk_Two;
    int addr=0, addr_c, opt;

    while ((opt = getopt(argc, argv, "ha:")) != -1)
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

    CAN_init(, &rk_One.can_bus);
    CAN_init(, &rk_Two.can_bus);



    exit(EXIT_SUCCESS);
}

