#include <stdio.h>
#include <netdb.h>

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
}

