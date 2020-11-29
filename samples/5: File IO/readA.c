#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd;
    char c;

    if ( (fd = open("test.txt", O_RDONLY)) < 0 )
    {
	perror("open");
	exit(0);
    }
    while (read(fd, &c, 1) > 0 ) {
	putchar(c);
    }
    read(0, &c, 1);
    return 0;
}
