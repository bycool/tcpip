#if 0
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/in.h>
#include <string.h>
#endif 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>



int portnum = 8888;
char ip[32] = "127.0.0.1";

void main(){
	int sockfd = -1;
	struct sockaddr_in seraddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("socket() error \n");
		return ;
	}

	bzero(&seraddr, sizeof(struct sockaddr_in));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr(ip);
	seraddr.sin_port =  htons(portnum);
	if(connect(sockfd, (struct sockaddr *)(&seraddr), sizeof(struct sockaddr)) == -1){
		printf("connect() error\n");
		return ;
	}
/*
    struct sockaddr_in gseraddr, gcliaddr;
    char serip[32], cliip[32];
    int gseraddr_len, gcliaddr_len;

    getsockname(sockfd, (struct sockaddr*)(&gcliaddr), &gseraddr_len);
    inet_ntop(AF_INET, &gcliaddr.sin_addr, cliip, sizeof(serip));
    printf("client ip: %s\n", cliip);

    getpeername(sockfd, (struct sockaddr*)(&gseraddr), &gcliaddr_len);
    inet_ntop(AF_INET, &gseraddr.sin_addr, serip, sizeof(cliip));
    printf("service ip : %s\n", serip);
*/
	send(sockfd, "hello", 6, 0);
/*
    getsockname(sockfd, (struct sockaddr*)(&gcliaddr), &gseraddr_len);
    inet_ntop(AF_INET, &gcliaddr.sin_addr, cliip, sizeof(serip));
    printf("client ip: %s\n", cliip);

    getpeername(sockfd, (struct sockaddr*)(&gseraddr), &gcliaddr_len);
    inet_ntop(AF_INET, &gseraddr.sin_addr, serip, sizeof(cliip));
    printf("service ip : %s\n", serip);
*/
	close(sockfd);
}
