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

#define max_users 100

struct mail{
	char from[1024];
	char to[1024];
	char msg[1024];
};


struct user{
	int avail;
	char email[1024];
	int port;
	int bytes;
};



void string_copy(char *from, char *to) {
  while ((*to++ = *from++) != '\0')
        ;
}      
void *connection_handler(void *);


struct user users[max_users];
 
int main(int argc,char** argv){		
  int welcomeSocket, newSocket,connSocket,n,num,childpid;
  char buffer[1024];
  float arr[2];
  int i,a,b;
  struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;
  float result;
  for(int i=0;i<max_users;i++){
  	users[i].avail=1;
  	users[i].bytes=0;
  }
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


void *connection_handler(void *socket_desc)
{
  int newSocket = *(int*)socket_desc;   //Get the socket descriptor
  int n;
  int userid=-1;
  struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;
  int connSocket;       
  int server_client;
  char email[1024];
  char psswd[1024]; 
  n=recv(newSocket, &server_client, sizeof(server_client), 0);
  printf("server/client %d \n",server_client);
  if(server_client==1){
    int login_register;
    printf("client side connected \n");
    int portnumber;
    int n=recv(newSocket,&portnumber,sizeof(portnumber),0);
    printf("port %d \n",portnumber);		
    // if the host connected is a client
    int log_reg_flag=0;   	  
    while(!log_reg_flag){   	  
      if(n=recv(newSocket, &login_register, sizeof(int), 0)>0){   	  
        if(login_register==0){
       	  printf("login \n");
    	    bzero((char*)email,1024);
    	    bzero((char*)psswd,1024);
          n=recv(newSocket, email, sizeof(email), 0);
	        FILE *fp3;
	        fp3=fopen("logins","r+");
	        //printf("not seg fautlt\n");
          int status=0;
	        while(!feof(fp3)){
		        char buffer1[1024],buffer2[1024];
            bzero((char*)buffer1,1024);
	          fgets(buffer1,1024,fp3);
            //printf("size of buffers %d %d",sizeof(buffer1),sizeof(buffer2));
	          //printf("%s %s %s %s \n",email,buffer1,psswd,buffer2);
            if(strcmp(buffer1,email)=='\n'){
		          printf("Login success \n");
          	  status=1;
              log_reg_flag=1;
              break;
	          }
	        }
	        if(status==1){
			      for(int i=0;i<max_users;i++){
			      	if(users[i].avail==0){
			      		if(strcmp(users[i].email,email)==0){
			      			users[i].avail==0;
			      			users[i].bytes==0;
			      		}
			      	}
			      }	
			      for(int i=0;i<max_users;i++){
				     	if(users[i].avail==1){
				        users[i].avail=0;
				        bzero((char *)users[i].email,1024);
				        strcpy(users[i].email,email);
				        users[i].port=portnumber;
				        userid=i;
				        break;
				      }
				    }
				   }	
	        fclose(fp3);
          send(newSocket,&status,sizeof(int),0);
          }
          else if(login_register==1){
          	printf("register \n");
          	bzero((char*)email,1024);
    	  	  bzero((char*)psswd,1024);
          	n=recv(newSocket, email, sizeof(email), 0);
		        printf("email %s pss %s \n",email,psswd);
						FILE *fp7;
						fp7=fopen("logins","r+");
						char intake[1024];
						int flaguser=0;
						while(fgets(intake, 1024, fp7) != NULL){
							if(strcmp(intake,email)=='\n'){
								flaguser=1;
								break;
							}
							bzero((char*)intake,1024);
						}
						fclose(fp7);
						
						if(flaguser==1){
							send(newSocket,&flaguser,sizeof(flaguser),0);
						}
						else{
			  	    FILE *fp;
				      fp=fopen("logins","a");
				      fputs(email,fp);
				      fputs("\n",fp); 
		          fclose(fp);
		          /*FILE *fp8;
		          fp8=fopen(email,"w+");
		          fputs(0,fp);
		          fputs("\n");
		          fclose(fp8);
		          */
		          send(newSocket,&flaguser,sizeof(flaguser),0);
		        }
		          
          }			
        }
    }
    


    printf("Ready to intitiate \n");
    char compose[1024];
    while(1){
    		bzero((char*)compose,1024);
        n=recv(newSocket, &compose, sizeof(compose), 0);
        users[userid].bytes+=n;
        printf("compose %s \n",compose);
        if(strcmp(compose,"WHO")==0){
    			for(int i=0;i<max_users;i++){
    				if(users[i].avail==0){
    					send(newSocket,users[i].email,sizeof(users[i].email),0);
    					users[userid].bytes+=sizeof(users[i].email);
    					send(newSocket,&users[i].port,sizeof(users[i].port),0);
    					users[userid].bytes+=sizeof(users[i].port);
    					send(newSocket,&users[i].bytes,sizeof(users[i].bytes),0);
    					users[userid].bytes+=sizeof(users[i].bytes);
    				}
    			}
    			char compl_sig[1024];
    			strcpy(compl_sig,"close");
    			send(newSocket,compl_sig,sizeof(compl_sig),0);		
    					
    		}
    		else if(strcmp(compose,"WHOAMI")==0){
    			send(newSocket,users[userid].email,sizeof(users[userid].email),0);
    			users[userid].bytes+=sizeof(users[userid].email);
    			send(newSocket,&users[userid].port,sizeof(users[userid].port),0);
    			users[userid].bytes+=sizeof(users[userid].port);
    			send(newSocket,&users[userid].bytes,sizeof(users[userid].bytes),0);   			
    			users[userid].bytes+=sizeof(users[userid].bytes);
    			
    			char compl_sig[1024];
    			strcpy(compl_sig,"close");
    			send(newSocket,compl_sig,sizeof(compl_sig),0);		
    		}
    		else if(strcmp(compose,"quit")){
    			users[userid].bytes=0;
    			users[userid].avail=1;
    			break;
    		}	
    		else{
    			char response[1024];
    			strcpy(response,"Format of the command is not valid");
    			send(newSocket,response,sizeof(response),0);
    			printf("%s \n",response);
    		}	
         
      } 
         
  }		

return 0;

}
