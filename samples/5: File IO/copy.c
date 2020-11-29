#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd1, fd2, n;
	char  buf[100];
	
	if (argc < 3) {
		printf(" Usage:  copy   oldfile   newfile \n");    exit(0);
	}
	if( (fd1= open(argv[1], O_RDONLY)) < 0 ) {
		perror("open() ");        exit(-1);
	}
	fd2 = open(argv[2], O_WRONLY|O_CREAT | O_TRUNC, 0666);
	if (fd2 < 0) {
		perror("open() ");        exit(-1);
	}
	while ((n = read(fd1, buf, 100)) > 0 )
		write(fd2, buf, n);
	close(fd1);
	close(fd2);
	return 0;
}
