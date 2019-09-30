#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
	int sockfd, n ;
	char recvline[4096];
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("usage: main <ip>\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);
//	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
		printf("connect %s fail\n", argv[1]);
		return -1;
	}

	while((n=read(sockfd, recvline, 4096))>0){
		recvline[n] = 0;
		fputs(recvline, stdout);
	}
	return 0;
}
