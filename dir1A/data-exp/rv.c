/* */
/* 1995/ 9/20            rv.c  */
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
  char            datafile[30], **av;
  FILE           *fp;
  unsigned long   i, length = 0;
  unsigned        header = 0;
  unsigned char   buff[BUFFSIZ], *ch;
  int             j;
  int num;
  char *array;
  int res;
  int is_ok = 1;

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

  if ((fp = fopen(datafile, "wb")) == NULL) {
    printf("File not exist\n");
    exit(1);
  }
  init_rv_server();
  init_rv_client();
  forward_mode();

  if (header) {
    ch = (unsigned char *) &length;
    while (com_in() != SOH);
    for (j = 0; j < 4; j++) {
      ch[j] = com_in();
    }
  } else {
    printf("Input file size, please.\n");
    printf("File size = ");
    scanf("%ld", &length);
    getchar();
    printf("Now I start receiving. Hit return key.\n");
    while (1) {
      if (getchar() == '\n')
	break;
    }
  }
  printf("Receiving start.\n\n");
  while (com_in() != STX);
  printf("STX recieved\n\n");
  for (i = 0; i < (length + k - 1)/ k; i++) {
    if (i != ((length + k - 1 ) / k ) - 1) {
      num = k;
    } else {
      num = length - (k * i);
    }

    // recieve packet
    while (1) {
      array = (char *) malloc(2 * num * sizeof(char));
      res = NAK;
      for (j = 0; j < 2 * num; j++){
	array[j] = com_in();
      }

      // check check byte to check
      int i = 0;
      int len = 2 * num;
      int is_ok = 1;
      char last;
      for (i = 0; i < len; i++) {
	if (i % 2 == 0) {
	  last = array[i];
	} else {
	  if (last != array[i]) {
	    is_ok = 0;
	    break;
	  }
	}
      }

      // response ACK or NAK
      if (is_ok) {
	res = ACK;
	backward_mode();
	com_out(res);
	forward_mode();
	break;
      } else {
	res = NAK;
	backward_mode();
	com_out(res);
	printf("NAK \n");
	forward_mode();
      }
    }
    for (j = 0; j < num; j++) {
      fputc(array[2*j], fp);
    }
    free(array);
  }
  printf("Receiving finished.\n");
  fclose(fp);
  close(rv_serversockfd);
  close(rv_clientsockfd);
}
