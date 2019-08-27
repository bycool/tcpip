#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void dg_echo(int sockfd, struct sockaddr * pcliaddr, socklen_t clilen){
	printf("dg_echo()\n");
	int n;
	socklen_t len;
	char mesg[4096];

	for(;;){
		len = clilen;
		n = recvfrom(sockfd, mesg, 4096, 0, pcliaddr, &len);
		sendto(sockfd, mesg, n, 0, pcliaddr, len);
	}
}

int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999);

	bind(sockfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	dg_echo(sockfd, (struct sockaddr*)(&cliaddr), sizeof(cliaddr));
}
