#include <stdio.h>
#include <unistd.h>

int main(void)
{
	pid_t pid;

	printf("before fork()\n");
	if( ( pid = fork() ) > 0 ) {
		printf("parent process\n");
		sleep(2);
	}
	else if (pid == 0 ) {
	        printf("child process: before exec()\n");
		execlp("vi", "vi", "asdf.txt", (char*)0);
		printf("child process: after exec()\n");
	}
	return 0;
}
