#include <stdio.h>
#include <unistd.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>

#define TIMEOUT 20

int main(void)
{
    struct  pollfd  fds[2];
    int  ret, fd1, fd2;

    fd1 = open ("./np1", O_RDONLY);
    if (fd1 < 0) {
            printf("pipe file 1 open fail\n");
            return 1;
    }
    fd2 = open("./np2", O_RDONLY);
    if (fd2 < 0) {
            printf("pipe file 2 open fail\n");
            return 1;
    }
    
    fds[0].fd = fd1;
    fds[0].events = POLLIN;

    fds[1].fd = fd2;
    fds[1].events = POLLIN;

    ret = poll (fds, 2, TIMEOUT *1000);
    
    if (ret == -1) return 1; /* poll error */
    else if (!ret) return 0; /*timeout */
    
    if (fds[0].revents & POLLIN) {
        printf("np1 is readable\n");
    }
    if (fds[1].revents & POLLIN) {
        printf("np2 is readable\n");
    }
    return 0;
}

