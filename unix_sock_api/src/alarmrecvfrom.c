#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>


void alarm_recv(int signo){
	printf("alarm_recv\n");
	return;
}

void dg_cli(FILE* fp, int sockfd, const struct sockaddr * pservaddr, socklen_t servlen){
	int n;
	char sendline[4096], recvline[4096];
	int on = 1;
	struct sockaddr_in prelay_addr;
	socklen_t len;
	int flags;

	signal(SIGALRM, alarm_recv);

	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, servlen);

	while(fgets(sendline, 4096, fp) != NULL){
		sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		alarm(1);
		for(;;){
			n = recvfrom(sockfd, recvline, 4096, 0, (struct sockaddr*)(&prelay_addr), &len);
			if(n<0)
				if(errno == EINTR)
					break;
			recvline[n] = 0;
			printf("from: %s, %s", inet_ntoa(prelay_addr.sin_addr), recvline);
		}
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
