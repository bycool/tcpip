#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void main(int argc, char** argv){
	int sockfd, n;
	char recvline[4097];
	struct sockaddr_in servaddr;
	struct in_addr **pptr;
	struct in_addr *inetaddrp[2];
	struct in_addr inetaddr;
	struct hostent *hp;
	struct servent *sp;

	if(argc != 3){
		printf("usage : daytimetcpcli1 <hostname> <service>\n");
		return ;
	}

	if((hp=gethostbyname(argv[1])) == NULL){
		if(inet_aton(argv[1], &inetaddr) == 0){
			printf("hostname error for %s: %s\n", argv[1], hstrerror(h_errno));
			return;
		}else{
			inetaddrp[0] = &inetaddr;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	}else{
		pptr = (struct in_addr**)hp->h_addr_list;
	}

	if((sp = getservbyname(argv[2], "tcp")) == NULL){
		printf("getservbyname error for %s\n", argv[2]);
		return;
	}

	for(; *pptr != NULL; pptr++){
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = sp->s_port;
		memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
		printf("try... %s:%d \n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));

		if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == 0)
			break;
		printf("connect err\n");
		close(sockfd);
	}

	if(*pptr == NULL){
		printf("unable to connect\n");
		return ;
	}

	while((n=read(sockfd, recvline, 4096)) > 0){
		recvline[n] = 0;
		fputs(recvline,stdout);
	}
}

