/* */
/* 1995/ 9/20        comfncs.c 			 */
/* */

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "modem.h"
#include "inet.h"

int server_so_reuseaddr = 1;

int 
com_out(ptr)
	unsigned char   ptr;
{
	int             nwritten;
	nwritten = write(clientsockfd, &ptr, 1);
	return (nwritten);
}

int 
com_send(ptr)
	unsigned char   ptr;
{
	int             nwritten;
	static int      set = 1;
	long            now;
	int             i;

	static unsigned char err[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

	if (set++ == 1)
		srand(time(&now) % 37);
	if ((rand()) % 100 == 0) {
		i = rand();
		if (ptr & err[i % 8])
			ptr = ptr & (~err[i % 8]);
		else
			ptr = ptr + err[i % 8];
	}
	nwritten = write(clientsockfd, &ptr, 1);
	return (nwritten);
}

int 
com_in()
{
	int             nread;
	unsigned char   ptr;
	nread = read(serversockfd, &ptr, 1);
	return (ptr);
}

void 
forward_mode()
{
	clientsockfd = sd_clientsockfd;
	serversockfd = rv_newserversockfd;
}

void 
backward_mode()
{
	clientsockfd = rv_clientsockfd;
	serversockfd = sd_serversockfd;
}

void 
init_sd_client()
{

	bzero((char *) &rv_serv_addr, sizeof(rv_serv_addr));
	rv_serv_addr.sin_family = AF_INET;
	rv_serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	rv_serv_addr.sin_port = htons(SERV_TCP_PORT);

	if ((sd_clientsockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("sd_client: can't open stream socket");

	if (connect(sd_clientsockfd, (struct sockaddr *) & rv_serv_addr, sizeof(rv_serv_addr)) < 0)
		printf("sd_client: can't connect to server");
}

void 
init_rv_client()
{

	bzero((char *) &sd_serv_addr, sizeof(sd_serv_addr));
	sd_serv_addr.sin_family = AF_INET;
	sd_serv_addr.sin_addr.s_addr = inet_addr(CLNT_HOST_ADDR);
	sd_serv_addr.sin_port = htons(SERV_TCP_PORT);

	if ((rv_clientsockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("rv_client: can't open stream socket");

	if (connect(rv_clientsockfd, (struct sockaddr *) & sd_serv_addr, sizeof(sd_serv_addr)) < 0)
		printf("rv_client: can't connect to server");
}

void 
init_sd_server()
{
	int             clilen;
	struct sockaddr_in cli_addr;
	struct timeval time_out;
	
	time_out.tv_sec = 1;
	time_out.tv_usec = 000000;
	
	
	if ((sd_serversockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	  printf("sd_server: can't open stream socket");
		exit(1);
	}

	setsockopt(sd_serversockfd, SOL_SOCKET, SO_REUSEADDR, &server_so_reuseaddr, sizeof(int));

	bzero((char *) &sd_serv_addr, sizeof(sd_serv_addr));
	sd_serv_addr.sin_family = AF_INET;
	sd_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sd_serv_addr.sin_port = htons(SERV_TCP_PORT);

	if (bind(sd_serversockfd, (struct sockaddr *) & sd_serv_addr, sizeof(sd_serv_addr)) < 0)
		printf("sd_server: can't bind local address");

	if (listen(sd_serversockfd, 5) < 0)
		printf("sd_server: listen error");

	clilen = sizeof(cli_addr);
	sd_newserversockfd = accept(sd_serversockfd, (struct sockaddr *) & cli_addr, &clilen);
	if (sd_newserversockfd < 0)
		printf("sd_server: accept error");
	
	if(setsockopt(sd_newserversockfd, SOL_SOCKET,SO_RCVTIMEO,&time_out,sizeof(struct timeval)) < 0){
	  printf("init_sd_server()/setsockout(SO_RCVTIMEO)");
	  exit(1);
	}
	
	
	sd_serversockfd = sd_newserversockfd;
}

void 
init_rv_server()
{
	int             clilen;
	struct sockaddr_in cli_addr;
	struct timeval time_out;
	
	time_out.tv_sec = 1;
	time_out.tv_usec = 000000;
	
	if ((rv_serversockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("rv_server: can't open stream socket");
		exit(1);
	}
	
	setsockopt(rv_serversockfd, SOL_SOCKET, SO_REUSEADDR, &server_so_reuseaddr, sizeof(int));
	
	bzero((char *) &rv_serv_addr, sizeof(rv_serv_addr));
	rv_serv_addr.sin_family = AF_INET;
	rv_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	rv_serv_addr.sin_port = htons(SERV_TCP_PORT);

	if (bind(rv_serversockfd, (struct sockaddr *) & rv_serv_addr, sizeof(rv_serv_addr)) < 0)
		printf("rv_server: can't bind local address");

	if (listen(rv_serversockfd, 5) < 0)
		printf("rv_server: listen error");

	clilen = sizeof(cli_addr);
	rv_newserversockfd = accept(rv_serversockfd, (struct sockaddr *) & cli_addr, &clilen);
	if (rv_newserversockfd < 0)
		printf("rv_server: accept error");


 	if(setsockopt(rv_newserversockfd, SOL_SOCKET,SO_RCVTIMEO,&time_out,sizeof(struct timeval)) < 0){
	  printf("init_rv_server()/setsockout(SO_RCVTIMEO)");
	  exit(1);
	}

	rv_serversockfd = rv_newserversockfd;
}
