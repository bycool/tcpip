#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include <linux/rtnetlink.h>


#define BUFLEN (sizeof(struct rt_msghdr)+512)
#define SEQ 9999

int main(int argc, char** argv){
	int sockfd;
	char *buf;
	pid_t pid;
	ssize_t n;
	struct rt_msghdr* rtm;
	struct sockaddr *sa, *rti_info[RTAX_MAX];
	struct sockaddr_in *sin;

	if(argc != 2){
		printf("usage: getrt <ipaddr>\n");
		return -1;
	}

	sockfd = socket(AF_ROUTE, SOCK_RAW, 0);

	buf = calloc(1, BUFLEN);

	rtm = (struct rt_msghdr *) buf;
	rtm->rtm_msglen = sizeof(struct rt_msghdr) + sizeof(struct sockaddr_in);
	rtm->rtm_version = RTM_VERSION;
	rtm->rtm_type = RTM_GET;
	rtm->rtm_addrs = RTA_DST;
	rtm->rtm_pid = pid = getpid();
	rtm->rtm_seq = SEQ;

	sin = (struct sockaddr_in*)(rtm+1);
	sin->sin_len = sizeof(struct sockaddr_in);
	sin->sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &sin->sin_addr);

	write(sockfd, rtm, rtm->rtm_meglen);

	do{
		n = read(sockfd, rtm, BUFLEN);
	}while(rtm_type != RTM_GET || rtm->rtm_seq != SEQ || rtm->rtm_pid != pid);
	
	rtm = (struct rt_msghdr*)buf;
	sa = (struct sockaddr*)(rtm+1);
	get_rtaddr(rtm->rtm_addrs, sa, rti_info);
	if((sa = rti_info[RTAX_DST]) != NULL)
		printf("dest: %s\n", sock_ntop_host(sa, sa->sa_len));

	if((sa = rti_info[RTAX_GATEWAY]) != NULL)
		printf("gateway: %s\n", sock_masktop(sa, sa->sa_len));

	if((sa = rti_info[RTAX_GENMASK]) != NULL)
		printf("genmask: %s\n", sock_masktop(sa, sa->sa_len));

	return 0;
}
