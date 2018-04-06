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

struct mail{
	char from[1024];
	char to[1024];
	char msg[1024];
};
struct arg{
	int arg1;
	char arg2[1024];
	char arg3[1024];
  char arg4[1024];
};
	

struct arg_online{
	int clisock;
	char arg2[1024];
	char arg3[1024];
	struct mail sentmail;
};

void string_copy(char *from, char *to) {
  while ((*to++ = *from++) != '\0')
        ;
}      
void *connection_handler(struct arg *passing);
void *online_handler(struct arg_online *passing);

int main(int argc,char** argv){		
  int welcomeSocket, newSocket,connSocket,n,num,childpid,centralSocket;
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

  int register_stat=0;
  printf("enter 0 to reg and 1 not to reg \n");
  scanf("%d",&register_stat);
  if(register_stat==0){
    centralSocket = socket(PF_INET, SOCK_STREAM, 0);    
    serverAddrc.sin_family = AF_INET;  
    serverAddrc.sin_port = htons(atoi( argv[7]) );
    serverAddrc.sin_addr.s_addr = inet_addr(argv[6]);
    memset(serverAddrc.sin_zero, '\0', sizeof serverAddrc.sin_zero);  
    addr_sizec = sizeof serverAddrc;
    if(connect(centralSocket, (struct sockaddr *) &serverAddrc, addr_sizec)<0){
      printf("error connecting to central server \n");
    }
    int affirm=1;
    send(centralSocket,&affirm,sizeof(affirm),0);
    char name[1024];
    printf("sending name %s \n",argv[3]);
    string_copy(argv[3],name);
    send(centralSocket,&name,sizeof(name),0);
    close(centralSocket);
  }
  pthread_t thread_id;

  
  while(newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size)){
    puts("Connection accepted");
    struct arg passing;
    passing.arg1=newSocket;
    string_copy(argv[3],passing.arg2);
    string_copy(argv[4],passing.arg3);
    string_copy(argv[5],passing.arg4);
    if( pthread_create( &thread_id , NULL , connection_handler ,(struct arg*) &passing) < 0){
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
  char email[1024];
  char psswd[1024]; 
  n=recv(newSocket, &server_client, sizeof(server_client), 0);
  if(server_client==1){
    int login_register;
    printf("client side connected \n");		
    // if the host connected is a client
    int log_reg_flag=0;   	  
    while(!log_reg_flag){   	  
      if(n=recv(newSocket, &login_register, sizeof(int), 0)>0){   	  
        if(login_register==0){
       	  printf("login \n");
    	    bzero((char*)email,1024);
    	    bzero((char*)psswd,1024);
          n=recv(newSocket, email, sizeof(email), 0);
          n=recv(newSocket, psswd, sizeof(psswd), 0);
          char filename[1024];
          strcpy(filename,(*passing).arg2);
          printf("filename %s \n",filename);
	        FILE *fp3;
	        fp3=fopen(filename,"r");
	        //printf("not seg fautlt\n");
          int status=0;0;
	        while(!feof(fp3)){
		        char buffer1[1024],buffer2[1024];
            bzero((char*)buffer1,1024);
    	      bzero((char*)buffer2,1024);
	          fgets(buffer1,1024,fp3);
            fgets(buffer2,1024,fp3);
            //printf("size of buffers %d %d",sizeof(buffer1),sizeof(buffer2));
	          //printf("%s %s %s %s \n",email,buffer1,psswd,buffer2);
            if(strcmp(buffer1,email)=='\n'&&strcmp(buffer2,psswd)=='\n'){
		          printf("Login success \n");
          	  status=1;
              log_reg_flag=1;
              break;
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
          	n=recv(newSocket, psswd, sizeof(psswd), 0);
		        printf("email %s pss %s \n",email,psswd);
            char filename[1024];
            strcpy(filename,(*passing).arg2);
            printf("filename %s \n",filename);
	    	    FILE *fp;
		        fp=fopen(filename,"a");
		        fputs(email,fp);
		        fputs("\n",fp); 
		        fputs(psswd,fp);
		        fputs("\n",fp);
            fclose(fp);
            FILE *fp2;
            fp2=fopen(email,"a");
            fputs(email,fp);
            fputs("\n",fp);
            fclose(fp2);
          }			
        }
    }
    


    printf("Ready to intitiate \n");
    int compose;
    while(1){
        n=recv(newSocket, &compose, sizeof(int), 0);
        printf("compose %d \n",compose);
        if(compose==2){
          FILE *fp4;
				  fp4=fopen(email,"r");
          //dont use feof to control a loop it never ends
          //use fgets(pcompanyRow, 1024, f) != NULL
          char msg[1024];
          bzero((char*)msg,1024);
				  while(fgets(msg, 1024, fp4) != NULL){
            printf("message %s",msg);
            send(newSocket,&msg,sizeof(msg),0);
            bzero((char*)msg,1024);
				  }
          strcpy(msg,"Checking for emails completed \n");
          send(newSocket,&msg,sizeof(msg),0);		
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
		      serverAddrx.sin_port = htons(atoi( (*passing).arg4) );
 			    serverAddrx.sin_addr.s_addr = inet_addr((*passing).arg3);
 			    memset(serverAddrx.sin_zero, '\0', sizeof serverAddrx.sin_zero);  
		      addr_sizex = sizeof serverAddrx;
          // establish connection with the other mail server
				  int checkstatusonline=0;
				  if((connect(connSocket, (struct sockaddr *) &serverAddrx, addr_sizex))<0)
           	printf("Error while connecting to the server\n");
        	else{
            printf("New socket created for the second mail server\n");
					  checkstatusonline=1;
				  }
				  // waiting for the recipient server to be online create a new thread and do the waiting in that thread
				  struct arg_online passing_online;
				  passing_online.clisock=newSocket;
          strcpy(passing_online.arg2,(*passing).arg3);
          strcpy(passing_online.arg3,(*passing).arg4);
				  passing_online.sentmail=sentmail;
				  pthread_t thread_idd;				
				  if(checkstatusonline==0&&pthread_create( &thread_idd , NULL , &online_handler ,(struct arg_online*) &passing_online) < 0){
        		perror("could not create thread");
        		return 1;
      		}
				  else if(checkstatusonline==0){
					  printf("created a thread to listen to the reciever server\n");
				  }
          			
          if(checkstatusonline==1){	
            int server_client_c=2;
   				  send(connSocket,&server_client_c,sizeof(int),0);	
            //establish as server
            send(connSocket,&sentmail,sizeof(struct mail),0);	
            int statusofsentemail;
            n=recv(connSocket,&statusofsentemail,sizeof(int),0);
            if(statusofsentemail==1){
             	printf("successfully sent \n");
					    send(newSocket,&statusofsentemail,sizeof(int),0);
            }
            else{
             	printf("Mail failed to send \n");
					    send(newSocket,&statusofsentemail,sizeof(int),0);
            }
			    }
			    else{
				    int statusofsentemail=2;
				    send(newSocket,&statusofsentemail,sizeof(int),0);
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
    char to[1024];
    bzero((char*)to,1024);
    strcpy(to,recmail.to);
    int status_recv=0;
    char filename[1024];
    strcpy(filename,(*passing).arg2);
    printf("filename %s \n",filename);
		FILE *fp5;
		fp5=fopen(filename,"r");
	  while(!feof(fp5)){
		  char user[1024];
      char password[1024];
      bzero((char*)user,1024);
      bzero((char*)password,1024);
      fgets(user,1024,fp5);
      fgets(password,1024,fp5);
      printf("user %s password %s",user,password);
      if(strcmp(user,to)=='\n'){
		    printf("User exists \n");
        status_recv=1;
	    }
	  }
		fclose(fp5);
		FILE *fp6;
    if(status_recv){
      fp6=fopen(to,"a");
      char from[1024],msg[1024];
      bzero((char*)from,1024);
      bzero((char*)msg,1024);
      strcpy(from,recmail.from);
      strcpy(msg,recmail.msg);
      fputs(from,fp6);
      fputs("\n",fp6);
      fputs(msg,fp6);
      fputs("\n",fp6);
      fclose(fp6);
    }
    send(newSocket,&status_recv,sizeof(int),0);
          						
  }   
return 0;

}



void *online_handler(struct arg_online *passing)
{
  int newSocket = (*passing).clisock;     //Get the client socket
  int n;
  int connSocket;
  struct sockaddr_in serverAddr,serverAddrx;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size,addr_sizex;       			
  connSocket = socket(PF_INET, SOCK_STREAM, 0);  
  serverAddrx.sin_family = AF_INET;
  serverAddrx.sin_port = htons(atoi( (*passing).arg3) );
  serverAddrx.sin_addr.s_addr = inet_addr((*passing).arg2);
  memset(serverAddrx.sin_zero, '\0', sizeof serverAddrx.sin_zero);  
  addr_sizex = sizeof serverAddrx;
  
  // establish connection with the other mail server
  struct mail sentmail;
  sentmail=(*passing).sentmail;
  while((connect(connSocket, (struct sockaddr *) &serverAddrx, addr_sizex))<0){printf("waiting\n");}
  int server_client_c=2;  
  send(connSocket,&server_client_c,sizeof(int),0);	
  //establish as server
  send(connSocket,&sentmail,sizeof(struct mail),0);	           		
  int statusofsentemail;
  n=recv(connSocket,&statusofsentemail,sizeof(int),0);
  if(statusofsentemail==1){
    printf("successfully sent \n");
		send(newSocket,&statusofsentemail,sizeof(int),0);
  }
  else{
    printf("Mail failed to send \n");
		send(newSocket,&statusofsentemail,sizeof(int),0);
  }			
          						

return 0;
}
