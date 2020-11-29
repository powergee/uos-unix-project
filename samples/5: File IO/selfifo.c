#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define TIMEOUT 5
#define BUF_LEN 1024

int main(void)
{
    struct  timeval  tv;
    fd_set  readfds;
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


    FD_ZERO(&readfds);
    FD_SET(fd1, &readfds);
    FD_SET(fd2, &readfds);

    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    ret = select(fd2+1, &readfds,
                         NULL, NULL, &tv);  
    if (ret == -1) return 1; /* select error */
    else if (!ret) return 0; /*timeout */
    
    if (FD_ISSET(fd1, &readfds)) {
        printf("pipe file 1 data read\n");
        return 0;
    }
    if (FD_ISSET(fd2, &readfds)) {
          printf("pipe file 2 data read\n");
          return 0;
    }
    fprintf(stderr, "This should not happen!\n");
    return 1;
}



