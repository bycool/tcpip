#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

struct portfd {
	int fd;
	struct portfd* next;
};

static unsigned short port1;
static unsigned short port2;

struct protfd* pfd1;
struct protfd* pfd2;

pthread_mutex_t portfdacc_lock;

void* 
