#include <stdio.h>
#include <unistd.h>

int main(void)
{
	fprintf(stdout, "Good Bye ...");

	sleep(5);

	fflush(stdout);

	sleep(5);

	return 0;
}
