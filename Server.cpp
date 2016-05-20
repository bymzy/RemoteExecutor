

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

#define PORT 8889
#define IP "192.168.2.18"
#define BUFFERISZE (1024*1024*4)

int MakeDaemon()
{
	int err = 0;
	err = daemon(0, 0);
	if (err != 0) {
		err = errno;
	}

	return err;
}

int main()
{
	int err = 0;
	int fd = -1;
	int client = -1;
	struct sockaddr_in localaddr;
	char buf[BUFFERISZE] = {0};
	char output[BUFFERISZE] = {0};
	FILE * handle = NULL;

	do {
		/* serves as daemon*/
#if 0
		err = MakeDaemon();
		if (err != 0) {
			break;
		}
#endif

		/* create socket */
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (-1 == fd) {
			err = errno;
			std::cerr<<"create socket failed!"<<std::endl;
			break;
		}

		memset(&localaddr, 0, sizeof(localaddr));
		localaddr.sin_family = AF_INET;
		localaddr.sin_port = htons(PORT);
		localaddr.sin_addr.s_addr = inet_addr(IP);

		err = bind(fd, (struct sockaddr*)&localaddr, sizeof(localaddr));
		if (0 != err) {
			err = errno;
			std::cerr<<"bind failed!"<<std::endl;
			break;
		}

		err = listen(fd, 10);
		if (0 != err) {
			err = errno;
			std::cerr<<"listen failed!"<<std::endl;
			break;
		}

		while(1) {
			/*loop to handle request*/
			struct sockaddr clientaddr;
			int ret = 0;
			socklen_t len = sizeof(clientaddr);
			client = accept(fd, (struct sockaddr*)&clientaddr, &len);
			if (client < 0) {
				err = errno;
				std::cerr<<"accept failed!"<<std::endl;
				break;
			}

			while(1) {
				memset(buf, 0, BUFFERISZE);
				ret = recv(client, buf, BUFFERISZE, 0);
				if (ret < 0) {
					std::cerr<<" recv failed!"<<std::endl;
					break;
				} else if (ret == 0) {
					std::cout<<"client closed!"<<std::endl;
					break;
				} else {
					handle = popen(buf, "r");
					if (NULL == handle) {
						std::cerr<<"open failed, command: " << buf<<std::endl;
						break;
					} 

					/*read exec output*/
					char * temp = NULL;
					do {
						memset(output, 0, BUFFERISZE);
						temp = fgets(output, BUFFERISZE, handle);
						err = send(client, output, strlen(output), 0);
						if (err <= 0) {
							std::cout<<"send failed!"<<std::endl;
							err = errno;
							break;
						}
					} while(NULL != temp);

					fclose(handle);
				}

				if (-1 != client) {
					close(client);
					client = -1;
					break;
				}
			}
		} 

	} while(0);

	if (-1 != fd) {
		close(fd);
		fd = -1;
	}

	return err;
}


