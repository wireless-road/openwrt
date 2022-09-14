#include "libgs.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static void gs_thread(gs_conninfo_t* conninfo);

static int gs_read_reg(modbus_t *ctx, int reg, int count, uint16_t *buffer)
{
    int ret;
    usleep(5000);
    ret = modbus_read_input_registers(ctx, reg, count, buffer);
    if (ret == -1) {
//        fprintf(stderr, "Read reg %d failed: %s\n", reg, modbus_strerror(errno));
    }
    return ret;
}

int gs_get_version(modbus_t *ctx)
{
    int ret;
    int device_mode = 0;
    int sw_revision = 0;

    if (gs_read_reg(ctx, R_MMI_MBUS_MAP, 1, &device_mode) == -1)
    {
        return -1;
    }
    if (gs_read_reg(ctx, R_MMI_SW_VER, 1, &sw_revision) == -1)
    {
        return -1;
    }
    printf("Device mode: %s\n", device_mode ? "MMI" : "FLOMAC");
    printf("Software version: %d\n", sw_revision);
    return 0;
}
#ifdef METER_TYPE_FLOMAC
int gs_get_summator(int sum_num)
{
    int ret;
    uint16_t buf[2];
    int reg;
    float sum;
    switch (sum_num)
    {
    case 1:
        reg = REG_SUM1;
        break;
    case 2:
        reg = REG_SUM2;
        break;
    case 3:
        reg = REG_SUM3;
        break;
    case 4:
        reg = REG_SUM4;
        break;
    default:
        fprintf(stderr, "Invalid summator number %d", reg);
        return -1;
    }

    ret = gs_read_reg(reg, 2, buf);
    if (ret == -1)
    {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return ret;
    }
    sum = modbus_get_float_badc(buf);
    printf("Current summator %d value is: %f", sum_num, sum);
    return 0;
}
#endif

int gs_get_all_units(modbus_t *ctx, measure_units_t *measure_units)
{
    uint16_t *p;

    uint16_t buf[8];
    p = (uint16_t *)measure_units;
    if (gs_read_reg(ctx, R_MMI_UNITS_BASE, 8, &buf) == -1)
        return -1;
    for (int i = 0; i < 8; i++)
    {
        *p = buf[i];
        p++;
    }

    return 0;
}

int gs_get_all_measurements(modbus_t *ctx, measurements_t *measurements)
{
    float *p;
    p = (float *)measurements;
    uint16_t buf[20];
    if (gs_read_reg(ctx, R_MMI_MEASUREMENTS_BASE, 20, &buf) == -1)
        return -1;
    for (int i = 0; i < 20; i += 2)
    {
        *p = modbus_get_float_badc(&buf[i]);
        p++;
    }
    return 0;
}

int gs_set_mass_units(modbus_t *ctx, int mass_units)
{
    uint8_t ret;
    uint16_t readback_value;
    ret = modbus_write_and_read_registers(ctx, R_MMI_MASS_U, 1, &mass_units,
                                          R_MMI_MASS_U, 1, &readback_value);
    if ((mass_units == readback_value) || !ret)
        return 0;
    else
        return -1;
}
int gs_set_volume_units(modbus_t *ctx, int volume_units)
{
    uint8_t ret;
    uint16_t readback_value;
    ret = modbus_write_and_read_registers(ctx, R_MMI_VOLUME_U, 1, &volume_units,
                                          R_MMI_VOLUME_U, 1, &readback_value);
    if ((volume_units == readback_value) || !ret)
        return 0;
    else
        return -1;
}


int gs_start_count(modbus_t *ctx){
    return modbus_write_bit(ctx, C_MMI_STARTSTOP, 1);
}
int gs_stop_cunt(modbus_t *ctx){
    return modbus_write_bit(ctx, C_MMI_STARTSTOP, 0);
}


int gs_reset_total_counters(modbus_t *ctx){
    int ret;
    if(modbus_write_bit(ctx, C_MMI_RESET_TOTALS, 1) == -1){
        //return -1;
    }
    if(modbus_write_bit(ctx, C_MMI_RESET_TOTALS, 0) == -1){
        return -2;
    }
    //if(modbus_write_bit(ctx, C_MMI_RESET_INVENT, 1) == -1){
    //    //return -3;
    //}
    //if(modbus_write_bit(ctx, C_MMI_RESET_INVENT, 0) == -1){
    //    return -4;
    //}
    return 0;
}

