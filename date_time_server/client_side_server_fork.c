/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc,char** argv){
  int welcomeSocket, newSocket,childpid,connSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;
  
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    
  serverAddr.sin_family = AF_INET;
  
  serverAddr.sin_port = htons(atoi(argv[2]));
  
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

 
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  
  if(listen(welcomeSocket,5)==0)
    printf("Client-side Listening\n");
  else
    printf("Error\n");


  
for ( ; ; ) 
 {
    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    
    if ( (childpid = fork ()) == 0 ) 
       {
         close(welcomeSocket);
     
         connSocket = socket(PF_INET, SOCK_STREAM, 0);
    
         serverAddrx.sin_family = AF_INET;
  
         serverAddrx.sin_port = htons(atoi(argv[4]));
  
         serverAddrx.sin_addr.s_addr = inet_addr(argv[3]);
 
         memset(serverAddrx.sin_zero, '\0', sizeof serverAddrx.sin_zero);  

         addr_sizex = sizeof serverAddrx;

        if((connect(connSocket, (struct sockaddr *) &serverAddrx, addr_sizex))<0)
           printf("Error\n");
        else
             printf("New client socket created in client-side server\n");

         while(recv(newSocket, buffer,1024, 0)>0)
           { 
            if(strcmp(buffer,"quit\n")==0)
              break;
                send(connSocket,buffer,1024,0);
                recv(connSocket,buffer,1024,0);
                printf("Buffer received in client-side server: %s",buffer);
                send(newSocket,buffer,1024,0);    
           }
         close(connSocket);
         close(newSocket);
         exit(0);
     }
  close(newSocket);
}
 return 0;
}

