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

using namespace std;

int main (int argv, char ** argc)
{
	//cout << "Hier";
	CReader *reader = new CReader();
	int s, s2, t, len;
	int ret = 0;
	int run = 1;
	char str[100];
	stringstream buff(ios::in|ios::out);
	struct sockaddr_un local, remote;
	
	try {
		if (!reader->init()) {
			exit(0);
		}
	}
	catch (...) {
		cout << "Error!" << endl;
	}
	
	// UDS -------------------------------------------------------------
	//start server

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ) {
		perror("socket");
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	while (run) {
		int done, n;
		printf("Waiting for a connection...\n");
		t = sizeof(remote);
		if ((s2 = accept(s, (struct sockaddr *)&remote, (socklen_t*)&t)) == -1) {
			perror("accept");
			exit(1);
		}

		printf("Connected.\n");

		done = 0;

		do {
			// receive data from client
			n = recv(s2, str, 100, 0);
			str[n] = '\0';
			cout << str << endl;
			if (n <= 0) {
				if (n < 0)
					perror("recv");

				done = 1;
			} else {
				
				if (strncmp(str,"GET_OCTOMAP",n) == 0) {
					done = 0;
				} else if (strncmp(str,"QUIT",n) == 0) {
					//reader->writeDataBT(); // write octree to bt-file
					run = 0; // stop main loop
					done = 1;
				}
			}

			// send data to client
			if (!done) {
				//strcpy(str,"RESPONSE");
				//str[8] = '\0';
				
				ret = reader->getOctomap(buff);
				if (ret == 0)
					buff << "NO_DATA";
					
				//cout << "RESPONSE:" << buff.str().c_str() << endl;
				//cout << buff.str() << endl;
				if (send(s2, buff.str().c_str(), strlen(buff.str().c_str()), 0) < 0) {
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
