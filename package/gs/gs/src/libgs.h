#include <stdio.h>
#include <errno.h>
#include <modbus/modbus.h>

#define DEF_ADDR 1

/* Main params registres */
#define REG_MASS_FLOW 300
#define REG_VOLUME_VLOW 302
#define REG_DENSITY 304
#define REG_TEMPERATURE 306
#define REG_REF_DENSITY 308
#define REG_CORR_VOL_FLOW 310
#define REG_PRESSURE 312
#define REG_DRIVEGAIN 323


/* summators */
#define REG_SUM1 708
#define REG_SUM2 807
#define REG_SUM3 757
#define REG_SUM4 857

/* MODBUS Config registres */
#define REG_DEVADDR 430
#define REG_BAUDRATE 431
#define REG_PARITY 432
#define REG_HW_WP 433
#define REG_MBUS_MODE 436
#define REG_FLOAT_BYTEORDER 437

/* System registers */
#define REG_HWVER 400
#define REG_HWMOD 401
#define REG_SWVER 402
#define REG_RDAY 403
#define REG_RMONTH 404
#define REG_RYEAR 405


int gs_init(char* port);
void gs_close(void);
int gs_get_current_values(void);
int gs_get_summator(int sum_num);