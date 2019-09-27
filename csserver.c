// Aaron Fanick - 103824121
// Assignment 5 - March 29, 2018
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int sd, client;
	char buffer[30] = "";
	socklen_t len;
	struct sockaddr_in servAdd;
	struct sockaddr_in cliAdd;

	servAdd.sin_family = AF_INET;
	servAdd.sin_addr.s_addr = INADDR_ANY;
	servAdd.sin_port = 7777;

	// opens socket, binds it, and listens for connection
	sd = socket(AF_INET, SOCK_STREAM, 0);
	bind(sd, (struct sockaddr*)&servAdd, sizeof(servAdd));
	listen(sd, 5);
	len = sizeof(cliAdd);
	int fd[2];
	pid_t pid;

	while(1) 
	{
		// waits for connection from client 
		printf("Waiting for connection...\n");
		client = accept(sd, NULL, NULL);
		printf("Connected.\n");
		int numberOfBytes;
		size_t commandLength;

		// reads the shell command entered in client
		while((numberOfBytes = read(client, &commandLength, sizeof(commandLength))) > 0) 
		{
			char commandResult[100];
			size_t nBytes;
			pipe(fd);

			if(pipe(fd) == -1) 
			{
				exit(1);
			}

			if((pid = fork()) == -1) 
			{
				perror("fork");
				exit(1);
			}

			if(pid == 0) 
			{
				// executes shell command
				read(client, &buffer, commandLength);
				printf("received command: %s\n", buffer);
				close(fd[0]);
				dup2(fd[1], 1);
				execlp(buffer, buffer, (char *)0);
			}
			else 
			{
				// writes result of executing shell command to client
				close(fd[1]);
				while((nBytes = read(fd[0], &commandResult, sizeof(commandResult))) > 0) 
				{
					write(client, &nBytes, sizeof(nBytes));
					write(client, &commandResult, nBytes);
				}
				write(client, &nBytes, sizeof(nBytes));
			}
		}
	close(client);
	}
}

	