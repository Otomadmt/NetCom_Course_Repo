#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "myerror.c"

#define _BUFFER_SIZE_ 255

int main(int argc, char *argv[])
{
	int client_sock, ser_port;
	struct sockaddr_in server_addr;
	char send_msg[_BUFFER_SIZE_];
	char recv_msg[_BUFFER_SIZE_];

	/* Check for argument(s) */
	if (argc != 3) {
		printf("usage: client SERVER_IP SERVER_PORT\n");
		exit(1);
	} else {
		ser_port = atoi(argv[2]);
		if (ser_port < 1024 && ser_port > 0) {
			printf("[WARN]using a well-known port %d\n", ser_port);
		} else if (ser_port <= 0) {
			printf("[ERROR]port error\n");
			exit(1);
		}
	}

	/* Create socket */
	if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		err_fnc("socket");
	}

	/* Specify Server Addr */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(ser_port);
	if (inet_aton(argv[1], &server_addr.sin_addr) == 0) {
		err_fnc("inet_aton");
	}
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* Connect to server */
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		err_fnc("connect");
	} else {
		printf("Connected to %s:%d\n", argv[1], ser_port);
	}

	while (1) {
		/* Send msg */
		memset(send_msg, 0, sizeof(send_msg)); // Clean up char *send_msg
		if (fgets(send_msg, _BUFFER_SIZE_, stdin) == NULL) {
			continue;
		}
		if (strlen(send_msg) == 0) { // Skip empty msg
			continue; 		
		} else if (send_msg[0] == '\n' && send_msg[1] == '\0') { 
			continue;
		}
		else {
			if (send_msg[strlen(send_msg) - 1] != '\n') { // Add \n to the end
				send_msg[strlen(send_msg)] = '\n';
			}
		}
		
		if (send(client_sock, send_msg, strlen(send_msg), 0) < 0) {
			err_wrn("send");
		} else {
			printf("Send: %s", send_msg);
		}

		/* Receive msg */
		memset(recv_msg, 0, sizeof(recv_msg));
		if (recv(client_sock, recv_msg, strlen(send_msg), 0) < 0) {
			err_wrn("recv");
		} else {
			printf("Recv: %s", recv_msg);
		}

		/* Close socket on "bye" */
		if (strcmp("bye\n", send_msg) == 0) {
			if (close(client_sock) < 0) {
				err_fnc("close");
			}
			break;
		}
	}
	return 0;
}
