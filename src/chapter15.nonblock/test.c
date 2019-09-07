#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void main(){
	pid_t pid;

	if((pid=fork())==0){
		sleep(2);
		kill(getppid(), SIGTERM);
		printf("child: send SIGTERM to parent\n" );
		printf("child: exit(0);\n");
		exit(0);
	}

	printf("parent: pause()\n");
	pause();
	printf("parent: return\n");
	return;
}
