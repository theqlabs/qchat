/*
 * sender.c -- multicasts "hello, world!" to a multicast group once a second
 *
 * Antony Courtney,	25/11/94
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// IP addresses in the range 224.0.0.0 to 239.255.255.255 ( Class D addresses) belongs to multicast. 
// No host can have this as IP address, but every machine can join a multicast address group.


#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

main(int argc, char *argv[]) {

     //struct ip_mreq {
     //    struct in_addr imr_multiaddr; /* multicast group to join */
     //    struct in_addr imr_interface; /* interface to join on */
     //}

     struct sockaddr_in addr;
     int fd, cnt;
     struct ip_mreq mreq;
     char *message="Hello World!";

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }

     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
     addr.sin_port=htons(HELLO_PORT);
     
     /* now just sendto() our destination! */
     while (1) {
	  if (sendto(fd,message,sizeof(message),0,(struct sockaddr *) &addr,
		     sizeof(addr)) < 0) {
	       perror("sendto");
	       exit(1);
	  }
	  sleep(1);
     }
}