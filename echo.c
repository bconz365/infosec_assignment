#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	const char* addr = "localhost";
	const int port = 7;

	struct sockaddr_in destination;
	destination.sin_family = AF_INET;

	inet_pton(AF_INET, addr, &destination.sin_addr);
	destination.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	int conn = connect(sock, (struct sockaddr*)&destination, sizeof(destination));

	int sent = 0;
	int received = 0;
	while(1){	
		char message[1024];
		scanf("%s", message);
		sent = send(sock, message, strlen(message), 0);

		char buffer[1024];

		received = recv(sock, buffer, 1024, 0);

			buffer[received] = '\0';
			printf("%s ", buffer);
			if((received>0)){
				printf("\n");
			}
	}
	// close the socket
	close(sock);
	return 0;
}