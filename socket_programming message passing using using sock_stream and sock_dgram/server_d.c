/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
int main(){
  int welcomeSocket, newSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ---- */
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_DGRAM, 0);
  printf("%d \n",welcomeSocket);
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(7891);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));



  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;

	//recieve the msg sent by the client
	int lenread=recvfrom(welcomeSocket,buffer,1024,0,(struct sockaddr *) &serverAddr,(socklen_t *) &addr_size);
	printf("length read= %d string=%s \n",lenread,buffer);
  /*---- Send message to the socket of the incoming connection ----*/
  sendto(welcomeSocket,buffer,strlen(buffer),0,(struct sockaddr *) &serverAddr, addr_size);

  return 0;
}

