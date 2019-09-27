#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int server;
	char command[30] = "";
	struct sockaddr_in servAdd;

	// opens and connects the socket
	server = socket(AF_INET, SOCK_STREAM, 0);
	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = inet_addr(argv[1]);
	servAdd.sin_port = 7777;
	connect(server, (struct sockaddr*)&servAdd, sizeof(servAdd));
	pid_t pid;
	char buffer[100];
	size_t nBytes;

	if((pid = fork()) == -1) 
	{
		perror("fork");
		exit(1);
	}
	if(pid == 0) 
	{
		while(1) 
		{
			// user enters a shell command and sends it to server
			sleep(1);
			printf("enter shell command: ");
			scanf("%s", command);
			size_t length = strlen(command) + 1;
			write(server, &length, sizeof(length));
			write(server, command, length);
		}
	}
	else 
	{
		// prints the result of executing the shell command
		while(read(server, &nBytes, sizeof(nBytes)) > 0) 
		{
			read(server, buffer, nBytes);
			write(STDOUT_FILENO, buffer, nBytes);
		}

		kill(pid, SIGTERM);
	}
}
