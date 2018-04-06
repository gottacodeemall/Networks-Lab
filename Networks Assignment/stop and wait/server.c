/****************** SERVER CODE ****************/

#include <stdio.h>
#include <stdlib.h>
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

int get_ip (char * hostname , char * ip) 
 {struct hostent *he;     
  struct in_addr **addr_list;     
  int i;     
  if ( (he = gethostbyname( hostname ) ) == NULL)     
   {herror("gethostbyname");         
    return 1;
   }     

  addr_list = (struct in_addr **) he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++)
   {strcpy(ip , inet_ntoa(*addr_list[i]) );
    return 0;
   }
  return 1;
 }

int main (int argc, char **argv)
 {char * hostname = argv[1];     
  char ip[100];     
  get_ip(hostname , ip);     

  int welcomeSocket, newSocket, n, i, ack;
  int ch1, ch2, ch3;
  struct packet buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_DGRAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(atoi(argv[2]));
  /* Set IP address to ip */
  serverAddr.sin_addr.s_addr = inet_addr(ip);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/

  printf ("STOP-AND-WAIT-PROTOCOL SIMULATOR || RDT 3.0 ||\n");
  printf ("==============================================\n\n");

  for (i = 0;;)
   {recvfrom(welcomeSocket, &buffer[i], sizeof(struct packet), 0, (struct sockaddr *) &serverStorage, &addr_size);
    if (buffer[i].fin == 1)
     {printf ("FIN Signal Received.\n");
      break;
     }
    printf ("Do you want to simulate packet loss? (0/1)\n");
    scanf ("%d", &ch1);

    if (ch1 == 0)
     {printf ("Data: %s, Sequence Number: %d\n", buffer[i].data, buffer[i].seqno);
      if (i > 0 && buffer[i].seqno == buffer[i - 1].seqno) //Checks whether the packet received is a duplicate
       {printf("Duplicate Packet Received. Already Acknowledged Before.\nSending ACK %d\n", ack);
        ack = buffer[i].seqno;
       }
      else
       {printf ("Do you want to acknowledge the packet received? (0/1)\n");
        scanf ("%d", &ch2);

        if (ch2 == 1)
         {ack = buffer[i].seqno;
          i++;
         }
        else
         {ack = 1 - buffer[i].seqno;
         }
       }
      printf ("Do you want to simulate ACK Loss? (0/1)\n");
      scanf ("%d", &ch3);

      if (ch3 == 0)
       {sendto(welcomeSocket, &ack, sizeof(int), 0, (struct sockaddr *) &serverStorage, addr_size);
       }
     }
   }

  /*---- Print the received message ----*/

  printf ("\nCOMPLETE DATA\n");
  printf ("=============\n");
  for (i = 0; buffer[i].fin != 1; i++)
       printf ("Packet %d: %s\n", buffer[i].seqno, buffer[i].data);

  /*---- Send message to the socket of the incoming connection ----*/

  return 0;
 }
