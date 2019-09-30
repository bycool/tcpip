#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void dg_cli(FILE* fp, int sockfd, const struct sockaddr* pservaddr, socklen_t servlen){
	int n;
	char sendline[4096], recvline[4096];
	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	while(fgets(sendline, 4096, fp) != NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

		n = recvfrom(sockfd, recvline, 4096, 0, NULL, NULL);
		if(n<0){
			if(errno == EWOULDBLOCK) {
				fprintf(stderr, "sock timeout\n");
				continue;
			}else{
				fprintf(stderr, "recvfrom error\n");
			}
		}
		recvline[n] = 0;
		fprintf(stdout, recvline);
	}
}

void main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		fprintf(stderr, "so_recv ipaddr\n");
		return;
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	dg_cli(stdin, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	return;
}
