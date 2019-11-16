#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char buf[4096];
	int flags;
    struct timeval tval;

	printf("main pid: %d\n", getpid());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);

	bind(listenfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	listen(listenfd, 10);

	maxfd = listenfd;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; i++)
		client[i] = -1;

    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	tval.tv_sec = 0;
	tval.tv_usec = 0;

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	printf("listenfd: %d\n", listenfd);  //3

	for( ;; ) {

		printf("sleep 10...\n");
		sleep(5);

		rset = allset;  //allset包含所有accept和listenfd，而rset在select返回前也包含所有的套接字描述符，但select会将集合中不可读的套接字去掉，只保留可读的套接字。
		nready = select(maxfd+1, &rset, NULL, NULL, &tval);
		printf("nready: %d\n", nready);

		if(FD_ISSET(listenfd, &rset)){  //listenfd是否在rset内
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &clilen); 

			for(i=0; i<FD_SETSIZE; i++){
				printf("client[%d] = %d\n", i, client[i]);
				if(client[i] < 0){
					client[i] = connfd;
					break;
				}
			}

			if(i==FD_SETSIZE){
				printf("too many clients\n");
				return;
			}

			FD_SET(connfd, &allset);
			printf("add FD_SET(%d, &allset)\n", connfd);
			if(connfd > maxfd)
				maxfd = connfd;

			if(i>maxi)
				maxi = i;

			if(--nready <= 0)
				continue;
		}

		for(i=0; i<=maxi; i++){
			if((sockfd = client[i])<0)
				continue;
			printf("for {} sockfd = %d\n", sockfd);
			if(FD_ISSET(sockfd, &rset)){
				printf("rset set sockfd : %d\n", sockfd);
				if( (n = read(sockfd, buf, 4096)) == 0){
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
					printf("close(fd) : %d\n", sockfd);
				}else{
					printf("write sockfd : %d\n", sockfd);
					write(sockfd, buf, n);
				}

			if(--nready <= 0)
				break;
			}
		}

		

	}
}
