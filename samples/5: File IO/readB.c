#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd, linecnt = 0;
    char c;
    char *fname = "test.txt";

    if ( (fd = open(fname, O_RDONLY)) < 0 ) {
	perror("open()");
	exit(-1);
    }
    while ( read(fd, &c, 1) > 0 ) {
	putchar(c);
	if( c == '\n' )
		linecnt++;
    }
    printf("Total line: %d\n", linecnt);
    read(0, &c, 1);
    return 0;
}
