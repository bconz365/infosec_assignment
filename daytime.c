#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	const char* addr = "localhost";
	const int port = 13;

	struct sockaddr_in destination;
	destination.sin_family = AF_INET;

	inet_pton(AF_INET, addr, &destination.sin_addr);
	destination.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		printf("something went wrong w/ socket\n");
	} else {
		printf("no problem w/ socket\n");
	}
	int conn = connect(sock, (struct sockaddr*)&destination, sizeof(destination));
	if(conn < 0){
		printf("something went wrong\n");
	} else {
		printf("no problem\n");
	}
	printf("should be good\n");

	close(sock);
	return 0;
}