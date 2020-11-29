#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int    ret;

	ret = truncate ("./test.txt", 16384);	
	if (ret == -1) {
		perror("truncate");
		return -1;
	}
	return 0;
}

