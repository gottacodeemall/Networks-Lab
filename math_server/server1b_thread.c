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
   char buffer[1024];
   float arr[2];
   int i,a,b,n,num;  float result;

   while((n=recv(sock, buffer, 1024, 0))>0)
    {
        if(buffer[0]=='+')
          {
            a=buffer[2]-48;
            b=buffer[4]-48;
            result=a+b;
          }
 
       else if(buffer[0]=='-')
           {
              a=buffer[2]-48;
              b=buffer[4]-48;
             result=a-b;
           }
 
      else if(buffer[0]=='*')
           {
              a=buffer[2]-48;
              b=buffer[4]-48;
             result=a*b;
            }
 
     else if(buffer[0]=='/')
          {
             a=buffer[2]-48;
             b=buffer[4]-48;
            result=(float)a/b;
          }
    
     else if(buffer[0]=='s' && buffer[1]=='i')
        {   
          num=0;
          for(i=4;buffer[i]!='\n';i++)
           {
            buffer[i]=buffer[i]-48;
             num=num*10+buffer[i];
           }
          result= sin((float)num);
        }
    else if(buffer[0]=='c' && buffer[1]=='o')
        {
          num=0;
          for(i=4;buffer[i]!='\n';i++)
           {
              buffer[i]=buffer[i]-48;
             num=num*10+buffer[i];
           }
         
          result= cos((float)num);
        }
      else
        {
          num=0;
          for(i=5;buffer[i]!='\n';i++)
           {
              buffer[i]=buffer[i]-48;
             num=num*10+buffer[i];
           }
         result= sqrt((float)num);
        }
      arr[0]=result;
      send(sock,&arr,2*sizeof(float),0);
   }
return 0;

}

