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
	char from[1024];
	char to[1024];
	char msg[1024];
};

void string_copy(char *from, char *to) {
  while ((*to++ = *from++) != '\0')
        ;
}      

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
   	char email[1024];
    char psswd[1024];
   	
   	
   	int server_client=1;
   	send(clientSocket,&server_client,sizeof(int),0);
   	//establish as client
   	
   	int portnumber=atoi(argv[3]);
   	send(clientSocket,&portnumber,sizeof(int),0);
    int log_reg_flag=0;
    while(!log_reg_flag){
    	int login_register=1;
     	if(login_register==0){
          printf("Enter the email \n");
          bzero((char*)email,1024);
          scanf("%s",email);
          send(clientSocket,email,sizeof(email),0);
          int status=0;
          n=recv(clientSocket,&status,sizeof(int),0);
          if(status==1){
          	printf("Successfully logged in \n");
            log_reg_flag=1;
          }
          else{
          	printf("Login Failed Retry \n");
          }
        }
    	else{
    		  printf("Enter the email \n");
        	bzero((char*)email,1024);
        	scanf("%s",email);
        	send(clientSocket,email,sizeof(email),0);
        	int flaguser;
        	int n=recv(clientSocket,&flaguser,sizeof(flaguser),0);
        	if(flaguser==0){
        		printf("Successfully Registered \n");
        		log_reg_flag=1;
        	}	
        	else
        		printf("User already exists \n");	
        }
    }	
    printf("SESSION started \n");
    
    while(1){
    	printf("commands available WHO, WHOAMI,quit \n");
    	char compose[1024];
    	bzero((char*)compose,1024);
    	scanf("%s",compose);
    	printf("compose %s \n",compose);
    	send(clientSocket,compose,sizeof(compose),0);
    	if(strcmp(compose,"WHO")==0){
    		char mail[1024];
    		bzero((char *)mail,1024);
    		while(recv(clientSocket,&mail,sizeof(mail),0)>0){
    			if(strcmp(mail,"close")==0){
    				break;
    			}
    			int portrecv;
    			int bytesrecv;
    			recv(clientSocket,&portrecv,sizeof(portrecv),0);
    			recv(clientSocket,&bytesrecv,sizeof(bytesrecv),0);
    			printf("%s			%d			%d\n",mail,portrecv,bytesrecv);
    			bzero((char *)mail,1024);
    		}
    	}
    	else if(strcmp(compose,"WHOAMI")==0){
    		char mail[1024];
    		bzero((char *)mail,1024);
    		recv(clientSocket,mail,sizeof(mail),0);
    		int portrecv;
    		int bytesrecv;
    		recv(clientSocket,&portrecv,sizeof(portrecv),0);
    		recv(clientSocket,&bytesrecv,sizeof(bytesrecv),0);
    		printf("%s			%d			%d\n",mail,portrecv,bytesrecv);
    		char resp[1024];
    		bzero((char *)resp,1024);
    		recv(clientSocket,resp,sizeof(resp),0);
    		
    	}
    	else if(strcmp(compose,"quit")==0){
    		break;
    	}
    	else{
    		char response[1024];
    		n=recv(clientSocket,response,sizeof(response),0);
    		printf("%s \n",response);
    	}	
    	
    	
    	
/*    	else if(compose==2){
        char msg[1024];
        while(recv(clientSocket,msg,sizeof(msg),0)>0){
          printf("%s ",msg);
          if(strcmp(msg,"Checking for emails completed \n")==0)
            break;
          bzero((char*)msg,1024);
        }
    	}
    	else{
    		char to[1024];
    		char msg[1024];
    		bzero((char*)to,1024);
    		bzero((char*)msg,1024);
    		printf("Enter the To address \n");
    		scanf(" %s",to);
    		printf("Enter the message \n");
		    char nlw[10];
		    scanf(" %[^\n]s",msg);
        // very important:  %d automatically eats whitespaces and special characters. Logical, isn’t it? Whitespaces and special characters are characters, not numbers! However, %c has to interpret whitespaces and special characters as inputs, because %c reads characters . Luckily enough, the “fix to scanf to do what you intend it to do” is to leave a space before %c. That way you say to scanf to automatically eat whitespaces and special characters, like enter! So, just change scanf(“%c”, &c); to scanf(” %c”, &c); and you will be just fine.
		    //fgets(msg,sizeof(msg),stdin);
		    //msg[strlen(msg)-1]='\0';
        //https://stackoverflow.com/questions/3302255/c-scanf-vs-gets-vs-fgets
        printf("%s %s \n",to,msg);
    		struct mail sending;
    		strcpy(sending.from,email);
    		strcpy(sending.to,to);
    		strcpy(sending.msg,msg);
    		send(clientSocket,&sending,sizeof(struct mail),0);
		    int status_sent;
		    n=recv(clientSocket,&status_sent,sizeof(int),0);
		    if(status_sent==1){
			   printf("mail successfully sent\n");
		    }
		    else if(status_sent==0){
			   printf("No user mail delivery failed\n");
		    }
		    else{
			   printf("reciever server down mail will be sent when it is online\n");
		    }
    	}
    	
*/    	
    }		
    
    
    
   exit(0);
  return 0;
}

