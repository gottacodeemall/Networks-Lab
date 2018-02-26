/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc,char** argv){
  int welcomeSocket, newSocket,n,num,childpid;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  time_t curtime;
 
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  
  serverAddr.sin_family = AF_INET;
  
  serverAddr.sin_port = htons(atoi(argv[2]));
  
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

 
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  
  if(listen(welcomeSocket,5)==0)
    printf("Main server Listening\n");
  else
    printf("Error\n");


  
for ( ; ; ) 
 {
    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    
      if ( (childpid = fork ()) == 0 ) 
       {
          close(welcomeSocket);
          while(recv(newSocket,buffer,1024,0)>0)
            {
              printf("Request is received from client-side server and clild is created\n");
              time(&curtime);
              strcpy(buffer,ctime(&curtime));
              send(newSocket,buffer,1024,0);
           }
          exit(0);
       }
   close(newSocket);
}
 return 0;
}

