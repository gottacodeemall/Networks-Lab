/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc,char** argv){
  int clientSocket;
   float a[2];
  char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
 
  serverAddr.sin_family = AF_INET;
 
  serverAddr.sin_port = htons(atoi(argv[2]));
  
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
   
    printf("Enter expression:");
    while(fgets(buffer,1024,stdin)!=NULL)
       { 
        send(clientSocket,buffer,1024,0);
         if(strcmp(buffer,"quit\n")==0)
             break;
        recv(clientSocket, &a, 2*sizeof(float), 0);
        printf("%f\n",a[0]);   
        printf("Enter expression:");
      }
   exit(0);
  return 0;
}

