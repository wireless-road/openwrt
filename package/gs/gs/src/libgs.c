#include "libgs.h"


modbus_t *ctx;


static int gs_read_reg(int reg, int count, uint16_t* buffer)
{   
    int ret;
    ret = modbus_read_registers(ctx, reg, count, buffer);
    if (ret == -1)
    {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }
}


int gs_get_version(void)
{
    int ret;
    int i;
    uint16_t version[6];

    ret = gs_read_reg(REG_HWVER, 6, version);
    if (ret == -1)
    {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return ret;
    }
    
    for (i=0; i < ret; i++) {
        printf("VER[%d]=%d (0x%X)\n", i, version[i], version[i]);
    }
    return 0;

}

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

int gs_get_current_values(void)
{
    int ret;
    float mass_flow;
    float volume_flow;
    /* add more if needed, just test */

    uint16_t buf[2];
    ret = gs_read_reg(REG_MASS_FLOW, 2, buf);
    mass_flow = modbus_get_float_badc(buf);
    ret = gs_read_reg(REG_VOLUME_VLOW, 2, buf);
    volume_flow = modbus_get_float_badc(buf);
    printf ("Current measurements: MASS: %f, VOLUME: %f", mass_flow, volume_flow);
    return 0;
}

int gs_init(char* port)
{   
    ctx = modbus_new_rtu(port, 115200, 'N', 8, 1);
    if (ctx == NULL) {
    fprintf(stderr, "Unable to create the libmodbus context\n");
    return -1;
}
    modbus_set_slave(ctx, DEF_ADDR);
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    gs_get_version();
    return 0;
}

void gs_close(void)
{
    modbus_close(ctx); // does we need it woth RS485 connection?
    modbus_free(ctx);
}