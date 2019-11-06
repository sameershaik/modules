#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>


int main(void)
{
	int fd;
	char *message = "sameer is a good programmer";
	int len;
	int ret;
	
	len = strlen(message);
	fd = open("/dev/chardev_char", O_APPEND);

	if (fd == -1) {
		printf("unable to open the file\n");
		return -1;
	}
	ret = write(fd, message, len);

	if (ret < len) {
		printf("ret %d\t len%d\n", ret, len);
		printf("Unable to write the data\n");
		close(fd);
		return ret;
	}
	close(fd);
	return 0;
}
	
		
