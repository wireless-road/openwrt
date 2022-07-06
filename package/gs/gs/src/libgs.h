#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>

#include "mmi.h"
#include "flomac.h"

#define DEF_ADDR 2

int gs_init(char* port);
void gs_close(void);
int gs_get_version(void);
int gs_get_current_values(void);
int gs_get_total_values(void);
// int gs_get_summator(int sum_num);
