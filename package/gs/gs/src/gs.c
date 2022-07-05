#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "libgs.h"


/* jhust read all, print it and exit */
int main(int argc, char *argv[])
{
	int ret;
	int i;
	char port[] = "/dev/ttymxc2";
	/* for now, hardcoded ttymxc2 , 115200 8n1 as device port */
	ret = gs_init(port);
	ret = gs_get_current_values();
	for (i=1; i<5; i++)
	{
		ret = gs_get_summator(i);
	}
	gs_close();
	if (!ret) return ret;
	return 0;
}
