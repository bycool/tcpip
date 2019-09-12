#include <sys/socket.h>
#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>

void main(){
	int fd = open("1.txt", O_CREAT | O_RDONLY);
	fd_set rset;

	FD_ZERO(&rset);
	if(FD_ISSET(fd, &rset))
		printf("1.is_set\n");
	FD_SET(fd, &rset);
	if(FD_ISSET(fd, &rset))
		printf("2.is_set\n");
	close(fd);
}
