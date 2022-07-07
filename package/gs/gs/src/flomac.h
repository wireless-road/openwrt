#ifndef FLOMAC_H
#define FLOMAC_H
/*
 *   =====================
 *   FLOMAC mode registers
 *   =====================
 */

/* Main params registres */
#define REG_F_MASS_FLOW 300
#define REG_F_VOLUME_VLOW 302
#define REG_F_DENSITY 304
#define REG_F_TEMPERATURE 306
#define REG_F_REF_DENSITY 308
#define REG_F_CORR_VOL_FLOW 310
#define REG_F_PRESSURE 312
#define REG_F_DRIVEGAIN 323

/* summators */
#define REG_F_SUM1 708
#define REG_F_SUM2 807
#define REG_F_SUM3 757
#define REG_F_SUM4 857

/* MODBUS Config registres */
#define REG_F_DEVADDR 430
#define REG_F_BAUDRATE 431
#define REG_F_PARITY 432
#define REG_F_HW_WP 433
#define REG_F_MBUS_MODE 436
#define REG_F_FLOAT_BYTEORDER 437

/* System registers */
#define REG_F_HWVER 400
#define REG_F_HWMOD 401
#define REG_F_SWVER 402
#define REG_F_RDAY 403
#define REG_F_RMONTH 404
#define REG_F_RYEAR 405

#endif
