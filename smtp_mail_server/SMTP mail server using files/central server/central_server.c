#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>

void *connection_handler(void *);

int main(int argc,char** argv){		
  int welcomeSocket, newSocket,connSocket,n,num,childpid;
  char buffer[1024];
  float arr[2];
  int i,a,b;
  struct sockaddr_in serverAddr,serverAddrx,serverAddrc;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex,addr_sizec;
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

  pthread_t thread_id;

  
  while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size)){
    puts("Connection accepted");
    if( pthread_create( &thread_id , NULL , connection_handler ,(void*) &newSocket) < 0){
      perror("could not create thread");
      return 1;
    }
    puts("Handler assigned");
  }
  
 return 0;
}

void *connection_handler(void *socket_desc){
	int sock = *(int*)socket_desc;
	int affirm;
	int n;
	n=recv(sock,&affirm,sizeof(int),0);
	if(affirm==1){
		//connected to a server
		char nameofserver[1024];
		n=recv(sock,&nameofserver,sizeof(nameofserver),0);
		printf("name of the new server: %s \n",nameofserver);
		FILE *fp;
		fp=fopen("central.txt","a+");
		printf("writing into file \n");
		fputs(nameofserver,fp);
		fputs("\n",fp);
		fclose(fp);
	}
	else{
		char nameofserver[1024];
		n=recv(sock,&nameofserver,sizeof(nameofserver),0);
		printf("name of the server to be checked: %s \n",nameofserver);
		FILE *fp2;
		fp2=fopen("central.txt","w");
		int found=0;
		char msg[1024];
		bzero((char*)msg,1024);
		while(fgets(msg, 1024, fp2) != NULL){
            printf("message %s",msg);
            if(strcmp(msg,nameofserver)=='\n'){
            	found=1;
            	break;
            }
            bzero((char*)msg,1024);
		}
		fclose(fp2);
		send(sock,&found,sizeof(found),0);
	}
}