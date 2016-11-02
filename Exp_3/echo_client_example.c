#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int client_sock;
	struct sockaddr_in server_addr;
	char send_msg[] = "Hello Network!";
	char recv_msg[255];

	/* 创建socket */
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7);
	inet_aton(argv[1], &server_addr.sin_addr);
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* 连接服务器 */
	connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	/* 发送消息 */
	printf("Send: %s\n", send_msg);
	send(client_sock, send_msg, sizeof(send_msg), 0);

	/* 接收并显示消息 */
	recv(client_sock, recv_msg, sizeof(recv_msg), 0);
	printf("Recv: %s\n", recv_msg);

	/* 关闭socket */
	close(client_sock);

	return 0;
}
