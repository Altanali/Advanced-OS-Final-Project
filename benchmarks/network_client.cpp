#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <stdlib.h>

using namespace std;

int main() {
	int port = 9998;
	struct hostent *server = gethostbyname("localhost");
	if(!server) {
		cout << "No such host available.\n";
		exit(1);
	}
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		cout << "Failed to bind to port\n";
		exit(1);	
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		cout << "Failed to connect to server!\n";
		exit(1);
	} 
	char buffer[256];
	buffer[256] = '\0';
    fgets(buffer,255,stdin);
    if(write(sockfd, buffer, strlen(buffer)) < 0) {
		cout << "Failed to message server!\n";
		exit(1);
	}
    if(read(sockfd, buffer, 255) < 0) {
		cout << "Failed to read socket message!\n";
		exit(1);
	}
    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}