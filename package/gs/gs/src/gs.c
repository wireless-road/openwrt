#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

//#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

/*
* Example button handler
* Define won handlers for all keycodes from button.c
*/
void button_handler(int code){
	printf("Handled button: 0x%04x (%d)\n", (int)code, (int)code);
}

int main(int argc, char* argv[])
{
	int ret = azt_init();
	if (ret == -1) {
		return 1;
	}

    rk_t left_rk, right_rk;
    rk_init(1, &left_rk);
    rk_init(2, &right_rk);
	buttons_init(&left_rk,
                 &right_rk);

    // simulation of fueling process
//    left_rk.state = trk_enabled_fueling_process;
//    left_rk.fueling_price_per_liter = 30.0;
//    left_rk.fueling_dose_in_liters = 3;

    int azt_req_flag = 0;
	while(1) {
		buttons_handler(&left_rk, &right_rk);
        azt_req_flag = azt_rx_handler();
        if(azt_req_flag) {
            left_rk.azt_req_hndl(azt_request(), &left_rk);
            right_rk.azt_req_hndl(azt_request(), &right_rk);
        }

        if(left_rk.is_not_fault(&left_rk)) {
            left_rk.process(&left_rk);
        } else {
        }

        if(right_rk.is_not_fault(&right_rk)) {
            right_rk.process(&right_rk);
        } else {
        }

    }
    azt_deinit();

	return 0;

}
