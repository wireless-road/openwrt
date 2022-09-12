#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

// 1st inst
gs_conninfo_t gs1_conninfo =
{
	.port = 4,
	.baudrate = 9600,
	.devaddr = DEF_ADDR
};
measure_units_t units_s1;
measurements_t measurements_s1;

// 2nd inst
gs_conninfo_t gs2_conninfo =
{
	.port = 5,
	.baudrate = 9600,
	.devaddr = DEF_ADDR
};
measure_units_t units_s2;
measurements_t measurements_s2;

/* begin testing code */
void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}
void print_units(measure_units_t *mu)
{
	printf("Units: %d %d %d %d %d %d %d\n",
            mu->massflow_unit,
            mu->density_unit,
            mu->temperature_unit,
            mu->volumeflow_unit,
            mu->pressure_unit,
            mu->mass_unit,
            mu->volume_unit);
}

void print_measurements(measurements_t *ms)
{
	printf("Measurements:\nMFR: %.2f\nDEN: %.2f\nTEMP: %.2f\n\
VFR: %.2f\nPRESS: %.2f\nMASS_TOTAL: %.2f\n\
VOL_TOTAL: %.2f\nMASS_INV: %.2f\nVOL_INV: %.2f\n",
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

void test(modbus_t *ctx)
{
	int i;
	gs_get_version(ctx);
		gs_get_all_units(ctx, &units_s1);
		print_units(&units_s1);
		usleep(3000000);
		// test loop
		while (TRUE) {
			gs_reset_total_counters(ctx);
			gs_start_count(ctx);

			while (i < 1000){
				clearScreen();
				gs_get_all_measurements(ctx, &measurements_s1);
				print_measurements(&measurements_s1);
				usleep(50000);
			}
		}
}
/* end testing code */

/*
* Example button handler
* Define won handlers for all keycodes from button.c
*/
void button_handler(int code){
	printf("Handled button: 0x%04x (%d)\n", (int)code, (int)code);
}

int main(int argc, char* argv[])
{
	int buttons_fd;
	int ret = azt_init();
	if (ret == -1) {
		return 1;
	}

    rk_t left_rk, right_rk;
    rk_init(1, &left_rk);
    rk_init(2, &right_rk);
	buttons_fd = buttons_init();

    // simulation of fueling process
//    left_rk.state = trk_enabled_fueling_process;
//    left_rk.fueling_price_per_liter = 30.0;
//    left_rk.fueling_dose_in_liters = 3;

    int azt_req_flag = 0;
	while(1) {
		buttons_handler(&buttons_fd);
        azt_req_flag = azt_rx_handler();
        if(azt_req_flag) {
            left_rk.azt_req_hndl(azt_request(), &left_rk);
            right_rk.azt_req_hndl(azt_request(), &right_rk);
        }

        if(left_rk.is_not_fault(&left_rk)) {
            left_rk.process(&left_rk);
        } else {
//            printf("errorL\r\n");
        }

        if(right_rk.is_not_fault(&right_rk)) {
            right_rk.process(&right_rk);
        } else {
//            printf("errorR\r\n");
        }

    }
    azt_deinit();

	return 0;

	modbus_t *ctx;
	ctx = gs_init(&gs2_conninfo);
	if (ctx != NULL)
	{
		test(ctx);
		gs_close(ctx);
	}
	return 0;
}
