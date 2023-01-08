#ifndef ERROR_H
#define ERROR_H

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

#include "error.h"

typedef struct error_t error_t;

struct error_t {
    int32_t code;
    float message;
};

typedef enum {
    ERROR_INPUT_4_20_NOT_CONNECTED = 0, // обрыв цепи 4-20мА // 90001
    ERROR_MODBUS_NOT_CONNECTED = 1,  // массомер не на связи  // 90002
	ERROR_INPUT_4_20_LOW_PRESSURE = 2,  // низкое давление по 4-20мА // 90004
	ERROR_INPUT_4_20_HIGH_PRESSURE = 3,  // высокое давление по 4-20мА // 90008
	ERROR_INPUT_4_20_SWAPPED_WIRES = 4,  // перепутаны провода на 4-20мА  // 90016
	ERROR_UNKNOWN = 8
}errorCodes;

#define ERROR_SET(p,n) ((p) |= ((1) << (n)))
#define ERROR_CLEAR(p,n) ((p) &= ~((1) << (n)))
#define ERROR_IS_SET(p,n) ((p & (1<<n)) != 0)
#define ERROR_IS_CLEAR(p,n) ((p & (1<<n)) == 0)

#define ERROR_MESSAGE_DEFAULT_DISPLAY_STATE     90000.00

void error_init(error_t* storage);
void error_set(error_t* storage, errorCodes code);
void error_clear(error_t* storage, errorCodes code);
int error_is_set(error_t* storage, errorCodes code);
int error_is_clear(error_t* storage, errorCodes code);

#endif  // ERROR_H

