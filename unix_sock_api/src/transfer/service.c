#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


void* wait_for_connect(void* args){
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int nready, client1[FD_SETSIZE], *client;
    ssize_t n;
    fd_set rset, allset;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    char buf[4096];

    printf("main pid: %d\n", getpid());

	client = args;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9999);

    bind(listenfd, (struct sockaddr*)(&servaddr), sizeof(servaddr));

    listen(listenfd, 10);

    maxfd = listenfd;
    maxi = -1;
    for(i=0; i<FD_SETSIZE; i++)
        client[i] = -1;

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    printf("listenfd: %d\n", listenfd);  //3

    for( ;; ) {
		rset = allset;

        nready = select(maxfd+1, &rset, NULL,NULL,NULL);
        printf("nready: %d\n", nready);

		if(FD_ISSET(listenfd, &rset)){
			clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr*)(&cliaddr), &clilen);

            for(i=0; i<FD_SETSIZE; i++){
                printf("A:client[%d] = %d\n", i, client[i]);
                if(client[i] < 0){
                    client[i] = connfd;
                printf("B:client[%d] = %d\n", i, client[i]);
                    break;
                }
            }

            if(i==FD_SETSIZE){
                printf("too many clients\n");
                return;
            }

            FD_SET(connfd, &allset);
            printf("add FD_SET(%d, &allset)\n", connfd);
            if(connfd > maxfd)
                maxfd = connfd;

            if(i>maxi)
                maxi = i;

            if(--nready <= 0)
                continue;
        }

        for(i=0; i<=maxi; i++){
            if((sockfd = client[i])<0)
                continue;
            printf("for {} sockfd = %d\n", sockfd);
            if(FD_ISSET(sockfd, &rset)){
                printf("rset set sockfd : %d\n", sockfd);
                if( (n = read(sockfd, buf, 4096)) == 0){
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    printf("close(fd) : %d\n", sockfd);
                }else{
                    printf("write sockfd : %d\n", sockfd);
                    write(sockfd, buf, n);
                }

                if(--nready <= 0)
                    break;
            }
        }
    }
}

void* send_info_pthread(void* args){

	int* client = args;
	int i = 0;
	for(;;){
		sleep(5);
		for(i=0; i<5; i++){
//			printf("client[%d] : %d\n", i, client[i]);
			if(client[i] != -1){
				write(client[i], "service back", 12);
				printf("write: %d\n", client[i]);
			}
		}
	}
}


void main(){
	int socketall[5] = {0};
	pthread_t waitfor_pthread;
	pthread_t sendinfo_pthread;


	pthread_create(&waitfor_pthread, NULL, wait_for_connect, socketall);
	pthread_create(&sendinfo_pthread, NULL, send_info_pthread, socketall);

	pthread_join(waitfor_pthread, NULL);
	pthread_join(sendinfo_pthread, NULL);
}
