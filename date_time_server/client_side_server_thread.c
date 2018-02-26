/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 


void *connection_handler(void *);
char ip[50];
char port[50];

int main(int argc,char** argv)
 {
  int welcomeSocket, newSocket;  
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  
  serverAddr.sin_family = AF_INET;
  
  serverAddr.sin_port = htons(atoi(argv[2]));
  
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
 
  strcpy(ip,argv[3]);
  strcpy(port,argv[4]);

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

 
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  
  if(listen(welcomeSocket,5)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  addr_size = sizeof serverStorage;
  pthread_t thread_id;

  
while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size))
  {
   puts("Connection accepted");
    if( pthread_create( &thread_id , NULL , connection_handler , (void*) &newSocket) < 0)
      {
        perror("could not create thread");
        return 1;
      }

     puts("Handler assigned");
  }

if (newSocket < 0)
   {
    perror("accept failed");
    return 1;
   }

close(newSocket);
return 0;
}


void *connection_handler(void *socket_desc)
{
   int sock = *(int*)socket_desc;     //Get the socket descriptor
   char buffer[1024];int connSocket;
   socklen_t addr_sizex;
   struct sockaddr_in serverAddrx;

   connSocket = socket(PF_INET, SOCK_STREAM, 0);
    
         serverAddrx.sin_family = AF_INET;
  
         serverAddrx.sin_port = htons(atoi(port));
  
         serverAddrx.sin_addr.s_addr = inet_addr(ip);
 
         memset(serverAddrx.sin_zero, '\0', sizeof serverAddrx.sin_zero);  

         addr_sizex = sizeof serverAddrx;

        if((connect(connSocket, (struct sockaddr *) &serverAddrx, addr_sizex))<0)
           printf("Error\n");
        else
             printf("New client socket created in client-side server\n");

         while(recv(sock, buffer,1024, 0)>0)
           { 
            if(strcmp(buffer,"quit\n")==0)
              break;
                send(connSocket,buffer,1024,0);
                recv(connSocket,buffer,1024,0);
                printf("Buffer received in client-side server: %s",buffer);
                send(sock,buffer,1024,0);    
           }
         close(connSocket);
return 0;

}

