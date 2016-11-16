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
	char send_msg[] = "Hello Network!\n";
	char recv_msg[255];

	/* 创建socket */
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(12345);
	inet_aton("127.0.0.1", &server_addr.sin_addr);
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	/* 连接服务器 */
	connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	/* 发送消息 */
	printf("Send: %s", send_msg);
	send(client_sock, send_msg, strlen(send_msg), 0);

	/* 接收并显示消息 */
	memset(recv_msg, 0, sizeof(recv_msg));
	recv(client_sock, recv_msg, strlen(send_msg), 0);
	printf("Recv: %s", recv_msg);

	/* 关闭socket */
	close(client_sock);

	return 0;
}
