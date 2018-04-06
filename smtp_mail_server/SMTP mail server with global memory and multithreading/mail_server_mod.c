/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>


struct user{
	char email[50];
	char psswd[50];
};


struct mail{
	char from[50];
	char to[50];
	char msg[500];
};
struct arg{
	int arg1;
	char* arg2;
	char* arg3;
};
	
int avail[100][10];
int useravail[100];
struct user users[100];
struct mail mailbox[100][10]; // 100 users each 10 mails

void *connection_handler(struct arg *passing);

int main(int argc,char** argv){


	for(int i=0;i<100;i++){
		for(int j=0;j<10;j++){
			avail[i][j]=1;
		}
	}
	
	for(int i=0;i<100;i++)
		useravail[i]=1;
		
		
  int welcomeSocket, newSocket,connSocket,n,num,childpid;
  char buffer[1024];
float arr[2];
int i,a,b;
  struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;
float result;
  
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  
  serverAddr.sin_family = AF_INET;
  
  serverAddr.sin_port = htons(atoi(argv[2]));
  
  serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

 
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  
  if(listen(welcomeSocket,50)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  addr_size = sizeof serverStorage;
  pthread_t thread_id;

  
while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size))
  {
   puts("Connection accepted");
   	struct arg passing;
   	passing.arg1=newSocket;
   	passing.arg2=(char*)argv[3];
   	passing.arg3=(char*)argv[4];
    if( pthread_create( &thread_id , NULL , connection_handler ,(struct arg*) &passing) < 0)
      {
        perror("could not create thread");
        return 1;
      }

     puts("Handler assigned");
  }
  
 return 0;
}


void *connection_handler(struct arg *passing)
{
   int newSocket = (*passing).arg1;     //Get the socket descriptor
   int userid=-1;
   int n;
   struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;
  int connSocket;       
          int server_client;
          n=recv(newSocket, &server_client, sizeof(server_client), 0);
       if(server_client==1){
       	  int login_register;
       	  char email[50];
    	  char psswd[50]; 
       	  printf("client side connected \n");		
       	  // if the host connected is a client
       	  
    start:   	  
       	  n=recv(newSocket, &login_register, sizeof(int), 0);   	  
       	  if(login_register==0){
       	  printf("login \n");
    	  bzero((char*)email,50);
    	  bzero((char*)psswd,50);
          n=recv(newSocket, email, sizeof(email), 0);
		  n=recv(newSocket, psswd, sizeof(psswd), 0);
          int status=0;
          for(int i=0;i<100;i++){
          	if(!useravail[i]&&strcmp(users[i].email,email)==0&&strcmp(users[i].psswd,psswd)==0){
          		printf("Login success \n");
          		status=1;
          		userid=i;
          	}
          }
          send(newSocket,&status,sizeof(int),0);
          if(status!=1)
          	goto start;
          
          }
          else if(login_register==1){
          	printf("register \n");
          	bzero((char*)email,50);
    	  	bzero((char*)psswd,50);
          	n=recv(newSocket, email, sizeof(email), 0);
          	n=recv(newSocket, psswd, sizeof(psswd), 0);
          	for(int i=0;i<100;i++){
          		if(useravail[i]==1){
          			strcpy(users[i].email,email);
          			strcpy(users[i].psswd,psswd);
          			useravail[i]=0;
          			printf("user id allocated %d \n",i);
          			break;
          		}
          	}
          	
          	goto start;
          }			
          printf("login cleared in server \n");	
          int compose;
          while(1){
          		n=recv(newSocket, &compose, sizeof(int), 0);
          		printf("compose %d \n",compose);
          		if(compose==2){
          			for(int i=0;i<10;i++){
          				struct mail inmail;
          				inmail=mailbox[userid][i];
          				send(newSocket,&inmail,sizeof(struct mail),0);
          			}
          			printf("inbox checked \n");
          		}
          		else if(compose==1){
          			struct mail sentmail;
          			n=recv(newSocket, &sentmail,sizeof(struct mail),0);
          			printf("from: %s \n",sentmail.from);
          			printf("to: %s \n",sentmail.to);
          			printf("msg: %s \n",sentmail.msg);
          			
          			//recieved mail
          			connSocket = socket(PF_INET, SOCK_STREAM, 0);
    
         			serverAddrx.sin_family = AF_INET;
  
		            serverAddrx.sin_port = htons(atoi( (*passing).arg3) );
  
 			        serverAddrx.sin_addr.s_addr = inet_addr((*passing).arg2);
 
 			        memset(serverAddrx.sin_zero, '\0', sizeof serverAddrx.sin_zero);  

		            addr_sizex = sizeof serverAddrx;
          			// establish connection with the other mail server
          			if((connect(connSocket, (struct sockaddr *) &serverAddrx, addr_sizex))<0)
           				printf("Error\n");
        			else
             			printf("New socket created for the second mail server\n");
             			
             		int server_client_c=2;
   					send(connSocket,&server_client_c,sizeof(int),0);	
             		//establish as server
             		send(connSocket,&sentmail,sizeof(struct mail),0);	
             		
             		int statusofsentemail;
             		n=recv(connSocket,&statusofsentemail,sizeof(int),0);
             		if(statusofsentemail==1){
             			printf("successfully sent \n");
             		}
             		else{
             			printf("Mail failed to send \n");
             		}		
             	}
             	else{
             		break;
             	}
             	 				
         } 
         
       }		
       else if(server_client==2){
       
       		// if the host connected is a server
       		printf("New server connection established \n");
       		struct mail recmail;
       		n=recv(newSocket, &recmail, sizeof(struct mail), 0);	
       		printf("%s %s %s \n",recmail.from,recmail.to,recmail.msg);
          	char to[50];
          	bzero((char*)to,50);
          	strcpy(to,recmail.to);
          	int userid_recv=-1;
          	int status_recv=0;
          	for(int i=0;i<100;i++){
          		if(strcmp(users[i].email,to)==0){
          		printf("User Found %s    %s\n",users[i].email,to);
          		status_recv=1;
          		userid_recv=i;
          		break;
          		}
          	}
          	printf("userid %d\n",userid_recv);
          	send(newSocket,&status_recv,sizeof(int),0);
          	if(status_recv==1){
          		for(int i=0;i<10;i++){
          			if(avail[userid_recv][i]==1){
          				mailbox[userid_recv][i]=recmail;
          				avail[userid_recv][i]=0;
          				printf(" senders user id %d \n",userid_recv);
          				break;
          			}
          		}			
          	}		
          						
       }   
return 0;

}
