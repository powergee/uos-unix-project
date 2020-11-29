#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFSIZE  4096

int main(void)
{
	int fd, linecnt = 0, cnt,  i;
	char buf[BUFSIZE];
	char *fname = "test.txt";
	
	if( (fd = open(fname, O_RDONLY)) < 0 )
	{
		perror("open()");
		exit(-1);
	}
	while ( (cnt  = read(fd, buf, BUFSIZE)) > 0 ) {
		for (i = 0; i < cnt; i++)  {
		      if( buf[i]=='\n') linecnt++;
		}
	}
	printf("Total line: %d\n", linecnt);
        close(fd);
	return 0;
}
