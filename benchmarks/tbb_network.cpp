#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <stdio.h>
#include <assert.h>
#include <string>

#include "tasks.hpp"


using namespace std;
using namespace oneapi::tbb;

const int max_threads = 4;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;
int sockfd = -1;
static oneapi::tbb::task_group tgroup;
static oneapi::tbb::task_arena tarena;




void accept_task(int connection) {
	char buffer[100];
	auto bytesRead = read(connection, buffer, 100);
	cout << "Input Message: " << buffer << endl;
	string response = "Hello from the server!\n";
	if(strncmp(buffer, "exit", 4) == 0) {
		cout << "Closing server.\n";
		response = "Hello from the server!\n";
		send(connection, response.c_str(), response.size(), 0);
		close(connection);
		raise(SIGUSR1);
	}

	send(connection, response.c_str(), response.size(), 0);
	close(connection);
	
}

void thread_accept_message(int connection) {

		tarena.enqueue(
			[connection] {
				tgroup.run(bind(accept_task, connection));
			}
		);
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
	sockaddr.sin_port = htons(9998);

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

	close(sockfd);

}


void setup_thread_exit_signal() {
    struct sigaction action;
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    action.sa_sigaction = [](int sig, siginfo_t *info, void *unused) {
		fprintf(stdout, "Signal Caught\n");
        // tgroup.wait(); // wait for all tasks to complete
		tarena.execute( [&] {
			cout << "here\n";
			tgroup.wait();
			cout << "there\n";
			shutdown(sockfd, SHUT_RDWR);
		});
		fprintf(stdout, "Exiting\n");
		return;
		exit(1);
    };

	if(sigaction(SIGUSR1, &action, NULL) == -1) {
		cout << "Failed to initialize signal handler!\n";
		exit(1);
	}
}
 
int main() {
	setup_thread_exit_signal();
	startup_server();
}