#include <fcntl.h>
#include <stdio.h>

int main(void)
{
	char *fname = "test.txt";
	int fd;
	FILE *fp;

	if( (fd = open(fname, O_WRONLY)) > 0 )
		printf("Success!!\n File descriptor: %d\n", fd);
	else
		printf("failed\n");
	
	if( (fp = fdopen(fd, "w")) != NULL )
		printf("fdopen() Success!!\n");
	else
		printf("fdopen() failed\n");

	fprintf(fp, "I'm your father.\n");

	fclose(fp);
}
