#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>


int connect_nonb(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec){
	int flags, n, error;
	socklen_t len;
	fd_set rset, wset;
	struct timeval tval;
//	char buf[4096];

	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	error = 0;
	if((n=connect(sockfd, saptr, salen))<0)
		if(errno != EINPROGRESS)
			return 01;

	if(n==0)
		goto done;

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if((n=select(sockfd+1, &rset, &wset, NULL, &tval)) == 0){
		close(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}

	if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)){
		len = sizeof(error);
		if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len)<0)
			return -1;

//		while((n = read(sockfd, buf, 4096))>0){
//			buf[n] = 0;
//			fputs(buf, stdout);
//		}
	}else{
		printf("select error: socket not set\n");
		return -1;
	}

done:
	fcntl(sockfd, F_SETFL, flags);
	if(error){
		close(sockfd);
		errno = error;
		return -1;
	}
	return 0;
}


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
			printf("hostname error for %s: %s\n", argv[1], hstrerror(errno));
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

		if(connect_nonb(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr), 10) == 0)
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

