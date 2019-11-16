#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX(A,B) ((A)>(B)?(A):(B))

void str_cli(FILE* fp , int sockfd){
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[4096];
	int n;

	stdineof = 0;
	FD_ZERO(&rset);
	for( ;; ){
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(fp), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)){
			if((n=read(sockfd, buf, 4096)) == 0){
				if(stdineof == 1){
					return ;
				}else{
					printf("service terminated\n");
					return;
				}
			}
			buf[n] = 0;
			printf("return: %s", buf);
		}

		if(FD_ISSET(fileno(fp), &rset)){
			if((n = read(fileno(fp), buf, 4096)) == 0){
				stdineof = 1;
				shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			write(sockfd, buf, n);
		}
	}
}

void main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 3){
		printf("tcpcli: <ipAddress> <port>\n");
		return ;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	str_cli(stdin, sockfd);

	exit(0);
}
