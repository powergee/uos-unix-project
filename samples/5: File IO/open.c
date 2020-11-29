#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	char *fname = "test.txt";
	int fd;
	
	fd = open(fname, O_RDONLY);
	if( fd >= 0 )
	{
		printf("Success\n FIleName: %s, fd: %d\n", fname, fd);
		close(fd);
	}
	else
		perror("open failed\n");


	return 0;
}

