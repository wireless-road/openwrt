#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpiod.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>

#include "mmi.h"
#include "flomac.h"

typedef struct {
    uint8_t port;
    uint16_t baudrate;
    uint8_t pinctrl;
    uint8_t rtspin;
} gs_conninfo_t;

#define DEF_ADDR 2

modbus_t* gs_init(gs_conninfo_t *conninfo);
void gs_close(modbus_t *ctx);
int gs_get_version(modbus_t *ctx);
int gs_get_current_values(modbus_t *ctx);
int gs_get_total_values(modbus_t *ctx);
// int gs_get_summator(int sum_num);
