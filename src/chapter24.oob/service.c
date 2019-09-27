#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

int listenfd, connfd;
void sig_urg(int);

void main(int argc, char** argv){
	int n;
	char buff[100];
	int sin_size = -1;
	struct sockaddr_in seraddr, cliaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0){
		printf("create socket err: [%d|%s]", errno, strerror(errno));
		return;
	}

    bzero(&seraddr, sizeof(struct sockaddr_in));
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    seraddr.sin_port = htons(9999);

    if(-1 == bind(listenfd, (struct sockaddr*)(&seraddr), sizeof(struct sockaddr))){
        printf("bind error [%d|%s]\n", errno, strerror(errno));
        return ;
    }

	listen(listenfd, 10);
	sin_size = sizeof(struct sockaddr_in);
	printf("service wait from conn\n");
	connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &sin_size);

	signal(SIGURG, sig_urg);
	fcntl(connfd, F_SETOWN, getpid());

	for(;;){
		if ((n=read(connfd, buff, sizeof(buff)-1)) == 0){
			printf("recv eof\n");
			exit(0);
		}
		buff[n] = 0;
		printf("read %d bytes : %s\n", n , buff);
	}

}

void sig_urg(int signo){
	int n;
	char buff[100];

	printf("SIGURG received\n");
	n = recv(connfd, buff, sizeof(buff)-1, MSG_OOB);
	buff[n] = 0;
	printf("recv %d OOB bytes: %s\n", n, buff);
}
