#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <errno.h>

void* doit(void*);

void main(int argc, char** argv){
	int listenfd, *iptr;
	pthread_t tid;

	socklen_t addrlen, len;
	struct sockaddr_in servaddr, *cliaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

    if(-1 == bind(listenfd, (struct sockaddr*)(&servaddr), sizeof(struct sockaddr))){
        printf("bind error [%d|%s]\n", errno, strerror(errno));
        return ;
    }

	addrlen = sizeof(struct sockaddr_in);

	listen(listenfd, 10);

	cliaddr = malloc(addrlen);

	for(;;){
		len = addrlen;
		iptr = malloc(sizeof(int));
		printf("service wait connect\n");
		*iptr = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
		pthread_create(&tid, NULL, &doit, iptr);
		pthread_detach(tid);
	}
}

void str_echo(int sockfd){
    ssize_t n;
    char buf[4096];
	printf("sockfd: %d\n", sockfd);

    while((n = read(sockfd, buf, 4096)) > 0){
        write(sockfd, buf, n);
    }
}


void* doit(void* arg){
	int connfd;

	connfd = *((int*)arg);
	str_echo(connfd);
	close(connfd);
	free(arg);
	printf("%d out\n", connfd);
	return NULL;
}

