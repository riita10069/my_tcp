#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_TCP_PORT   55997

#define CLNT_HOST_ADDR	"192.168.31.193"
#define SERV_HOST_ADDR    "192.168.32.193"

int             sd_serversockfd, sd_newserversockfd, sd_clientsockfd;
int             rv_serversockfd, rv_newserversockfd, rv_clientsockfd;
int             serversockfd, newserversockfd, clientsockfd;
struct sockaddr_in sd_serv_addr, rv_serv_addr;
