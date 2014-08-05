/*
 * socket-test.cpp
 *
 *  Created on: 05/08/2014
 *      Author: root
 */


#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc,char** argv){


  int sockfd,n;
	   struct sockaddr_in servaddr,cliaddr;
	   socklen_t len;
	   char mesg[1000];

	   sockfd=socket(AF_INET,SOCK_DGRAM,0);

	   bzero(&servaddr,sizeof(servaddr));
	   servaddr.sin_family = AF_INET;
	   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	   servaddr.sin_port=htons(32000);
	   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	   for (;;)
	   {
	      len = sizeof(cliaddr);
	      n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
	      mesg[n] = 0;
	      printf("-------------------------------------------------------\n");
	      printf("Received the following:\n");
	      printf("%s",mesg);
	      printf("-------------------------------------------------------\n");


	      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	   }


}
