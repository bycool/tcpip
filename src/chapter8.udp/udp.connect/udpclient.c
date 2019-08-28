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
	char sendline[4096], recvline[4096];
	
	connect(sockfd, (struct sockaddr*)pservaddr, servlen);

	while(fgets(sendline, 4096, fp) != NULL){
//		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
//		n = recvfrom(sockfd, recvline, 4096, 0, NULL, NULL);
		write(sockfd, sendline, strlen(sendline));
		n = read(sockfd, recvline, 4096);
		printf("n: %d\n", n);
//		recvline[n] = 0;
		fputs(recvline, stdout);
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
