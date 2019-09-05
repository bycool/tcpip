#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int readable_timeo(int fd, int sec){
	fd_set rset;
	struct timeval tv;

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return (select(fd+1, &rset, NULL, NULL, &tv));
}

void dg_cli(FILE* fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen){
	int n;
	char sendline[4096], recvline[4097];
	
	printf("dg_cli()\n");

	while(fgets(sendline, 4096, fp) != NULL){
		printf("sendline1: %s\n", sendline);
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		printf("sendline2: %s\n", sendline);

		if(readable_timeo(sockfd, 5) == 0){
			fprintf(stderr, "socket timeout\n");
		}else{
			n = recvfrom(sockfd, recvline, 4096, 0, NULL, NULL);
			recvline[n] = 0;
			fputs(recvline, stdout);
		}
	}
}


void main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("main  <ipAddress>\n");
		return;
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
//	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);


	dg_cli(stdin, sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	return ;
	
}


