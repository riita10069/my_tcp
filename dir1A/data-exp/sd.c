/* */
/* 1995/ 9/20            sd.c  */
/* */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "modem.h"
#include "inet.h"

#define BUFFSIZ 2000
#define k 100

// K is defined in modem.h
main(argc, argv)
int             argc;
char           *argv[];
{
  char            datafile[30], **av, cc;
  FILE           *fp;
  unsigned        header = 0;
  unsigned char   intbuf[BUFFSIZ], *ch;
  unsigned long   i, length;
  int             j;
  int             sockfd;
  struct sockaddr_in serv_addr;

  av = argv;
  av++;
  if (argc > 2) {
    while ((*av)[0] == '-') {
      switch ((*av)[1]) {
      case 'h':
	header = 1;
	break;
      default:
	printf("Illegal option: %s... usage: ", *(av));
	exit(1);
	break;
      }
      argc--;
      av++;
    }
  }
  if (argc > 1)
    strcpy(datafile, (*av));
  else {
    printf("Require filename\n");
    exit(1);
  }
  
  if ((fp = fopen(datafile, "rb")) == NULL) {
    printf("File not exist\n");
    exit(1);
  }
  init_sd_client();
  init_sd_server();
  forward_mode();

  fseek(fp, 0L, 0);
  fseek(fp, 0L, 2);
  length = ftell(fp);
  fseek(fp, 0L, 0);

  if (header) {
    ch = (unsigned char *) &length;
    com_out(SOH);
    for (j = 0; j < 4; j++) {
      com_out(ch[j]);
    }
  } else {
    printf("File size is %ld byte.\n", length);
    printf("Everything is ready, hit return key.\n");
    while (1) {
      if (getchar() == '\n')
	break;
    }
  }
  printf("Sending start.\n\n");
  com_out(STX);














  // change
  for (i = 0; i < (length + k - 1) / k; i++) {
    int j; int n;
    // to define n means length to send bit.
    if (i != ((length + k - 1) / k) - 1 ) {
      n = k;
    } else {
      n = length - (k * i);
    }
    // to make packet.
    char *array;
    array = (char *) malloc(2 * n * sizeof(char));
    for (j = 0; j < n; j++){
      char tmp = fgetc(fp);
      // data bit
      array[2*j] = tmp;
      // check bit
      array[2*j+1] = tmp;
    }

    // send real data
    int res;
    int l;
    while (1) {
      for (l = 0; l < 2*n; l++){
	com_send(array[l]);
      }
      // get ACK or NAK
      backward_mode();
      res = com_in();
      forward_mode();
      if (res == NAK) {
	printf("%d is NAK.\n", i);
      } else if (res == ACK) {
	printf("%d sent and get ACK.\n", i);
	break;
      }
    }
    free(array);
  }
  
  printf("Sending finished.\n");
  fclose(fp);
  close(sd_clientsockfd);
  close(sd_serversockfd);
}
