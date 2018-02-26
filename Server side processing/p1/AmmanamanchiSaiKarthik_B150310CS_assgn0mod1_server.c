#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 3000 /*port*/
#define LISTENQ 8 /*maximum number of client connections */
struct nums{
	int num1;
	int num2;
};

char *itoa(int num, char *buf)
{
        if(buf == NULL)
        {
                return NULL;
        }
        snprintf(buf, sizeof(buf), "%d", num);
        printf("%s \n",buf);
        return buf;

}

int get_one_num(char *buf){
	int num1=0;
	int strlen=0;
	int i;
	for(i=4;;i++){
		if(buf[i]=='\0')
			break;
		else
			strlen++;	
	}
	int power=strlen-1;
	for(i=4;i<4+strlen;i++){
		num1+=(buf[i]-'0')*pow(10,power);
			power--;	
	}
	return num1;
}


struct nums getnum(char* buf){
	printf("%s a \n",buf);
	int num1=0,num2=0;
	int strlen=0;
	int i;
	for(i=2;;i++){
		if(buf[i]==' ')
			break;
		else
			strlen++;
	}
	printf("%d b \n",strlen);
	int curi=i;
	int power=strlen-1;
	for( i=2;i<2+strlen;i++){
			num1+=(buf[i]-'0')*pow(10,power);
			power--;
	}
	
	strlen=0;
	for(i=curi+1;;i++){
		if(buf[i]=='\0')
			break;
		else
			strlen++;	
	}
	power=strlen-1;
	for(i=curi+1;i<curi+1+strlen;i++){
		num2+=(buf[i]-'0')*pow(10,power);
			power--;	
	}
	struct nums ans;
	ans.num1=num1;
	ans.num2=num2;
	printf("%d %d",num1,num2);
	printf("\n");
	return ans; 
}		
int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 socklen_t clilen;
 char buf[MAXLINE];
 struct sockaddr_in cliaddr, servaddr;
 if (argc !=3) {
  perror("Give 3 arguments"); 
  exit(1);
 }	
 //creation of the socket
 listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
 //preparation of the socket address 
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = inet_addr(argv[1]);
 servaddr.sin_port = htons(atoi(argv[2]));
	
 bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
 listen(listenfd, LISTENQ);
	
 printf("%s\n","Server running...waiting for connections.");
	
 for ( ; ; ) {
		
  clilen = sizeof(cliaddr);
  connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  printf("%s\n","Received request...");
				
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
   printf("string recieved from the client: %s",buf);
   int len=strlen(buf);
		if(buf[0]=='+'){
			struct nums ans=getnum(buf);
			int output=ans.num1 + ans.num2;
			printf("%d ",output);
			printf("\n");
			send(connfd, &output, sizeof(output), 0);
		}
		else if(buf[0]=='-'){
			struct nums ans=getnum(buf);
			int output=ans.num1 - ans.num2;
			send(connfd, &output, sizeof(output), 0);
		}
		else if(buf[0]=='*'){
			struct nums ans=getnum(buf);
			int output=ans.num1 * ans.num2;
			send(connfd, &output, sizeof(output), 0);
		}
		else if(buf[0]=='/'){
			struct nums ans=getnum(buf);
			int output=ans.num1 / ans.num2;
			send(connfd, &output, sizeof(output), 0);
		}
		else if(buf[0]=='s'){
			int ans=get_one_num(buf);
			int output=sin(ans);
			send(connfd, &output, sizeof(output), 0);
		}
		else if(buf[0]=='c'){
			int ans=get_one_num(buf);
			int output=cos(ans);
			send(connfd, &output, sizeof(output), 0);
		}
  }
			
 close(connfd);
	
 }
 //close listening socket
 close(listenfd); 
}

