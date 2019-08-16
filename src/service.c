#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>

void main(){
	int sfd , cfd;
	int rc = 0;
	struct sockaddr_in seraddr, cliaddr;
	int sin_size = -1;
	int portnum = 8888;
	char buf[10] = { 0 };

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&seraddr, sizeof(struct sockaddr_in));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	seraddr.sin_port = htons(portnum);

	if(-1 == bind(sfd, (struct sockaddr*)(&seraddr), sizeof(struct sockaddr))){
		printf("bind error\n");
		return ;
	}

	listen(sfd, 10);

	sin_size = sizeof(struct sockaddr_in);

	cfd = accept(sfd, (struct sockaddr*)(&cliaddr), &sin_size);
/*
    struct sockaddr_in gseraddr, gcliaddr;
    char serip[32], cliip[32];
    int gseraddr_len, gcliaddr_len;

    getsockname(cfd, (struct sockaddr*)(&gcliaddr), &gseraddr_len);
	inet_ntop(AF_INET, &gseraddr.sin_addr, serip, sizeof(serip));
	printf("server ip: %s\n", serip);

    getpeername(cfd, (struct sockaddr*)(&gseraddr), &gcliaddr_len);
	inet_ntop(AF_INET, &gcliaddr.sin_addr, cliip, sizeof(cliip));
	printf("client ip : %s\n", cliip);
*/

	int pid;
	if((pid = fork()) == 0){
		close(sfd);
		recv(cfd, buf, 10, 0);
		printf("pid: %d, buf: %s\n",getpid(),  buf);
	}else if(pid > 0){
		
		close(cfd);
		printf("pid: %d\n",getpid());
//		sleep(10);
	}

/*
    getsockname(cfd, (struct sockaddr*)(&gcliaddr), &gseraddr_len);
	inet_ntop(AF_INET, &gseraddr.sin_addr, serip, sizeof(serip));
	printf("server ip: %s\n", serip);

    getpeername(cfd, (struct sockaddr*)(&gseraddr), &gcliaddr_len);
	inet_ntop(AF_INET, &gcliaddr.sin_addr, cliip, sizeof(cliip));
	printf("client ip : %s\n", cliip);
*/

	close(sfd);
}
