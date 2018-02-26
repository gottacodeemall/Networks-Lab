/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
int main(){
  int clientSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
	// after connect send to server
	strcpy(buffer, "hi from client ");
	addr_size=sizeof(serverStorage);
	sendto(clientSocket, buffer, 15,0,(struct sockaddr *) &serverAddr,addr_size);
  /*---- Read the message from the server into the buffer ----*/
  memset(buffer,0,sizeof(char)*1024);
  addr_size=sizeof(serverStorage);
  int lenread=recvfrom(clientSocket, buffer, 1024, 0,(struct sockaddr *) &serverAddr, (socklen_t *) &addr_size);

  /*---- Print the received message ----*/
  printf("Data received: %s lenread=%d",buffer,lenread);   

  return 0;
}

