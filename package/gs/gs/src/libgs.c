#include "libgs.h"

modbus_t *ctx;

static int gs_read_reg(int reg, int count, uint16_t *buffer)
{
    int ret;
    usleep(100000);
    ret = modbus_read_input_registers(ctx, reg, count, buffer);
    if (ret == -1)
    {
        fprintf(stderr, "Read reg %d failed: %s\n", reg, modbus_strerror(errno));
        return -1;
    }
}

int get_gs_protocol(void)
{
    int ret;
    uint16_t mode;
    ret = gs_read_reg(REG_F_MBUS_MODE, 1, &mode);
    if (mode)
        printf("MODE: MMI\n");
    else
        printf("MODE: FLOMAC\n");
}
int gs_get_version(void)
{
    int ret;
    int i;
    int device_mode;
    int sw_revision;

    if (gs_read_reg(R_MMI_MBUS_MAP, 1, &device_mode) == -1)
    {
        return -1;
    }
    if (gs_read_reg(R_MMI_SW_VER, 1, &sw_revision) == -1)
    {
        return -1;
    }
    printf("Device mode: %s\n", device_mode ? "MMI" : "FLOMAC");
    printf("Software varsion: %d\n", sw_revision);
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

int gs_get_total_values(void)
{
    int ret;
    float mass_total;
    float volume_total;

    uint16_t buf[2];
    ret = gs_read_reg(R_MMI_MASS_TOTAL, 2, buf);
    if (!ret)
        mass_total = modbus_get_float_badc(buf);

    ret = gs_read_reg(R_MMI_VOL_TOTLAL, 2, buf);
    if (!ret)
        volume_total = modbus_get_float_badc(buf);

    printf("TOTAL MASS: %f\nTOTAL VOLUME: %f\n", mass_total, volume_total);
    return 0;
}

int gs_get_current_values(void)
{
    int ret;
    float mass_flow;
    float volume_flow;
    /* add more if needed, just test */

    uint16_t buf[2];
    ret = gs_read_reg(R_MMI_MASS_FRATE, 2, buf);
    mass_flow = modbus_get_float_badc(buf);
    ret = gs_read_reg(R_MMI_VOLUME_FRATE, 2, buf);
    volume_flow = modbus_get_float_badc(buf);
    printf("Current measurements: MASS: %f, VOLUME: %f\n", mass_flow, volume_flow);
    return 0;
}

int rts_gpio_fd = -1;

void rts_gpio_init()
{
    rts_gpio_fd = open("/sys/class/gpio/gpio128/value", O_WRONLY | O_SYNC);
}

void csetrts(modbus_t *ctx, int on)
{
    if (on)
    {
        write(rts_gpio_fd, "1", 1);
    }
    else
    {
        write(rts_gpio_fd, "0", 1);
    }
}

int gs_init(char *port)
{
    rts_gpio_init();
    int ret;
    ctx = modbus_new_rtu(port, 9600, 'N', 8, 1);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }
    modbus_set_slave(ctx, DEF_ADDR);
    modbus_set_response_timeout(ctx, 0, 100000);
    modbus_rtu_set_custom_rts(ctx, csetrts);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
    ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    if (!ret)
    {
        fprintf(stderr, "Error setting mode to RS485!");
        return ret;
    }

    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
    return 0;
}

void gs_close(void)
{
    modbus_close(ctx); // does we need it woth RS485 connection?
    modbus_free(ctx);
}
