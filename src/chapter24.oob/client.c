#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void main(int argc, char **argv){
	int sockfd;
	struct sockaddr_in seraddr;

	char ip[32] = "127.0.0.1";

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&seraddr, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr(ip);
	seraddr.sin_port = htons(9999);

    if(connect(sockfd, (struct sockaddr *)(&seraddr), sizeof(struct sockaddr)) == -1){
        printf("connect() error [%d|%s]\n", errno, strerror(errno));
        return ;
    }

	write(sockfd, "123", 3);
	printf("write normal data 3 bytes\n");
	sleep(1);

	send(sockfd, "4", 1, MSG_OOB);
	printf("write oob data 1byte\n");
	sleep(1);

	write(sockfd, "54", 2);
	printf("write normal data 2 bytes\n");
	sleep(1);

	send(sockfd, "7", 1, MSG_OOB);
	printf("write oob data 1byte\n");
	sleep(1);

	write(sockfd, "54", 2);
	printf("write normal data 2 bytes\n");
	sleep(1);

	sleep(3);
	exit(0);
}
