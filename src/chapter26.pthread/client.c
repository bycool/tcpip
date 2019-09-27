#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

FILE* fp;
int sockfd;

void quit_sig(int signo){
	close(sockfd);
	exit(0);
}

void* copyto(void*);

void echo_cli(FILE* fp_args, int sockfd_args){
	char recvline[4096];
	pthread_t tid;

	sockfd = sockfd_args;
	fp = fp_args;

	pthread_create(&tid, NULL, &copyto, NULL);

	while(read(sockfd, recvline, 4096)>0)
		fputs(recvline, stdout);
}

void* copyto(void* arg){
	char sendline[4096];

	while(fgets(sendline,4096, fp)!=NULL){
		write(sockfd, sendline, strlen(sendline));
	}

	shutdown(sockfd, SHUT_WR);
	return NULL;
}

void main(int argc, char** argv){
	int sockmain;
	struct sockaddr_in servaddr;
	char ip[32] = "127.0.0.1";


	signal(SIGINT, quit_sig);

	sockmain = socket(AF_INET, SOCK_STREAM, 0);
	if(sockmain < 0){
		printf("create socket err[%d|%s]\n", errno, strerror(errno));
		return;
	}

	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port= htons(atoi(argv[2]));
	if(connect(sockmain, (struct sockaddr*)(&servaddr), sizeof(struct sockaddr_in)) < 0){
		printf("connect error [%d|%s]\n", errno, strerror(errno));
		return;
	}

	echo_cli(stdin, sockmain);
}
