
#include "pcap.h"

#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "WS2_32.lib")

#define IPPROTO_TCP  6
#define IPPROTO_UDP  17
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2

unsigned int proto1;
unsigned int proto2;
unsigned int proto3;

static int listensock1;
static int listensock2;
static int listensock3;

struct sockaddr_in servaddr1;
struct sockaddr_in servaddr2;
struct sockaddr_in servaddr3;

static int client1[FD_SETSIZE];
static int client2[FD_SETSIZE];
static int client3[FD_SETSIZE];

int maxi1, maxi2, maxi3, maxfd, connfd, sockfd;
fd_set rset, allset;
struct sockaddr_in cliaddr, servaddr;
unsigned int clilen;
int nready, n;
char buf[1024];
int flags;

typedef struct ipaddr {
	u_char b1;
	u_char b2;
	u_char b3;
	u_char b4;
}ipaddr;

typedef struct iphead {
	unsigned char	ver_ihl;
	unsigned char	tos;
	unsigned short	tlen;
	unsigned short	identification;
	unsigned short	flags_fo;
	unsigned char	ttl;
	unsigned char	proto;
	unsigned short	crc;
	struct ipaddr	saddr;
	struct ipaddr	daddr;
	unsigned int	op_pad;
}iphead;

typedef struct tcphead {
	unsigned short	srcport;
	unsigned short	dstport;
	unsigned int	th_seq;
	unsigned int	th_ack;
	unsigned short	tcplen;
	unsigned char	tcp_flag;
	unsigned short	tcp_win;
	unsigned short	tcp_sum;
	unsigned short	tcp_urp;
}tcphead;

#define TCP_LEN(th) (((th)->tcplen & 0xf0) >> 4)


void packet_handler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char* pkt_data){
	struct tm *ltime;
	char timestr[16];
	iphead* iph;
	tcphead* tcph;
	unsigned int iplen;
	unsigned int tcplen;
	unsigned short sport;
	unsigned short dport;
	int i = 0;
	unsigned char* data = NULL;
	int* client = NULL;

	struct timeval tval;
	tval.tv_sec = 0;
	tval.tv_usec = 0;

	rset = allset;
	nready = select(maxfd + 1, &rset, NULL, NULL, &tval);

	if (FD_ISSET(listensock1, &rset)){
		clilen = sizeof(cliaddr);
		connfd = accept(listensock1, (struct sockaddr*)(&cliaddr), &clilen);
		printf("port1： %d  accept connect\n", proto1);

		for (i = 0; i < FD_SETSIZE; i++){
			if (client1[i] < 0){
				client1[i] = connfd;
				break;
			}
		}

		if (i == FD_SETSIZE)
			return;

		FD_SET(connfd, &allset);

		if (connfd > maxfd)
			maxfd = connfd;

		if (i > maxi1)
			maxi1 = i;

		if (--nready <= 0)
			return;
	}

	if (FD_ISSET(listensock2, &rset)){
		clilen = sizeof(cliaddr);
		connfd = accept(listensock2, (struct sockaddr*)(&cliaddr), &clilen);
		printf("port2： %d  accept connect\n", proto2);

		for (i = 0; i < FD_SETSIZE; i++){
			if (client2[i] < 0){
				client2[i] = connfd;
				break;
			}
		}

		if (i == FD_SETSIZE)
			return;

		FD_SET(connfd, &allset);

		if (connfd > maxfd)
			maxfd = connfd;

		if (i > maxi2)
			maxi2 = i;

		if (--nready <= 0)
			return;
	}

	if (FD_ISSET(listensock3, &rset)){
		clilen = sizeof(cliaddr);
		connfd = accept(listensock3, (struct sockaddr*)(&cliaddr), &clilen);
		printf("port3： %d  accept connect\n", proto2);

		for (i = 0; i < FD_SETSIZE; i++){
			if (client3[i] < 0){
				client3[i] = connfd;
				break;
			}
		}

		if (i == FD_SETSIZE)
			return;

		FD_SET(connfd, &allset);

		if (connfd > maxfd)
			maxfd = connfd;

		if (i > maxi3)
			maxi3 = i;

		if (--nready <= 0)
			return;
	}

	for (i = 0; i < maxi1; i++){
		if ((sockfd = client1[i]) < 0)
			continue;

		if (FD_ISSET(sockfd, &rset)){
			//close(sockfd);

			if ((n = read(sockfd, buf, 1024)) < 0){
				FD_CLR(sockfd, &allset);
				client1[i] = -1;
			}

			if (--nready <= 0)
				break;
		}
	}

	for (i = 0; i < maxi2; i++){
		if ((sockfd = client2[i]) < 0)
			continue;

		if (FD_ISSET(sockfd, &rset)){
			//close(sockfd);

			if ((n = read(sockfd, buf, 1024)) < 0){
				FD_CLR(sockfd, &allset);
				client2[i] = -1;
			}

			if (--nready <= 0)
				break;
		}

	}

	for (i = 0; i < maxi3; i++){
		if ((sockfd = client3[i]) < 0)
			continue;

		if (FD_ISSET(sockfd, &rset)){
			//close(sockfd);

			if ((n = read(sockfd, buf, 1024)) < 0){
				FD_CLR(sockfd, &allset);
				client3[i] = -1;
			}

			if (--nready <= 0)
				break;
		}

	}

	iph = (iphead*)(pkt_data + 14);

	switch (iph->proto)
	{
	case IPPROTO_TCP:
		iplen = (iph->ver_ihl & 0xf) * 4;
		tcph = (tcphead*)((unsigned char*)iph + iplen);
		sport = ntohs(tcph->srcport);
		dport = ntohs(tcph->dstport);
		tcplen = TCP_LEN(tcph) * 4;
		data = (unsigned char*)tcph + tcplen;
		printf("data: %s\n", data);
		printf("--------------------------------------\n");

		if (sport == proto1)
			client = client1;
		else if (sport == proto2)
			client = client2;
		else if (sport == proto3)
			client = client3;

		for (i = 0; i < FD_SETSIZE; i++){
			if (client[i] > 0)
				write(client[i], data, strlen(data));
		}
		break;
	default:
		printf("default\n");
		break;

	}
}


