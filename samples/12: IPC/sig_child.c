#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void sig(int sig) {
	printf("child die (%d)\n", getpid());
}

void main() {
	signal(SIGINT, sig);
	pause();
}
