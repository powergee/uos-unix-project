#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define TIMEOUT 5
#define BUF_LEN 3

int main(void)
{
    struct  timeval  tv;
    fd_set  readfds;
    int  ret, mfd;

    mfd = open("./np1", O_RDONLY);
    if (mfd < 0) {
            printf("mouse device open fail\n");
            return 1;
    }

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(mfd, &readfds);

    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;
    
        ret = select(mfd+1, &readfds,
                         NULL, NULL, &tv);  
    if (ret == -1) return 1; /* select error */
    else if (!ret) return 0; /*timeout */
    
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
        printf("kbd event occurred\n");
          return 0;
    }
    if (FD_ISSET(mfd, &readfds)) {
          printf("mouse event occurred\n");
          return 0;
    }
    fprintf(stderr, "This should not happen!\n");
    return 1;
}
    
