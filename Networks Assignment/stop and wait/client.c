/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

struct packet
 {char data[100];
  int seqno;
  int fin;
 };

int get_ip (char * hostname, char * ip) 
 {struct hostent * he;     
  struct in_addr ** addr_list;     
  int i;     
  if ((he = gethostbyname (hostname) ) == NULL)     
   {herror("gethostbyname");         
    return 1;
   }     

  addr_list = (struct in_addr **) he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++)
   {strcpy(ip, inet_ntoa (*addr_list[i]));
    return 0;
   }
  return 1;
 }

int main (int argc, char **argv)
 {char * hostname = argv[1];     
  char ip[100];     
  get_ip(hostname, ip);

  int clientSocket, n, i, ack, seqno = 0, remainingRcvs = 0;
  struct packet * buffer;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  
  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (UDP in this case) */
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(atoi(argv[2]));
  /* Set IP address to ip */
  serverAddr.sin_addr.s_addr = inet_addr(ip);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  struct timeval tv;
  tv.tv_sec = 30;
  tv.tv_usec = 0;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

  printf ("STOP-AND-WAIT-PROTOCOL SIMULATOR || RDT 3.0 ||\n");
  printf ("==============================================\n\n");
  printf ("How many packets?\n");
  scanf ("%d", &n);
  buffer = (struct packet *)malloc((n + 1) * sizeof(struct packet));
  printf ("Enter data for each packet.\n");
  for (i = 0; i < n; i++)
   {scanf("%s", buffer[i].data);
    buffer[i].seqno = seqno;
    buffer[i].fin = 0;
    seqno = 1 - seqno;
   }
  /*---- Setup FIN Signal ----*/
  buffer[i].fin = 1;

  for (i = 0; i < n;)
   {printf ("Transmitting Packet %d\nWaiting for ACK %d\n", buffer[i].seqno, buffer[i].seqno);
    sendto(clientSocket, &buffer[i], sizeof(struct packet), 0, (struct sockaddr *) &serverAddr, addr_size);
    remainingRcvs++;
    if(recvfrom(clientSocket, &ack, sizeof(int), 0, NULL, NULL) == -1)
     {printf ("TIME OUT\n");
      printf ("Re-");
     }
    else
     {remainingRcvs--;
      printf ("ACK %d received.\n", ack);
      if (ack == buffer[i].seqno)
          i++;
      else
          printf ("Re-");
     }
   }

  tv.tv_sec = 10;
  tv.tv_usec = 0;
  setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

  while (remainingRcvs > 0)
   {if (recvfrom(clientSocket, &ack, sizeof(int), 0, NULL, NULL) != -1)
     {printf ("ACK %d received.\n", ack);
     }
    remainingRcvs--;
   }

  /*---- Send FIN Signal ----*/
  printf ("Sending FIN Signal\n");
  sendto(clientSocket, &buffer[i], sizeof(struct packet), 0, (struct sockaddr *) &serverAddr, addr_size);
  

  /*---- Print the received message ----*/
  free (buffer);

  return 0;
}
