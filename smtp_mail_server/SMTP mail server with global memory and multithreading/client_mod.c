/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>



struct mail{
	char from[50];
	char to[50];
	char msg[500];
};

int main(int argc,char** argv){
  int clientSocket,n;
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
  
  
   	printf("---------CLIENT SIDE------- \n");
   	char email[50];
    char psswd[50];
   	
   	
   	int server_client=1;
   	send(clientSocket,&server_client,sizeof(int),0);
   	//establish as client
start:   	
   	printf("Enter 1 to register and 0 to login \n");
  	int login_register=-1;
  	scanf("%d",&login_register);
   	send(clientSocket,&login_register,sizeof(int),0);
   	
   	if(login_register==0){
   	login:
    printf("Enter the email \n");
    bzero((char*)email,50);
    bzero((char*)psswd,50);
    scanf("%s",email);
    printf("Enter the password for %s \n",email);
    scanf("%s",psswd);
    send(clientSocket,email,sizeof(email),0);
    send(clientSocket,psswd,sizeof(psswd),0);
    int status=0;
    n=recv(clientSocket,&status,sizeof(int),0);
    if(status==1)
    	printf("Successfully logged in \n");
    else{
    	printf("Login Failed Retry \n");
    	goto start;
    }

	}
	else{
		printf("Enter the email \n");
    	bzero((char*)email,50);
    	bzero((char*)psswd,50);
    	scanf("%s",email);
   		printf("Enter the password for %s \n",email);
    	scanf("%s",psswd);
    	send(clientSocket,email,sizeof(email),0);
    	send(clientSocket,psswd,sizeof(psswd),0);
    	printf("Successfully Registered \n");
    	goto start;
    }	
    
    while(1){
    	printf("enter 1 to compose mail, 2 to check inbox and 0 to exit \n");
    	int compose;
    	scanf("%d",&compose);
    	printf("compose %d \n",compose);
    	send(clientSocket,&compose,sizeof(int),0);
    	if(compose==0){
    		break;
    	}
    	else if(compose==2){
    		 for(int i=0;i<10;i++){
    		 	struct mail inmail;
    		 	n=recv(clientSocket,&inmail,sizeof(struct mail),0);
    			printf("From addr: %s \n",inmail.from);
    			printf("To addr: %s \n",inmail.to);
    			printf("Message: %s \n",inmail.msg);
    			}
    		 printf("Checking for emails completed \n");	
    	}
    	else{
    		char to[50];
    		char msg[500];
    		bzero((char*)to,50);
    		bzero((char*)msg,500);
    		printf("Enter the To address \n");
    		scanf(" %s",to);
    		printf("Enter the message \n");
		char nlw[10];
		scanf(" %[^\n]s",msg);
// very important:  %d automatically eats whitespaces and special characters. Logical, isn’t it? Whitespaces and special characters are characters, not numbers! However, %c has to interpret whitespaces and special characters as inputs, because %c reads characters . Luckily enough, the “fix to scanf to do what you intend it to do” is to leave a space before %c. That way you say to scanf to automatically eat whitespaces and special characters, like enter! So, just change scanf(“%c”, &c); to scanf(” %c”, &c); and you will be just fine.
		//fgets(msg,sizeof(msg),stdin);
		//msg[strlen(msg)-1]='\0';
//https://stackoverflow.com/questions/3302255/c-scanf-vs-gets-vs-fgets
    		struct mail sending;
    		strcpy(sending.from,email);
    		strcpy(sending.to,to);
    		strcpy(sending.msg,msg);
    		send(clientSocket,&sending,sizeof(struct mail),0);
    	}
    }		
    
    
    
   exit(0);
  return 0;
}

