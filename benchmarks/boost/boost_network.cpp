#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <assert.h>
#include <string>

#include "../tasks.hpp"


using namespace std;

const int max_threads = 16;        // Number of threads in the pool
int sockfd = -1;
int port = 9999;

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

boost::asio::thread_pool pool(max_threads);


void accept_task(int connection) {
	char buffer[100];
	auto bytesRead = read(connection, buffer, 100);
	buffer[bytesRead] = '\0';
	string response = "Acknowleding message: " + string(buffer); 
	if(strncmp(buffer, "exit", 4) == 0) {
		cout << "Closing server.\n";
		response = "Closing server.\n";
		send(connection, response.c_str(), response.size(), 0);
		close(connection);
		raise(SIGUSR1);
	}

	send(connection, response.c_str(), response.size(), 0);
	close(connection);
	
}

void thread_accept_message(int connection) {
		boost::asio::post(pool, bind(accept_task, connection));
}


void startup_server() {

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		cout << "Failed to create socket\n";
		exit(1);
	}

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port);

	if(bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
		cout << "Failed to bind to port\n";
		exit(1);
	}

	if(listen(sockfd, 10) < 0) {
		cout << "Listen failed\n";
		exit(1);
	}

	auto addrlen = sizeof(sockaddr);
	while(true) {
		int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)(&addrlen));
		if(connection < 0) {
			cout << "No connection established.\n";
			exit(1);
		}
        thread_accept_message(connection);
	}
	pool.join();
	close(sockfd);


}


void setup_thread_exit_signal() {
    struct sigaction action;
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    action.sa_sigaction = [](int sig, siginfo_t *info, void *unused) {
		fprintf(stdout, "Signal Caught\n");		
		if(shutdown(sockfd, SHUT_RDWR) == -1) {
			cout << "Failed to shutdown port.\n";
			exit(1);
		}
		return;
    };

	if(sigaction(SIGUSR1, &action, NULL) == -1) {
		cout << "Failed to initialize signal handler!\n";
		exit(1);
	}
}
 
int main(int argc, char *argv[]) {
	if(argc > 1) port = stoi(argv[1]);
	setup_thread_exit_signal();
	startup_server();
}