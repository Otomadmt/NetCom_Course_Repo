#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "myerror.c"

#define _BUFFER_SIZE_ 255

int
main(int argc, char *argv[]) {

	int server_sock_listen, server_sock_data = -1, spec_port;
	socklen_t client_len;
	struct sockaddr_in server_addr, client_addr;
	char recv_msg[_BUFFER_SIZE_];
	pid_t pid;

	/* Check for argument(s) */
	if (argc != 2) {
		printf("usage: server PORT\n");
		exit(1);
	} else {
		spec_port = atoi(argv[1]);
		if (spec_port < 1024 && spec_port > 0) {
			printf("[WARN]using a well-known port %d\n", spec_port);
		} else if (spec_port <= 0) {
			printf("[ERROR]port error\n");
			exit(1);
		}
	}

	/* Create socket */
	if ((server_sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		err_fnc("socket");
	}

	/* Specify server config */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(spec_port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* Bind IP and port */
	if ((bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
		close(server_sock_listen);
		err_fnc("bind");
	}

	/* Listening socket */
	if (listen(server_sock_listen, 0) < 0) {
		close(server_sock_listen);
		err_fnc("listen");
	}

	while (1) {
		/* Accept connection */
		while(server_sock_data < 0) {
			server_sock_data = accept(server_sock_listen, (struct sockaddr *)&client_addr, &client_len);
		}

		/* Fork child */
		signal(SIGCHLD, SIG_IGN);
		if ((pid = fork()) < 0) {
			close(server_sock_data);
			close(server_sock_listen);
			err_fnc("fork");
		} else if (pid == 0) {
			char client_socket[30] = "UNKNOWN";
			sprintf(client_socket, "%s:%d", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
			printf("[INFO][%s] connected.\n", client_socket);
			
			/* Msg loop */
			while (1) {
				/* Receive msg */
				memset(recv_msg, 0, sizeof(recv_msg));
				if (recv(server_sock_data, recv_msg, sizeof(recv_msg), 0) < 0) {
					err_wrn("recv");
				} else {
					if(recv_msg[0] != '\0') {
						printf("[%s]Recv: %s", client_socket, recv_msg);
					}
				}

				/* Send msg */
				if (send(server_sock_data, recv_msg, strlen(recv_msg), 0) < 0) {
					err_wrn("send");
				} else {
					if(recv_msg[0] != '\0') {
						printf("[%s]Send: %s", client_socket, recv_msg);
					}
				}

				/* Close data socket on "bye" and wait for next */
				if (strcmp("bye\n", recv_msg) == 0 || recv_msg[0] == '\0') {
					/* Close listening socket */
					if (close(server_sock_data) < 0) {
						err_fnc("close");
					} else {
						printf("[INFO] [%s] closed.\n", client_socket);
						return 0;
					}
				}
			}
		}
		server_sock_data = -1;
	}

	return 0;
}
