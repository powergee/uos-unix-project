#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{	
	if( truncate("test.txt", 10) < 0 ) {
		perror("truncate error");
		exit(-1);
	}

	return 0;
}
