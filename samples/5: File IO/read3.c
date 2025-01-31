#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)  {
	int  fd, nr;
	char  c,  *fname = "/dev/tty";
	
	if( (fd = open(fname, O_RDONLY | O_NONBLOCK)) < 0 )   {
		perror("open()");
		exit(-1);
	}
	for( ; ; ) {
		if ((nr =  read(fd, &c, 1)) > 0 ) {
			printf("%c\n", c); 
			exit(0);
		}
		else if (nr == -1) {
		 	if (errno == EINTR) printf(".");
			if (errno == EAGAIN) printf("*");
		}
		else  break;
	}
                        close(fd);
}

