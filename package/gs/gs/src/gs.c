#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libgs.h"


/* jhust read all, print it and exit */
int main(int argc, char* argv[])
{
	int ret;
	// int i;
	char port[] = "/dev/ttymxc5";
	/* for now, hardcoded ttymxc2 , 115200 8n1 as device port */
	gs_init(port);
	gs_get_version();
	gs_get_current_values();
	gs_get_total_values();
	gs_close();
	return 0;
}
