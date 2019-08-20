#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

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
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	printf("main pid: %d\n", getpid());

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);

	bind(listenfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	listen(listenfd, 10);

	for( ;; ) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &clilen);
		if((childpid = fork()) == 0){
			printf("child.pid: %d\n", getpid());
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		close(connfd);
	}
}
