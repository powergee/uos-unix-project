# 프로젝트

제출방법: 12월 11일 23시 59분까지 이메일로 제출 `dhl.uos@icloud.com`

- 이메일에 텍스트 형태로 자신이 코딩한 프로그램과 설명을 기술할 것.
- 프로그램 수행 결과는 그림 파일(GIF, JPG 등) 로 첨부 가능.
- 절대로 hwp, doc, page 등 워드 문서나 파일을 첨부하지 말 것. 즉시 반려하며, 미 제출로 처리.
- 완벽하지 않더라도 반드시 아래 설명에 따라 자신이 직접 구현할 것,

인터넷에 있는 다른 형태를 복사해서 제출하면 감점.

---

게시판에서 다운로드 받은 mini_sh.c를 아래와 같이 확장하라.
1) “>”, “<“와 같은 Redirection 기능을 추가하라.
2) “|”와 같은 파이프 기능을 추가하라.

---

``` C
/* 리디렉션 ">" 처리

  예) # ls > abc.txt

      쉘은 fork로 child 프로세스를 생성한다.

      child가 fork() 이후 "abc.txt" 파일을 읽기/쓰기 모드로 open한다. 그리고 이 파일의 파일 디스크립터를 1(STDOUT_FILENO)번 파일 디스크립터로 복사한다.

      그 후 child는 ls로 exec한다.

*/

/* 아래 프로그램에서 cmd[1]은 "ls" 문자열을 가리킨다. cmd[2]는 ">"를, 그리고 cmd[3]는 "abc.txt"를 가리킨다. */

int pid1;
int fd;

pid1 = fork();

if (pid1 == 0) {  /* child */
   fd = open (cmd[3], O_RDWR | O_CREAT | S_IROTH, 0644);
   if (fd < 0) {
        perror ("error");
        exit(-1);
   }

   dup2(fd, STDOUT_FILENO);
   close(fd);
   exec(cmd[1], ...);
   exit(0);
}

/* Parent: Shell */
wait();
```

---

``` C
/* 리디렉션 "<" 처리

  예) # ls < abc.txt

      쉘은 fork로 child 프로세스를 생성한다.

      child가 fork() 이후 "abc.txt" 파일을 읽기 모드로 open한다. 그리고 이 파일의 파일 디스크립터를 0(STDIN_FILENO)번 파일 디스크립터로 복사한다.

      그 후 child는 ls로 exec한다.

*/

/* 아래 프로그램에서 cmd[1]은 "ls" 문자열을 가리킨다. cmd[2]는 "<"를, 그리고 cmd[3]는 "abc.txt"를 가리킨다. */

int pid1;
int fd;

pid1 = fork();

if (pid1 == 0) {  /* child */
   fd = open (cmd[3], O_RDONLY);
   if (fd < 0) {
        perror ("error");
        exit(-1);
   }

   dup2(fd, STDIN_FILENO);
   close(fd);
   exec(cmd[1], ...);
   exit(0);
}

/* Parent: Shell */
wait();
```

---

``` C
/* 파이프 "|" 처리

  예) # ls | more

      쉘은 fork로 child1 프로세스를 생성한다. child1은 이후 "ls"로 exec할 예정이다.
      child1은 파이프 fd를 생성한다.
      chile1은 fork로 child2 프로세스를 생성한다. child2는 이후 "more"로 exec할 예정이다.
           {
                child2은 fd[0]을 STDIN_FILENO로 복사한다.
                child2은 fd[0]와 fd[1]을 닫는다.
                child2은 more로 exec한다.
           }

      child1은 fd[1]을 STDOUT_FILENO로 복사한다.
      child1은 fd[0]와 fd[1]을 닫는다.
      child1은 ls로 exec한다.
*/

/* 아래 프로그램에서 cmd[1]은 "ls" 문자열을 가리킨다. cmd[2]는 "|"를, 그리고 cmd[3]는 "more"를 가리킨다. */

int p[2];
int pid1, pid2;

pid1 = fork();

if (pid1 == 0) {  /* First child */
    pipe(p);
    pid2 = fork();

    if (pid2 == 0) {  /* Second child */
         dup2(p[0], STDIN_FILENO);
         close (p[0]);
         close (p[1]);
         exec(cmd3, ...);
   }

   dup2(p[1], STDOUT_FILENO);
   close (p[0]);
   close (p[1]);
   exec(cmd[1], ...);
}



/* Parent : Shell */
wait();
```