int gs_init_pthreaded(int idx, gs_conninfo_t *conninfo)
{
    char filename[FILENAME_MAX_SIZE];

    // modbus_port
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_MODBUS_PORT, idx);

    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    conninfo->port = ret;

    // modbus_address
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_MODBUS_DEVICE_ADDRESS, idx);

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    conninfo->devaddr = ret;

    conninfo->baudrate = DEF_BAUDRATE;
    conninfo->connection_lost_flag = 0;

    pthread_create(&conninfo->thread_id, NULL, gs_thread, conninfo);
}

static void print_measts(measurements_t *ms)
{
	printf("Meas: MFR: %.2f. DEN: %.2f. TEMP: %.2f. \
VFR: %.2f. PRESS: %.2f. M_TOTAL: %.2f. \
V_TOTAL: %.2f. M_INV: %.2f. V_INV: %.2f\r\n",
				ms->mass_flowrate,
				ms->density,
				ms->temprature,
				ms->volme_flowrate,
				ms->pressure,
				ms->mass_total,
				ms->volume_total,
				ms->mass_inventory,
				ms->volume_inventory);

}


static void gs_thread(gs_conninfo_t* conninfo) {
    int ret;
    conninfo->ctx = gs_init(conninfo);
    ret = gs_get_version(conninfo->ctx);
    printf("ret0 %d\r\n", ret);
    
    if(ret == -1) {
        conninfo->connection_lost_flag = -1;
    }
    
    ret = gs_get_all_units(conninfo->ctx, &conninfo->measure_units);
    printf("ret1 %d\r\n", ret);
    ret = gs_reset_total_counters(conninfo->ctx);
    printf("ret2 %d\r\n", ret);
    gs_start_count(conninfo->ctx);
 
    while(1) {
        ret = gs_get_all_measurements(conninfo->ctx, &conninfo->measurements);
    	if(ret == -1) {
	    printf("rett %d\r\n", ret);
            conninfo->connection_lost_flag = -1;
        } else {
            conninfo->connection_lost_flag = 0;
            print_measts(&conninfo->measurements);
        }
        usleep(300000);
    }
}

int gs_check_state(gs_conninfo_t* conninfo) {
    return conninfo->connection_lost_flag;
}

modbus_t *gs_init(gs_conninfo_t *conninfo)
{
    modbus_t *ctx;
    int ret;
    char port[16];

    sprintf(&port, "/dev/ttymxc%d", conninfo->port);
//    printf("Connecting to %s...\n", port);
    ctx = modbus_new_rtu(port, conninfo->baudrate, 'N', 8, 1);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return ctx;
    }
    modbus_set_slave(ctx, conninfo->devaddr);
    modbus_set_response_timeout(ctx, 0, 100000);
    // modbus_set_debug(ctx, TRUE);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
    ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    if (!ret)
    {
        fprintf(stderr, "Error setting mode to RS485!\n");
        modbus_free(ctx);
        return NULL;
    }

    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }
    return ctx;
}

int gs_scan(gs_conninfo_t *conninfo)
{
    modbus_t *ctx;
    int ret;
    char port[16];

    sprintf(&port, "/dev/ttymxc%d", conninfo->port);
    printf("Connecting to %s...\n", port);
    ctx = modbus_new_rtu(port, conninfo->baudrate, 'N', 8, 1);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return ctx;
    }

    modbus_set_response_timeout(ctx, 0, 100000);
    // modbus_set_debug(ctx, TRUE);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
    ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    if (!ret)
    {
        fprintf(stderr, "Error setting mode to RS485!\n");
        modbus_free(ctx);
        return NULL;
    }

    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return NULL;
    }

    for(int i=0; i<247; i++) {
        modbus_set_slave(ctx, i);
        ret = gs_get_version(ctx);
        printf("Slave address %d responce: %d\r\n", i, ret);
        if(!ret) {
            // slave device responded
            return i;
        }
    }

    return -1;
}

void gs_close(modbus_t *ctx)
{
    modbus_close(ctx); // does we need it with RS485 connection?
    modbus_free(ctx);
}
