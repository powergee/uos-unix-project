#include <stdio.h>
#include <unistd.h>

int main(void)
{
     char buf[BUFSIZ];

     setvbuf(stdout, buf, _IOLBF, BUFSIZ);
     printf("Hello,"); sleep(1);
     printf("Unix!"); sleep(1);
     printf("\n"); sleep(5);

     setbuf(stdout, NULL);
     sleep(1);
     printf("How "); sleep(1);
     printf("are "); sleep(1);
     printf("you?"); sleep(1);
     printf("\n"); sleep(1);

     return 0;
}

