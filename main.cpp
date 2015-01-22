#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

#include "CReader.h"

#define SOCK_PATH "octomap"

#define BUFF_SIZE 100
#define UDS_SIZE 2048

using namespace std;

int main (int argv, char ** argc)
{
	CReader *reader = new CReader();
	int s, s2, t, len;
	int ret = 0;
	int run = 1;
	char str[BUFF_SIZE];
	char buff[UDS_SIZE];
	struct sockaddr_un local, remote;

	try {
		if (!reader->init()) {
			cout << "abort program!" << endl;
			if (reader != NULL)
				delete reader;
			exit(1);
		}
	}
	catch (...) {
		cout << "Error!" << endl;
	}

	// UDS -------------------------------------------------------------
	//start server

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ) {
		perror("socket");
		run = false;
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		run = false;
		//exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		run = false;
		//exit(1);
	}

	while (run) {
		int done, n;
		printf("uds::waiting for a connection...\n");
		t = sizeof(remote);
		if ((s2 = accept(s, (struct sockaddr *)&remote, (socklen_t*)&t)) == -1) {
			perror("accept");
			exit(1);
		}

		printf("uds::connected.\n");

		done = 0;

		do {
			// receive data from client
			n = recv(s2, str, BUFF_SIZE, 0);
			str[n] = '\0';
			cout << "uds_recv::" << str << "::" << n << endl;
			if (n <= 0) {
				if (n < 0)
					perror("recv");
				done = 1;
			} else {
				if (strncmp(str,"GET_OCTOMAP",n) == 0) {
					done = 0;
				} else if (strncmp(str,"QUIT",n) == 0) {
					run = 0; // stop main loop
					done = 1;
				}
			}
			// send data to client
			if (!done) {
				ret = reader->getOctomap(buff);
				if (ret == 0) {
					strncpy(buff,"NO_DATA",8);
					buff[8] = '\n';
				}

				if (send(s2, buff, ret, 0) < 0) {
					perror("send");
					done = 1;
				}
				done = 1;
			}
		} while (!done);

		close(s2);
	}
	if (reader != NULL)
		delete reader;
	return 0;
}
