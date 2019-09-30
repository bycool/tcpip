#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define MAX(A,B) ((A)>(B)?(A):(B))

void str_cli(FILE* fp , int sockfd){
	int maxfdp1;
	fd_set rset;
	char sendline[4096], recvline[4096];

	FD_ZERO(&rset);
	for( ;; ){
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(fp), sockfd) + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)){
			if(read(sockfd, recvline, 4096) == 0){
				printf("service terminated\n");
				return;
			}
			printf("%s", recvline);
		}
		if(FD_ISSET(fileno(fp), &rset)){
			if(read(fileno(fp), sendline, 4096) == 0)
				return ;
			write(sockfd, sendline, strlen(sendline));
		}

	}
}

void main(int argc, char** argv){
	int sockfd;
	int flags;
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("tcpcli: <ipAddress>");
		return ;
	}


	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK); //这里虽然把套接字设为非阻塞，但是select函数是阻塞的。read需要等待select检测到服务端有数据过来才会read sockfd。

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	int n = connect(sockfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));
	printf("n = %d\n", n);

	str_cli(stdin, sockfd);

	exit(0);
}
