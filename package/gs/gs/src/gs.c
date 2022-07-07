#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libgs.h"


gs_conninfo_t gs1_conninfo =
{
	.port = 4,
	.baudrate = 9600,
	.pinctrl = 4,
	.rtspin = 8
};

gs_conninfo_t gs2_conninfo =
{
	.port = 5,
	.baudrate = 9600,
	.pinctrl = 4,
	.rtspin = 0
};

/* jhust read all, print it and exit */
int main(int argc, char* argv[])
{
	int ret;
	modbus_t *ctx;
	ctx = gs_init(&gs1_conninfo);
	if (ctx != NULL)
	{
		gs_get_version(ctx);
		gs_get_current_values(ctx);
		gs_get_total_values(ctx);
		gs_close(ctx);
	}
	return 0;
}
