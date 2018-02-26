#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;
	 if (argc !=3) {
  perror("3 arguments needed \n"); 
  exit(1);
 }
 listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = inet_addr(argv[1]);
 servaddr.sin_port = htons(atoi(argv[2]));
	
 bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
 listen(listenfd, LISTENQ);
	
 printf("%s\n","Server running...waiting for connections.");
	
 for ( ; ; ) {
		
  clilen = sizeof(cliaddr);
  connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  printf("%s\n","Received request...");
				
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
   printf("%s",buf);
   FILE *fp;
   fp=fopen(buf,"r");
   if(fp==NULL){
   	char* strin;
   	int strinlen;
   	strin="File Not Found \n";
   	strinlen=strlen(strin);
   	send(connfd, strin, strinlen, 0);
   	}
   	else{ 
   		char charbuf;
   		int loop=1;
   		int sizeofcurbuf=0;
   		charbuf= getc(fp);
   		while(charbuf!=EOF){
   				if(sizeofcurbuf<MAXLINE-1){
   					buf[sizeofcurbuf++]=charbuf;
   				}
   				else{
   						n=sizeof(buf);
   						buf[n]='\0';
   						send(connfd, buf, n+1, 0);
   						char *strin;
   						strin="\n buffer size exceeded \n generating next message \n";
   						int strinlen=sizeof(strin);
   						send(connfd, strin, strinlen, 0);
   						sizeofcurbuf=0;
   				}
   				charbuf= getc(fp);
   		}
   		if(feof(fp)){
   				n=sizeof(buf);
   				buf[n]='\0';
   				send(connfd, buf, n+1, 0);
   			}
   		fclose(fp);
  }
			
 }
 close(connfd);
	
 }
 //close listening socket
 close(listenfd); 
}

