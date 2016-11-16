#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include <sys/utsname.h>
#include <netdb.h>

#include <signal.h>

/* 函数原型声明 */
int send_all(int sock, void *buf, int size);
int recv_all(int sock, void *buf, int size);

int main(int argc, char *argv[])
{
	int server_sock_listen, 		/* 服务器用于接受连接的socket */
	    server_sock_data; 			/* 服务器用于传输数据的socket */

	struct sockaddr_in server_addr, 	/* 服务器TCP/IPv4地址结构 */
			   client_addr;		/* 客户机TCP/IPv4地址结构 */
	int client_addr_len;			/* 客户机地址结构大小 */

	struct utsname server_name;		/* 服务器域名信息 */
	struct hostent *server_hostentry;	/* 服务器主机信息（含IP） */

	char *client_ip;			/* 客户机IP地址 */
	int client_port;			/* 客户机端口 */

	char recv_msg[255];			/* 接收到的客户机数据 */

	pid_t pid;

	int i;

	/* 判断命令行参数是否合法 */
	if (argc < 2) {
		printf("Usage: %s <Port>\n", argv[0]);

		return 1;
	}

	/* 创建socket */
	server_sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock_listen == -1) {
		printf("Create socket failed: %s\n", strerror(errno));

		return 1;
	}

	/* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* 绑定socket与地址 */
	if (bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		printf("Bind failed: %s\n", strerror(errno));
		close(server_sock_listen);

		return 1;
	}

	/* 监听socket */
	listen(server_sock_listen, 0);

	uname(&server_name);
	server_hostentry = gethostbyname(server_name.nodename);

	/* 显示服务器监听地址及端口 */
	i = 0;
	printf("Listening on:\n");
	while (server_hostentry->h_addr_list[i] != NULL) {
		printf("\t%s:%s\n", inet_ntoa(*(struct in_addr *)server_hostentry->h_addr_list[i]), argv[1]);
		i ++;
	}

	while (1) {
		/* 接受客户机连接 */
		server_sock_data = accept(server_sock_listen, (struct sockaddr *)&client_addr, &client_addr_len);
		if (server_sock_data == -1) {
			printf("Accept failed: %s\n", strerror(errno));
			close(server_sock_listen);

			return 1;
		}

		/* 显示客户端的IP地址及端口 */
		client_ip = inet_ntoa(client_addr.sin_addr);
		client_port = ntohs(client_addr.sin_port);
		printf(">>>>>>>>>>>>>>>>>>>>[%s:%d] CONNECTED.\n", client_ip, client_port);

		/* 创建子进程 */
		signal(SIGCHLD,SIG_IGN);	/* 防止僵尸进程 */
		pid = fork();

		/* 创建子进程出错，关闭监听socket并退出 */
		if (pid == -1) {
			printf("Create process failed: %s\n", strerror(errno));
			close(server_sock_listen);

			return 0;
		/* 子进程，循环接收客户机的数据并回送 */
		} else if (pid == 0) {
			/* 关闭监听socket的引用 */
			close(server_sock_listen);

			while (1) {
				memset(recv_msg, 0, sizeof(recv_msg) + 1);

				/* 接收数据，如果出错则中止循环 */
				if (recv(server_sock_data, recv_msg, sizeof(recv_msg), 0) < 0) {
					printf("Receive failed: %s\n", strerror(errno));
					break;
				/* 如果客户机退出或发送结束代码"bye"， 中止循环 */
				} else if ((strlen(recv_msg) == 0)
					|| (strcmp(recv_msg, "bye\r") == 0) 
					|| (strcmp(recv_msg, "bye\n") == 0)
					|| (strcmp(recv_msg, "bye\r\n") == 0)) {
					printf("<<<<<<<<<<<<<<<<<<<<[%s:%d] DISCONNECTED.\n", client_ip, client_port);
					break;
				/* 显示接收的消息并回送 */
				} else {
					printf("[%s:%d]%s", client_ip, client_port, recv_msg);

					if (send_all(server_sock_data, recv_msg, strlen(recv_msg)) <= 0) {
						printf("Send failed: %s\n", strerror(errno));
						break;
					}
				}
			}

			/* 关闭数据socket，中止子进程 */
			close(server_sock_data);

			return 0;
		/* 父进程，关闭数据socket引用，继续接受连接 */
		} else {
			close(server_sock_data);
		}
	}

	/* 关闭监听socket */
	close(server_sock_listen);

	return 0;
}

/* 函数send_all使用循环send的方式，
 * 确保指定长度的数据都已被写入发送缓冲区。
 * 返回值：成功>0，失败<=0 */
int send_all(int sock, void *buf, int size)
{
	int bytes;	/* 本次发送结果 */
	char *p;	/* 指向下一个待发送字节 */

	p = buf;
	do {
		bytes = send(sock, p, size, 0);
		if (bytes > 0) {
			size -= bytes;
			p += bytes;
		}
	} while ((size > 0) && (bytes > 0));

	return bytes;
}

/* 函数recv_all使用循环recv的方式，
 * 确保指定长度的数据都已从缓冲区接收。
 * 返回值：成功>0，失败<=0 */
int recv_all(int sock, void *buf, int size)
{
	int bytes;	/* 本次接收结果 */
	char *p;	/* 指向下一个待接收字节 */

	memset(buf, 0, size + 1);
	p = buf;
	do {
		bytes = recv(sock, p, size, 0);
		if (bytes > 0) {
			size -= bytes;
			p += bytes;
		}
	} while ((size > 0) && (bytes > 0));

	return bytes;
}
