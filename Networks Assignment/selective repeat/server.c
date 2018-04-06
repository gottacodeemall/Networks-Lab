/****************** SERVER CODE ****************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

struct packet
 {char data[100];
  int seqno;
  int fin;
 };

struct sendAck
 {int ack;
  int isNak;
 };

void call_sort (struct packet a[], int n)
 {int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;

  serverAddr.sin_port = htons(7892);

  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

  send(clientSocket, &n, sizeof(int), 0);
  send(clientSocket, a, n * sizeof(struct packet), 0);

  recv(clientSocket, &n, sizeof(int), 0);
  recv(clientSocket, a, n * sizeof(struct packet), 0);
 }

int main()
 {int welcomeSocket, newSocket, N, n, i, j, ack = -1, fin = 0, isNak;
  int ch1, ch2, ch3;
  struct packet buffer[1024];
  struct sendAck msg;
  int isAcknowledged[1024] = {0};
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
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

  /*---- Listen on the socket, with 5 max connection requests queued ----*/

  printf ("SELECTIVE REPEAT SIMULATOR\n");
  printf ("=============================\n\n");

  if(listen(welcomeSocket, 5) == 0)
     printf("Listening\n");
  else
     printf("Error\n");

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  recv(newSocket, &N, sizeof(int), 0);

  for (i = 0;;)
   {for (j = 0; j < N; j++)
     {recv(newSocket, &buffer[i + j], sizeof(struct packet), 0);
      if (buffer[i + j].seqno == -1) //Dummy Packet due to packet underflow
       {i--;
        continue;
       }
      if (buffer[i + j].fin == 1)
       {printf ("FIN Signal Received.\n");
        fin = 1;
        break;
       }
      printf ("Do you want to simulate packet loss for packet %d? (0/1)\n", buffer[i + j].seqno);
      scanf ("%d", &ch1);

      if (ch1 == 0)
       {printf ("Data: %s, Sequence Number: %d\n", buffer[i + j].data, buffer[i + j].seqno);
        if (isAcknowledged[buffer[i + j].seqno] == 1) //Checks whether the packet received is a duplicate
         {printf("Duplicate Packet Received. Already Acknowledged Before.\n");
          isNak = 0;
          ack = buffer[i + j].seqno;
          i--;
         }
        else
         {printf ("Do you want to acknowledge the packet received? (0/1)\n");
          scanf ("%d", &ch2);

          if (ch2 == 1)
           {isNak = 0;
            isAcknowledged[buffer[i + j].seqno] = 1;
            ack = buffer[i + j].seqno;
           }
          else
           {isNak = 1;
            ack = buffer[i + j].seqno;
            i--;
           }
         }
          
        printf ("Do you want to simulate ACK Loss? (0/1)\n");
        scanf ("%d", &ch3);

        if (ch3 == 0)
         {msg.ack = ack;
          msg.isNak = isNak;
          send(newSocket, &msg, sizeof(struct sendAck), 0);
         }
       }
      else
          i--;
     }

    if (fin == 1)
        break;

    i+=N;
   }

  /*---- Print the received message ----*/

  printf ("\nCOMPLETE DATA\n");
  printf ("=============\n");
  for (i = 0; buffer[i].fin != 1; i++)
       printf ("Packet %d: %s\n", buffer[i].seqno, buffer[i].data);

  call_sort (buffer, i);

  printf ("\nSORTED DATA\n");
  printf ("=============\n");
  for (i = 0; buffer[i].fin != 1; i++)
       printf ("Packet %d: %s\n", buffer[i].seqno, buffer[i].data);

  /*---- Send message to the socket of the incoming connection ----*/

  return 0;
 }

