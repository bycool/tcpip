#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

void str_cli(FILE* fp , int sockfd){
	int n = 0;
	char sendline[4096], recvline[4096];
	while(fgets(sendline, 4096, fp) != NULL){
		write(sockfd, sendline, strlen(sendline));
/*		if( (n=read(sockfd, recvline, 4096)) == 0){
			printf("service terminated\n");
			return;
		}else if(n<0){
			printf("timeout\n");
		}
		recvline[n] = 0;
		printf("%s", recvline);
*/
	}
}

void main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2){
		printf("tcpcli: <ipAddress>");
		return ;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

//	int flags = fcntl(sockfd, F_GETFL, 0);
//	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
//	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	connect(sockfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

	str_cli(stdin, sockfd);

	exit(0);
}
