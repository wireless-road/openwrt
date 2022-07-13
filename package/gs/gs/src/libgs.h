#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpiod.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>

#include "mmi.h"
#include "flomac.h"

#define DEF_ADDR 2
typedef struct gs_conninfo {
    uint8_t port;
    uint16_t baudrate;
    uint8_t devaddr;
} gs_conninfo_t;

typedef struct measure_units {
    uint16_t massflow_unit;
    uint16_t density_unit;
    uint16_t temperature_unit;
    uint16_t volumeflow_unit;
    uint16_t dummy00;
    uint16_t pressure_unit;
    uint16_t mass_unit;
    uint16_t volume_unit;
} measure_units_t;


typedef struct measurements {
    float mass_flowrate;
    float density;
    float temprature;
    float volme_flowrate;
    float dummy01;
    float pressure;
    float mass_total;
    float volume_total;
    float mass_inventory;
    float volume_inventory;
} measurements_t;

/* port handling */
modbus_t* gs_init(gs_conninfo_t *conninfo);
void gs_close(modbus_t *ctx);


/*common functions */
int gs_get_version(modbus_t *ctx);

int gs_get_all_units(modbus_t *ctx, measure_units_t *measure_units);

int gs_get_all_measurements(modbus_t *ctx, measurements_t *measurements);

int gs_set_mass_units(modbus_t *ctx, int mass_units);
int gs_set_volume_units(modbus_t *cts, int volume_units);

int gs_start_count(modbus_t *ctx);
int gs_stop_cunt(modbus_t *ctx);

/* Reset session counters */
int gs_reset_total_counters(modbus_t *ctx);

/* Reset total counters */
int gs_reset_inventory_counters(modbus_t *ctx); // Reset total counters
