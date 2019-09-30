#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

static void connect_alarm(int);

int connect_timeo(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec){

	int n;

	printf("nsec: %d\n", nsec);

	signal(SIGALRM, connect_alarm);

	if(alarm(nsec) != 0){
		printf("alarm was already set\n");
		return -1;
	}

	if((n = connect(sockfd, saptr, salen))<0){
		close(sockfd);
		if(errno == EINTR){
			errno = ETIMEDOUT;
			printf("EINTR,timeout\n");
		}
	}
	printf("n: %d\n", n);
	alarm(0);
	return n;
}

static void connect_alarm(int signo){
	printf("get SIGALRM\n");
	return;
}

void main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	int flags;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

//	flags = fcntl(sockfd, F_GETFL, 0);
//	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	printf("before connect_timeo\n");
	connect_timeo(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), 5);
	printf("after connect_timeo\n");
	return;
}
