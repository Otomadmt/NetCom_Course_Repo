#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int server_sock_listen, server_sock_data;
	struct sockaddr_in server_addr;
	char recv_msg[255];

	/* 创建socket */
	server_sock_listen = socket(AF_INET, SOCK_STREAM, 0);

	/* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* 绑定socket与地址 */
	bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr));

	/* 监听socket */
	listen(server_sock_listen, 0);

	while (1) {
		server_sock_data = accept(server_sock_listen, NULL, NULL);

		/* 接收并显示消息 */
		memset(recv_msg, 0, sizeof(recv_msg));
		recv(server_sock_data, recv_msg, sizeof(recv_msg), 0);
		printf("Recv: %s", recv_msg);

		/* 发送消息 */
		printf("Send: %s", recv_msg);
		send(server_sock_data, recv_msg, strlen(recv_msg), 0);

		/* 关闭数据socket */
		close(server_sock_data);
	}

	/* 关闭连接socket */
	close(server_sock_listen);

	return 0;
}
