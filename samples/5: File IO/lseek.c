#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	char *fname = "test.txt";
	int fd;
	off_t fsize;

	if( (fd = open(fname, O_RDONLY)) < 0 )
	{
		perror("open()");
		exit(-1);
	}
	fsize = lseek(fd, 0, SEEK_END);
	printf("The size of <%s> is %lu bytes.\n", fname, fsize);
	return 0;
}
