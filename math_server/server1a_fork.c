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
  int welcomeSocket, newSocket,n,num,childpid;
  char buffer[1024];
float arr[2];
int i,a,b;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
float result;
  
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


  
for ( ; ; ) 
 {
    addr_size = sizeof serverStorage;
    newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    
    if ( (childpid = fork ()) == 0 ) 
      {
          printf("Child process created\n");
          close(welcomeSocket);
          while((n=recv(newSocket, buffer, 1024, 0))>0)
              {
                if(strcmp(buffer,"quit\n")==0)
                    exit(0);
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
         send(newSocket,&arr,2*sizeof(float),0);
            }
         close(newSocket);
         exit(0);
     }
   close(newSocket);
}
 return 0;
}

