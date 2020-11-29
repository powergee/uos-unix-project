#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_EVENTS 64
 
void main() {
	int ret, nr_events, i, epfd, fd1, fd2;
	struct epoll_event *revents;
	struct epoll_event event;

	epfd = epoll_create(100);
	if (epfd<0) perror("epoll_create");

        fd1 = open("np1", O_RDWR);
        if (fd1 < 0) exit(1);
        fd2 = open("np2", O_RDWR);
        if (fd2 < 0) exit(1);
        
	event.data.fd = fd1;
	event.events = EPOLLIN;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &event);
	if (ret<0) perror("epoll_ctl");
	
	event.data.fd = fd2;
	event.events = EPOLLIN;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &event);
	if (ret<0) perror("epoll_ctl");
	
	revents = malloc(sizeof(struct epoll_event) * MAX_EVENTS);
	if (!revents) perror ("malloc");

	nr_events = epoll_wait(epfd, revents, MAX_EVENTS, -1); // infinite time out
	if (nr_events<0) perror("epoll_wait");
	for (i=0; i<nr_events; i++){
		printf("event=%d on fd=%d\n",
			revents[i].events,
			revents[i].data.fd);
	}
	close (epfd);
 	return;
}
