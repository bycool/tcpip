#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

void str_echo(int sockfd){
	ssize_t n;
	char buf[4096];

again:
	while((n = read(sockfd, buf, 4096)) > 0)
		write(sockfd, buf, n);

	if(n<0 && errno == EINTR)
		goto again;
	else if (n < 0)
		printf("read error\n");
}

void main(int arc, char** argv){
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready;
	struct pollfd client[FD_SETSIZE];
	ssize_t n;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char buf[4096];

	printf("main pid: %d\n", getpid());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);

	bind(listenfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	listen(listenfd, 10);

	maxfd = listenfd;
	maxi = 0;

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;

	for(i=1; i<FD_SETSIZE; i++)
		client[i].fd = -1;


	printf("listenfd: %d\n", listenfd);  //3

	for( ;; ) {
		nready = poll(client, maxi+1, -1);
		printf("nready: %d\n", nready);

		if(client[0].revents & POLLRDNORM){  //listenfd是否在rset内
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &clilen); 

			for(i=0; i<FD_SETSIZE; i++){
				if(client[i].fd < 0){
					client[i].fd = connfd;
					printf("add client[%d].fd = %d\n", i, client[i].fd);
					break;
				}
			}

			if(i==FD_SETSIZE){
				printf("too many clients\n");
				return;
			}

			client[i].events = POLLRDNORM;

			if(i>maxi)
				maxi = i;

			if(--nready <= 0)
				continue;
		}

		for(i=0; i<=maxi; i++){
			if((sockfd = client[i].fd)<0)
				continue;
			if(client[i].revents & (POLLRDNORM | POLLERR)){
				if( (n = read(sockfd, buf, 4096)) < 0){
					if(errno == ECONNRESET){
						close(sockfd);
						client[i].fd = -1;
					}else{
						printf("read err\n");
						return;
					}
				}else if(n == 0){
					close(sockfd);
					client[i].fd = -1;
				}else{
					printf("sockfd = %d\n", sockfd);
					write(sockfd, buf, n);
				}

			if(--nready <= 0)
				break;
			}
		}
	}
}
