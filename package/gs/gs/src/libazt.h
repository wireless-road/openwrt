
#ifndef SRC_LIBAZT_H
#define SRC_LIBAZT_H

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "libazt_ll.h"

#define TRK_DEFAULT_ADDRESS 0x01

#define COMPLIMENTARY(a)(0x7F & (~a))

#ifdef AZTLIB

#define AZT_RQST_DEL_SYMBOL             0x7F
#define AZT_RQST_ENDING_SYMBOL          0x03

#define AZT_REQUEST_PARAMS_MAX_AMOUNT   10

#define AZT_STARTING_SYMBOLS_AMOUNT     15
const char AZT_STARTING_SYMBOLS[] = {0x02, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
                                     0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14 };
const int AZT_ADDRESS_SHIFTS[] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135,
                                  150, 165, 180, 195, 210 };


#define AZT_RELATED_ADDRESS_BASE        0x20
#define AZT_RELATED_ADDRESSES_AMOUNT    15
const int AZT_RELATED_ADDRESS[] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                   0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F };

#endif  // AZTLIB

int azt_init(void);
void azt_handler(void);
void azt_deinit();

#endif //SRC_LIBAZT_H