int main(int argc, char* argv[])
{
	pcap_if_t *alldevs;
	pcap_if_t *d;

	int i = 0;
	int num;
	unsigned int netmask;

	struct bpf_program fcode;
	char packet_filter[64] = "tcp and ip";

	char errbuf[PCAP_ERRBUF_SIZE];
	int rc = 0;

	unsigned long ul = 1;

	pcap_t *datahandler;

	printf("enter port1: ");
	scanf("%d", &proto1);
	printf("enter port2: ");
	scanf("%d", &proto2);
	printf("enter port3: ");
	scanf("%d", &proto3);

	sprintf(packet_filter, "(src port %d) or (src port %d) or (src port %d)", proto1, proto2, proto3);

	printf("packet_fileter: %s\n", packet_filter);


	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1){
		fprintf(stderr, "pcap_findalldev_ex return -1\n");
		exit(1);
	}

	for (d = alldevs; d; d = d->next){
		printf("%d: [%s] :", ++i, d->name);
		if (d->description)
			printf("[%s]\n", d->description);
		else
			printf(" no description\n");
	}

	if (i == 0){
		printf("no interface found!\n");
		return -1;
	}

	printf("choose net dev, enter num: (1~%d): ", i);
	scanf("%d", &num);

	if (num < 1 || num > i){
		printf("interface number out of range\n");
		pcap_freealldevs(alldevs);
		return -1;
	}

	for (d = alldevs, i = 0; i < num - 1; d = d->next, i++);

	if ((datahandler = pcap_open(d->name,
		65536,
		PCAP_OPENFLAG_PROMISCUOUS,
		1000,
		NULL,
		errbuf)) == NULL){
		fprintf(stderr, "unable to open the adapter");
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_datalink(datahandler) != DLT_EN10MB){
		fprintf(stderr, "this program works only on ethernet networks\n");
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	if (pcap_compile(datahandler, &fcode, packet_filter, 1, netmask) < 0){
		fprintf(stderr, "unable to compile the packet filter. check the syntax.\n");
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_setfilter(datahandler, &fcode) < 0){
		fprintf(stderr, "set filter error\n");
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("listening on %s..\n", d->description);

	pcap_freealldevs(alldevs);

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	listensock1 = socket(AF_INET, SOCK_STREAM, 0);

	rc = ioctlsocket(listensock1, FIONBIO, (unsigned long *)&ul);
	if (rc == SOCKET_ERROR){
		printf("set nonblock socket1 port: %d error\n", proto1);
		return -1;
	}

	listensock2 = socket(AF_INET, SOCK_STREAM, 0);
	rc = ioctlsocket(listensock2, FIONBIO, (unsigned long *)&ul);
	if (rc == SOCKET_ERROR){
		printf("set nonblock socket2 port: %d error\n", proto2);
		return -1;
	}

	listensock3 = socket(AF_INET, SOCK_STREAM, 0);
	rc = ioctlsocket(listensock3, FIONBIO, (unsigned long *)&ul);
	if (rc == SOCKET_ERROR){
		printf("set nonblock socket3 port: %d error\n", proto3);
		return -1;
	}

	servaddr1.sin_family = AF_INET;
	servaddr2.sin_family = AF_INET;
	servaddr3.sin_family = AF_INET;

	servaddr1.sin_addr.S_un.S_addr = INADDR_ANY;
	servaddr2.sin_addr.S_un.S_addr = INADDR_ANY;
	servaddr3.sin_addr.S_un.S_addr = INADDR_ANY;

	servaddr1.sin_port = htons(proto1);
	servaddr2.sin_port = htons(proto2);
	servaddr3.sin_port = htons(proto3);

	rc = bind(listensock1, (struct sockaddr*)(&servaddr1), sizeof(servaddr1));
	if (rc != 0){
		printf("bind port1: %d error\n", proto1);
		return -1;
	}
	rc = bind(listensock2, (struct sockaddr*)(&servaddr2), sizeof(servaddr2));
	if (rc != 0){
		printf("bind port2: %d error\n", proto2);
		return -1;
	}
	rc = bind(listensock3, (struct sockaddr*)(&servaddr3), sizeof(servaddr3));
	if (rc != 0){
		printf("bind port3: %d error\n", proto3);
		return -1;
	}

	FD_ZERO(&allset);

	rc = listen(listensock1, 10);
	if (rc == SOCKET_ERROR){
		printf("listen port1: %d error\n", proto1);
		return -1;
	}
	FD_SET(listensock1, &allset);

	rc = listen(listensock2, 10);
	if (rc == SOCKET_ERROR){
		printf("listen port2: %d error\n", proto2);
		return -1;
	}
	FD_SET(listensock2, &allset);

	rc = listen(listensock3, 10);
	if (rc == SOCKET_ERROR){
		printf("listen port3: %d error\n", proto3);
		return -1;
	}
	FD_SET(listensock3, &allset);

	maxfd = listensock3;
	maxi1 = maxi2 = maxi3 = -1;

	for (i = 0; i < FD_SETSIZE; i++){
		client1[i] = -1;
		client2[i] = -1;
		client3[i] = -1;
	}

	pcap_loop(datahandler, 0, packet_handler, NULL);

	return 0;
}

