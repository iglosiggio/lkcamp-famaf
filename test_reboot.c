#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {
	//int rval = reboot(LINUX_REBOOT_CMD_HALT);
	int rval = reboot(LINUX_REBOOT_CMD_POWER_OFF);
	printf("%i\n", rval);
	printf("%s\n", strerror(errno));
	return 1;
}
