#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void dg_cli(FILE* fp, int sockfd, const struct sockaddr * pservaddr, socklen_t servlen){
	int n;
	int i = 2000;
	char sendline[4096], recvline[4096];

	for(i=0; i<2000;i++){
		sendto(sockfd, sendline, 1400, 0, pservaddr, servlen);
	}
}

int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("err arg\n");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	dg_cli(stdin, sockfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	exit(0);
}
