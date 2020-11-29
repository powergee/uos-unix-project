#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_EVENTS 64
 
void main() {
	int ret, nr_events, i, epfd;
	struct epoll_event *revents;
	struct epoll_event event;

	epfd = epoll_create(100);
	if (epfd<0) perror("epoll_create");

	event.data.fd = 0; // monitoring stdin fd = 0
	event.events = EPOLLIN | EPOLLOUT;
	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
	if (ret<0) perror("epoll_ctl");

	revents = malloc(sizeof(struct epoll_event) * MAX_EVENTS);
	if (!revents) perror ("malloc");

	nr_events = epoll_wait(epfd, revents, MAX_EVENTS, -1);
						// infinite time out
	if (nr_events<0) perror("epoll_wait");
	for (i=0; i<nr_events; i++){
		printf("event=%d on fd=%d\n",
			revents[i].events,
			revents[i].data.fd);
	}
	close (epfd);
 	return;
}


